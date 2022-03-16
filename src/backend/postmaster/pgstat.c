/* ----------
 * pgstat.c
 *	  Activity statistics infrastructure.
 *
 * Provides the infrastructure to collect and access activity statistics,
 * e.g. per-table access statistics, of all backends in shared memory.
 *
 * Most statistics updates are first first accumulated locally in each process
 * as pending entries, then later flushed to shared memory (just after commit,
 * or by idle-timeout).
 *
 * To avoid congestion on the shared memory, shared stats are updated once per
 * PGSTAT_MIN_INTERVAL (1000ms). If some pending entries remain unflushed due
 * to lock contention, an initial retry is done after
 * PGSTAT_RETRY_MIN_INTERVAL (1000ms) and then doubled at every retry. Finally
 * we perform a blocking stats update after PGSTAT_MAX_INTERVAL (60000ms).
 *
 * AFIXME: Isn't PGSTAT_MIN_INTERVAL way too long? What is the justification
 * for increasing it?
 *
 * AFIXME: architecture explanation.
 *
 *
 * NB: Code for individual kinds of statistics belongs into pgstat_*.c
 * whenever possible, not here.
 *
 * Copyright (c) 2001-2022, PostgreSQL Global Development Group
 *
 * IDENTIFICATION
 *	  src/backend/postmaster/pgstat.c
 * ----------
 */
#include "postgres.h"

#include <unistd.h>

#include "access/transam.h"
#include "access/xact.h"
#include "common/hashfn.h"
#include "lib/dshash.h"
#include "miscadmin.h"
#include "pgstat.h"
#include "port/atomics.h"
#include "replication/slot.h"
#include "storage/fd.h"
#include "storage/ipc.h"
#include "storage/lwlock.h"
#include "storage/pg_shmem.h"
#include "utils/guc.h"
#include "utils/memutils.h"
#include "utils/pgstat_internal.h"
#include "utils/timestamp.h"


/* ----------
 * Timer definitions.
 * ----------
 */

#define PGSTAT_MIN_INTERVAL			10000	/* Minimum interval of stats data
											 * updates; in milliseconds. */

#define PGSTAT_RETRY_MIN_INTERVAL	1000	/* Initial retry interval after
											 * PGSTAT_MIN_INTERVAL */

#define PGSTAT_MAX_INTERVAL			60000	/* Longest interval of stats data
											 * updates */


/* ----------
 * Initial size hints for the hash tables used in statistics.
 * ----------
 */

#define PGSTAT_SHARED_REF_HASH_SIZE	128
#define PGSTAT_SNAPSHOT_HASH_SIZE	512


/* hash table entry for finding the PgStatSharedRef for a key */
typedef struct PgStatSharedRefHashEntry
{
	PgStatHashKey key;			/* hash key */
	char		status;			/* for simplehash use */
	PgStatSharedRef *shared_ref;
} PgStatSharedRefHashEntry;

/* hash table for statistics snapshots entry */
typedef struct PgStatSnapshotEntry
{
	PgStatHashKey key;
	char		status;			/* for simplehash use */
	void	   *data;			/* the stats data itself */
} PgStatSnapshotEntry;

typedef struct PgStat_PendingDroppedStatsItem
{
	xl_xact_stats_item item;
	bool		is_create;
	dlist_node	node;
} PgStat_PendingDroppedStatsItem;


/* ----------
 * Backend-local Hash Table Definitions
 * ----------
 */

/* for references to shared statistics entries */
#define SH_PREFIX pgstat_shared_ref_hash
#define SH_ELEMENT_TYPE PgStatSharedRefHashEntry
#define SH_KEY_TYPE PgStatHashKey
#define SH_KEY key
#define SH_HASH_KEY(tb, key) \
	hash_bytes((unsigned char *)&key, sizeof(PgStatHashKey))
#define SH_EQUAL(tb, a, b) (memcmp(&a, &b, sizeof(PgStatHashKey)) == 0)
#define SH_SCOPE static inline
#define SH_DEFINE
#define SH_DECLARE
#include "lib/simplehash.h"

/* for stats snapshot entries */
#define SH_PREFIX pgstat_snapshot
#define SH_ELEMENT_TYPE PgStatSnapshotEntry
#define SH_KEY_TYPE PgStatHashKey
#define SH_KEY key
#define SH_HASH_KEY(tb, key) \
	hash_bytes((unsigned char *)&key, sizeof(PgStatHashKey))
#define SH_EQUAL(tb, a, b) (memcmp(&a, &b, sizeof(PgStatHashKey)) == 0)
#define SH_SCOPE static inline
#define SH_DEFINE
#define SH_DECLARE
#include "lib/simplehash.h"


/* ----------
 * Local function forward declarations
 * ----------
 */

static void pgstat_setup_memcxt(void);
static void pgstat_write_statsfile(void);
static void pgstat_read_statsfile(void);

static PgStatShm_StatEntryHeader *pgstat_shared_stat_init(PgStatKind kind,
														  PgStatShmHashEntry *shhashent,
														  int init_refcount);
static void pgstat_shared_stat_reset_one(PgStatKind kind, PgStatShm_StatEntryHeader *header);

static void pgstat_shared_ref_release(PgStatHashKey key, PgStatSharedRef *shared_ref, bool discard_pending);
static bool pgstat_shared_refs_need_gc(void);
static void pgstat_shared_refs_request_gc(void);
static void pgstat_shared_refs_gc(void);
static void pgstat_shared_refs_release_all(void);
typedef bool (*PgStatSharedRefReleaseMatch) (PgStatSharedRefHashEntry *, Datum data);
static void pgstat_shared_refs_release_matching(PgStatSharedRefReleaseMatch match, Datum match_data);

static bool pgstat_shared_stat_drop(const PgStatHashKey *key);

static void pgstat_reset_all_stats(TimestampTz ts);

static void pgstat_pending_delete(PgStatSharedRef *shared_ref);
static bool pgstat_pending_flush_stats(bool nowait);

static inline const PgStatKindInfo *pgstat_kind_info_for(PgStatKind kind);

static void pgstat_snapshot_global_build(PgStatKind kind);


/* ----------
 * GUC parameters
 * ----------
 */

bool		pgstat_track_counts = false;
int			pgstat_fetch_consistency = STATS_FETCH_CONSISTENCY_NONE;


/* ----------
 * Built from GUC parameter
 * ----------
 */

char	   *pgstat_stat_directory = NULL;

/* No longer used, but will be removed with GUC */
char	   *pgstat_stat_filename = NULL;
char	   *pgstat_stat_tmpname = NULL;


/* ----------
 * Stats shared memory state
 * ----------
 */

PgStatShmemGlobal *pgStatShmem = NULL;
static dsa_area *pgStatDSA = NULL;
static dshash_table *pgStatSharedHash = NULL;


/* ----------
 * Local data
 *
 * NB: There should be only variables related to stats infrastructure here,
 * not for specific kinds of stats.
 * ----------
 */

/*
 * Backend local references to shared stats entries. If there are pending
 * updates to a stats entry, the PgStatSharedRef is added to the pgStatPending
 * list.
 *
 * When a stats entry is dropped each backend needs to release its reference
 * to it before the memory can be released. To trigger that
 * pgStatShmem->gc_count is incremented - which each backend compares to their
 * copy of pgStatSharedRefAge on a regular basis.
 */
static pgstat_shared_ref_hash_hash *pgStatSharedRefHash = NULL;
static int	pgStatSharedRefAge = 0; /* cache age of pgStatShmLookupCache */

/*
 * Memory contexts containing the pgStatSharedRefHash table, the
 * pgStatSharedRef entries, and pending data respectively. Mostly to make it
 * easier to track / attribute memory usage.
 */
static MemoryContext pgStatSharedRefContext = NULL;
static MemoryContext pgStatSharedRefHashContext = NULL;
static MemoryContext pgStatPendingContext = NULL;

/*
 * List of PgStatSharedRefs with unflushed pending stats.
 *
 * Newly pending entries should only ever be added to the end of the list,
 * otherwise pgstat_pending_flush_stats() might not see them immediately.
 */
static dlist_head pgStatPending = DLIST_STATIC_INIT(pgStatPending);


static PgStat_SubXactStatus *pgStatXactStack = NULL;

/*
 * Force the next stats flush to happen regardless of
 * PGSTAT_MIN_INTERVAL. Useful in test scripts.
 */
static bool pgStatForceNextFlush = false;

/*
 * For assertions that check pgstat is not used before initialization / after
 * shutdown.
 */
#ifdef USE_ASSERT_CHECKING
static bool pgstat_is_initialized = false;
static bool pgstat_is_shutdown = false;
#endif


/*
 * The current statistics snapshot
 */
PgStatSnapshot stats_snapshot;

/* to free snapshot in bulk */
static MemoryContext pgStatSnapshotContext = NULL;


/* ----------
 * Constants
 * ----------
 */

/*
 * Define the different kinds of statistics. If reasonably possible, handling
 * specific to one kind of stats should go through this abstraction, rather
 * than making more of pgstat.c aware.
 *
 * See comments for struct PgStatKindInfo for details about the individual
 * fields.
 *
 * XXX: It'd be nicer to define this outside of this file. But there doesn't
 * seem to be a great way of doing that, given the split across multiple
 * files.
 */
static const PgStatKindInfo pgstat_kind_infos[PGSTAT_KIND_LAST + 1] = {

	/* stats kinds with a variable number of stats */

	[PGSTAT_KIND_DB] = {
		.name = "db",

		.fixed_amount = false,
		/* so pg_stat_database entries can be seen in all databases */
		.accessed_across_databases = true,

		.shared_size = sizeof(PgStatShm_StatDBEntry),
		.shared_data_off = offsetof(PgStatShm_StatDBEntry, stats),
		.shared_data_len = sizeof(((PgStatShm_StatDBEntry *) 0)->stats),
		.pending_size = sizeof(PgStat_StatDBEntry),

		.flush_pending_cb = pgstat_database_flush_cb,
		.reset_timestamp_cb = pgstat_database_reset_timestamp_cb,
	},

	[PGSTAT_KIND_TABLE] = {
		.name = "table",

		.fixed_amount = false,

		.shared_size = sizeof(PgStatShm_StatTabEntry),
		.shared_data_off = offsetof(PgStatShm_StatTabEntry, stats),
		.shared_data_len = sizeof(((PgStatShm_StatTabEntry *) 0)->stats),
		.pending_size = sizeof(PgStat_TableStatus),

		.flush_pending_cb = pgstat_relation_flush_cb,
		.pending_delete_cb = pgstat_relation_pending_delete_cb,
	},

	[PGSTAT_KIND_FUNCTION] = {
		.name = "function",

		.fixed_amount = false,

		.shared_size = sizeof(PgStatShm_StatFuncEntry),
		.shared_data_off = offsetof(PgStatShm_StatFuncEntry, stats),
		.shared_data_len = sizeof(((PgStatShm_StatFuncEntry *) 0)->stats),
		.pending_size = sizeof(PgStat_BackendFunctionEntry),

		.flush_pending_cb = pgstat_function_flush_cb,
	},

	[PGSTAT_KIND_SUBSCRIPTION] = {
		.name = "subscription",

		.fixed_amount = false,
		/* so pg_stat_subscription_stats entries can be seen in all databases */
		.accessed_across_databases = true,

		.shared_size = sizeof(PgStatShm_StatSubEntry),
		.shared_data_off = offsetof(PgStatShm_StatSubEntry, stats),
		.shared_data_len = sizeof(((PgStatShm_StatSubEntry *) 0)->stats),
		.pending_size = sizeof(PgStat_BackendSubEntry),

		.flush_pending_cb = pgstat_subscription_flush_cb,
		.reset_timestamp_cb = pgstat_subscription_reset_timestamp_cb,
	},


	/* stats we have a fixed amount of (mostly 1) */

	[PGSTAT_KIND_ARCHIVER] = {
		.name = "archiver",

		.fixed_amount = true,

		.reset_all_cb = pgstat_archiver_reset_all_cb,
		.snapshot_cb = pgstat_archiver_snapshot_cb,
	},

	[PGSTAT_KIND_BGWRITER] = {
		.name = "bgwriter",

		.fixed_amount = true,

		.reset_all_cb = pgstat_bgwriter_reset_all_cb,
		.snapshot_cb = pgstat_bgwriter_snapshot_cb,
	},

	[PGSTAT_KIND_CHECKPOINTER] = {
		.name = "checkpointer",

		.fixed_amount = true,

		.reset_all_cb = pgstat_checkpointer_reset_all_cb,
		.snapshot_cb = pgstat_checkpointer_snapshot_cb,
	},

	[PGSTAT_KIND_REPLSLOT] = {
		.name = "replslot",

		/*
		 * AFIXME: With a bit of extra work this could now be a !fixed_amount
		 * stats kind.
		 */
		.fixed_amount = true,

		.reset_all_cb = pgstat_replslot_reset_all_cb,
		.snapshot_cb = pgstat_replslot_snapshot_cb,
	},

	[PGSTAT_KIND_SLRU] = {
		.name = "slru",

		.fixed_amount = true,

		.reset_all_cb = pgstat_slru_reset_all_cb,
		.snapshot_cb = pgstat_slru_snapshot_cb,
	},

	[PGSTAT_KIND_WAL] = {
		.name = "wal",

		.fixed_amount = true,

		.reset_all_cb = pgstat_wal_reset_all_cb,
		.snapshot_cb = pgstat_wal_snapshot_cb,
	},
};

/* parameter for the shared hash */
static const dshash_parameters dsh_params = {
	sizeof(PgStatHashKey),
	sizeof(PgStatShmHashEntry),
	dshash_memcmp,
	dshash_memhash,
	LWTRANCHE_STATS
};


/* ------------------------------------------------------------
 * Public functions called from postmaster follow
 * ------------------------------------------------------------
 */

/*
 * The size of the shared memory allocation for stats stored in the shared
 * stats hash table. This allocation will be done as part of the main shared
 * memory, rather than dynamic shared memory, allowing it to be initialized in
 * postmaster.
 */
static Size
pgstat_dsa_init_size(void)
{
	Size		sz;

	/*
	 * The dshash header / initial buckets array needs to fit into "plain"
	 * shared memory, but it's beneficial to not need dsm segments
	 * immediately. A size of 256kB seems works well and is not
	 * disproportional compared to other constant sized shared memory
	 * allocations. NB: To avoid DSMs further, the user can configure
	 * min_dynamic_shared_memory.
	 */
	sz = 256 * 1024;
	Assert(dsa_minimum_size() <= sz);
	return MAXALIGN(sz);
}

static Size
pgstat_replslot_size(void)
{
	Size		sz;

	sz = MAXALIGN(mul_size(sizeof(PgStat_StatReplSlotEntry), max_replication_slots));
	sz = MAXALIGN(add_size(sz, mul_size(sizeof(bool), max_replication_slots)));

	return sz;
}

/*
 * Compute shared memory space needed for activity statistics
 */
Size
StatsShmemSize(void)
{
	Size		sz;

	sz = MAXALIGN(sizeof(PgStatShmemGlobal));
	sz = add_size(sz, pgstat_dsa_init_size());
	sz = add_size(sz, pgstat_replslot_size());

	return sz;
}

/*
 * Initialize activity statistics initialize during startup
 */
void
StatsShmemInit(void)
{
	bool		found;
	Size		sz;

	sz = StatsShmemSize();
	pgStatShmem = (PgStatShmemGlobal *)
		ShmemInitStruct("Shared Memory Stats", sz, &found);

	if (!IsUnderPostmaster)
	{
		dsa_area   *dsa;
		dshash_table *dsh;
		char	   *p = (char *) pgStatShmem;

		Assert(!found);

		/* the allocation of pgStatShmem itself */
		p += MAXALIGN(sizeof(PgStatShmemGlobal));

		/*
		 * Create a small dsa allocation in plain shared memory. Doing so
		 * initially makes it easier to manage server startup, and it also is
		 * a small efficiency win.
		 */
		pgStatShmem->raw_dsa_area = p;
		p += MAXALIGN(pgstat_dsa_init_size());
		dsa = dsa_create_in_place(pgStatShmem->raw_dsa_area,
								  pgstat_dsa_init_size(),
								  LWTRANCHE_STATS, 0);
		dsa_pin(dsa);

		/*
		 * To ensure dshash is created in "plain" shared memory, temporarily
		 * limit size of dsa to the initial size of the dsa.
		 */
		dsa_set_size_limit(dsa, pgstat_dsa_init_size());

		/* with the limit in place, create the dshash table */
		dsh = dshash_create(dsa, &dsh_params, 0);
		pgStatShmem->hash_handle = dshash_get_hash_table_handle(dsh);

		/* lift limit set above */
		dsa_set_size_limit(dsa, -1);

		/*
		 * Postmaster will never access these again, thus free the local
		 * dsa/dshash references.
		 */
		dshash_detach(dsh);
		dsa_detach(dsa);

		pg_atomic_init_u64(&pgStatShmem->gc_count, 1);


		/*
		 * Initialize global statistics.
		 */

		pgStatShmem->replslot.stats = (PgStat_StatReplSlotEntry *) p;
		p += MAXALIGN(sizeof(PgStat_StatReplSlotEntry) * max_replication_slots);
		LWLockInitialize(&pgStatShmem->replslot.lock, LWTRANCHE_STATS);

		LWLockInitialize(&pgStatShmem->slru.lock, LWTRANCHE_STATS);

		LWLockInitialize(&pgStatShmem->wal.lock, LWTRANCHE_STATS);
	}
	else
	{
		Assert(found);
	}
}


/* ------------------------------------------------------------
 * Functions manging the state of the stats system for all backends.
 * ------------------------------------------------------------
 */

/*
 * pgstat_restore_stats() - read on-disk stats into memory at server start.
 *
 * Should only be called by the startup process or in single user mode.
 */
void
pgstat_restore_stats(void)
{
	pgstat_read_statsfile();
}

/*
 * pgstat_discard_stats() -
 *
 * Remove the stats file.  This is currently used only if WAL recovery is
 * needed after a crash.
 *
 * Should only be called by the startup process or in single user mode.
 */
void
pgstat_discard_stats(void)
{
	int			ret;

	/* NB: this needs to be done even in single user mode */

	ret = unlink(PGSTAT_STAT_PERMANENT_FILENAME);
	if (ret != 0)
	{
		if (errno == ENOENT)
			elog(DEBUG2,
				 "didn't need to unlink permanent stats file \"%s\" - didn't exist",
				 PGSTAT_STAT_PERMANENT_FILENAME);
		else
			ereport(LOG,
					(errcode_for_file_access(),
					 errmsg("could not unlink permanent statistics file \"%s\": %m",
							PGSTAT_STAT_PERMANENT_FILENAME)));
	}
	else
	{
		ereport(DEBUG2,
				(errcode_for_file_access(),
				 errmsg("unlinked permanent statistics file \"%s\": %m",
						PGSTAT_STAT_PERMANENT_FILENAME)));
	}
}

/*
 * pgstat_before_server_shutdown() needs to be called by exactly one process
 * during regular server shutdowns. Otherwise all stats will be lost.
 *
 * We currently only write out stats for proc_exit(0). We might want to change
 * that at some point... But right now pgstat_discard_stats() would be called
 * during the start after a disorderly shutdown, anyway.
 */
void
pgstat_before_server_shutdown(int code, Datum arg)
{
	Assert(pgStatShmem != 0);
	Assert(!pgStatShmem->is_shutdown);

	/*
	 * Stats should only be reported after pgstat_initialize() and before
	 * pgstat_shutdown(). This is a convenient point to catch most violations
	 * of this rule.
	 */
	Assert(pgstat_is_initialized && !pgstat_is_shutdown);

	/* flush out our own pending changes before writing out */
	pgstat_report_stat(true);

	/*
	 * Only write out file during normal shutdown. Don't even signal that
	 * we've shutdown during irregular shutdowns, because the shutdown
	 * sequence isn't coordinated to ensure this backend shuts down last.
	 */
	if (code == 0)
	{
		pgStatShmem->is_shutdown = true;
		pgstat_write_statsfile();
	}
}


/* ------------------------------------------------------------
 * Backend initialization / shutdown functions
 * ------------------------------------------------------------
 */

/*
 * Shut down a single backend's statistics reporting at process exit.
 *
 * Flush any remaining statistics counts out to the stats system.
 * Without this, operations triggered during backend exit (such as
 * temp table deletions) won't be counted.
 */
static void
pgstat_shutdown_hook(int code, Datum arg)
{
	Assert(!pgstat_is_shutdown);
	Assert(IsUnderPostmaster || !IsPostmasterEnvironment);

	/*
	 * If we got as far as discovering our own database ID, we can report what
	 * we did to the stats system.  Otherwise, we'd be reporting an invalid
	 * database ID, so forget it.  (This means that accesses to pg_database
	 * during failed backend starts might never get counted.)
	 */
	if (OidIsValid(MyDatabaseId))
		pgstat_report_disconnect(MyDatabaseId);

	pgstat_report_stat(true);

	Assert(pgStatDSA);

	/* we shouldn't leave references to shared stats */
	pgstat_shared_refs_release_all();

	dshash_detach(pgStatSharedHash);
	pgStatSharedHash = NULL;

	/* there shouldn't be any pending changes left */
	Assert(dlist_is_empty(&pgStatPending));
	dlist_init(&pgStatPending);

	dsa_detach(pgStatDSA);
	pgStatDSA = NULL;

#ifdef USE_ASSERT_CHECKING
	pgstat_is_shutdown = true;
#endif
}

/* ----------
 * pgstat_initialize() -
 *
 *	Initialize pgstats state, and set up our on-proc-exit hook. Called from
 *	BaseInit().
 *
 *	NOTE: MyDatabaseId isn't set yet; so the shutdown hook has to be careful.
 * ----------
 */
void
pgstat_initialize(void)
{
	MemoryContext oldcontext;

	Assert(!pgstat_is_initialized);
	Assert(pgStatDSA == NULL);

	/* stats shared memory persists for the backend lifetime */
	oldcontext = MemoryContextSwitchTo(TopMemoryContext);

	pgStatDSA = dsa_attach_in_place(pgStatShmem->raw_dsa_area, NULL);
	dsa_pin_mapping(pgStatDSA);

	pgStatSharedHash = dshash_attach(pgStatDSA, &dsh_params,
									 pgStatShmem->hash_handle, 0);

	MemoryContextSwitchTo(oldcontext);

	pgstat_wal_initialize();

	/* Set up a process-exit hook to clean up */
	before_shmem_exit(pgstat_shutdown_hook, 0);

#ifdef USE_ASSERT_CHECKING
	pgstat_is_initialized = true;
#endif
}


/* ------------------------------------------------------------
 * Transaction integration
 * ------------------------------------------------------------
 */

/*
 * AFIXME: Should all the stats drop code be moved into pgstat_drop.c?
 */
static void
AtEOXact_PgStat_DroppedStats(PgStat_SubXactStatus *xact_state, bool isCommit)
{
	dlist_mutable_iter iter;
	int			not_freed_count = 0;

	if (xact_state->pending_drops_count == 0)
	{
		Assert(dlist_is_empty(&xact_state->pending_drops));
		return;
	}

	dlist_foreach_modify(iter, &xact_state->pending_drops)
	{
		PgStat_PendingDroppedStatsItem *pending =
		dlist_container(PgStat_PendingDroppedStatsItem, node, iter.cur);
		PgStatHashKey key = {
			.kind = pending->item.kind,
			.dboid = pending->item.dboid,
			.objoid = pending->item.objoid
		};

		if (isCommit && !pending->is_create)
		{
			/*
			 * Transaction that dropped an object committed. Drop the stats
			 * too.
			 */
			if (!pgstat_shared_stat_drop(&key))
				not_freed_count++;
		}
		else if (!isCommit && pending->is_create)
		{
			/*
			 * Transaction that created an object aborted. Drop the stats
			 * associated with the object.
			 */
			if (!pgstat_shared_stat_drop(&key))
				not_freed_count++;
		}

		dlist_delete(&pending->node);
		xact_state->pending_drops_count--;
		pfree(pending);
	}

	if (not_freed_count > 0)
		pgstat_shared_refs_request_gc();
}

/* ----------
 * AtEOXact_PgStat
 *
 *	Called from access/transam/xact.c at top-level transaction commit/abort.
 * ----------
 */
void
AtEOXact_PgStat(bool isCommit, bool parallel)
{
	PgStat_SubXactStatus *xact_state;

	AtEOXact_PgStat_Database(isCommit, parallel);

	/* handle transactional stats information */
	xact_state = pgStatXactStack;
	if (xact_state != NULL)
	{
		Assert(xact_state->nest_level == 1);
		Assert(xact_state->prev == NULL);

		AtEOXact_PgStat_Relations(xact_state, isCommit);
		AtEOXact_PgStat_DroppedStats(xact_state, isCommit);
	}
	pgStatXactStack = NULL;

	/* Make sure any stats snapshot is thrown away */
	pgstat_clear_snapshot();
}

static void
AtEOSubXact_PgStat_DroppedStats(PgStat_SubXactStatus *xact_state,
								bool isCommit, int nestDepth)
{
	PgStat_SubXactStatus *parent_xact_state;
	dlist_mutable_iter iter;
	int			not_freed_count = 0;

	if (xact_state->pending_drops_count == 0)
		return;

	parent_xact_state = pgstat_xact_stack_level_get(nestDepth - 1);

	dlist_foreach_modify(iter, &xact_state->pending_drops)
	{
		PgStat_PendingDroppedStatsItem *pending =
		dlist_container(PgStat_PendingDroppedStatsItem, node, iter.cur);
		PgStatHashKey key = {
			.kind = pending->item.kind,
			.dboid = pending->item.dboid,
			.objoid = pending->item.objoid
		};

		dlist_delete(&pending->node);
		xact_state->pending_drops_count--;

		if (!isCommit && pending->is_create)
		{
			/*
			 * Subtransaction creating a new stats object aborted. Drop the
			 * stats object.
			 */
			if (!pgstat_shared_stat_drop(&key))
				not_freed_count++;
			pfree(pending);
		}
		else if (isCommit)
		{
			/*
			 * Subtransaction dropping a stats object committed. Can't yet
			 * remove the stats object, the surrounding transaction might
			 * still abort. Pass it on to the parent.
			 */
			dlist_push_tail(&parent_xact_state->pending_drops, &pending->node);
			parent_xact_state->pending_drops_count++;
		}
		else
		{
			pfree(pending);
		}
	}

	Assert(xact_state->pending_drops_count == 0);
	if (not_freed_count > 0)
		pgstat_shared_refs_request_gc();
}

/* ----------
 * AtEOSubXact_PgStat
 *
 *	Called from access/transam/xact.c at subtransaction commit/abort.
 * ----------
 */
void
AtEOSubXact_PgStat(bool isCommit, int nestDepth)
{
	PgStat_SubXactStatus *xact_state;

	/* merge the sub-transaction's transactional stats into the parent */
	xact_state = pgStatXactStack;
	if (xact_state != NULL &&
		xact_state->nest_level >= nestDepth)
	{
		/* delink xact_state from stack immediately to simplify reuse case */
		pgStatXactStack = xact_state->prev;

		AtEOSubXact_PgStat_Relations(xact_state, isCommit, nestDepth);
		AtEOSubXact_PgStat_DroppedStats(xact_state, isCommit, nestDepth);

		pfree(xact_state);
	}
}

/*
 * AtPrepare_PgStat
 *		Save the transactional stats state at 2PC transaction prepare.
 */
void
AtPrepare_PgStat(void)
{
	PgStat_SubXactStatus *xact_state;

	xact_state = pgStatXactStack;
	if (xact_state != NULL)
	{
		Assert(xact_state->nest_level == 1);
		Assert(xact_state->prev == NULL);

		AtPrepare_PgStat_Relations(xact_state);
	}
}

/*
 * PostPrepare_PgStat
 *		Clean up after successful PREPARE.
 *
 * Note: AtEOXact_PgStat is not called during PREPARE.
 */
void
PostPrepare_PgStat(void)
{
	PgStat_SubXactStatus *xact_state;

	/*
	 * We don't bother to free any of the transactional state, since it's all
	 * in TopTransactionContext and will go away anyway.
	 */
	xact_state = pgStatXactStack;
	if (xact_state != NULL)
	{
		Assert(xact_state->nest_level == 1);
		Assert(xact_state->prev == NULL);

		PostPrepare_PgStat_Relations(xact_state);
	}
	pgStatXactStack = NULL;

	/* Make sure any stats snapshot is thrown away */
	pgstat_clear_snapshot();
}

/* ----------
 * pgstat_clear_snapshot() -
 *
 *	Discard any data collected in the current transaction.  Any subsequent
 *	request will cause new snapshots to be read.
 *
 *	This is also invoked during transaction commit or abort to discard
 *	the no-longer-wanted snapshot.
 * ----------
 */
void
pgstat_clear_snapshot(void)
{
	pgstat_assert_is_up();

	memset(&stats_snapshot.global_valid, 0, sizeof(stats_snapshot.global_valid));
	stats_snapshot.stats = NULL;
	stats_snapshot.mode = STATS_FETCH_CONSISTENCY_NONE;

	/* Release memory, if any was allocated */
	if (pgStatSnapshotContext)
	{
		MemoryContextDelete(pgStatSnapshotContext);

		/* Reset variables */
		pgStatSnapshotContext = NULL;
	}

	/*
	 * Historically the backend_status.c facilities lived in this file, and
	 * were reset with the same function. For now keep it that way, and
	 * forward the reset request.
	 */
	pgstat_clear_backend_activity_snapshot();
}

int
pgstat_pending_stats_drops(bool isCommit, xl_xact_stats_item **items)
{
	PgStat_SubXactStatus *xact_state = pgStatXactStack;
	int			nitems = 0;
	dlist_iter	iter;

	if (xact_state == NULL)
		return 0;

	/*
	 * We expect to be called for subtransaction abort (which logs a WAL
	 * record), but not for subtransaction commit (which doesn't).
	 */
	Assert(!isCommit || xact_state->nest_level == 1);
	Assert(!isCommit || xact_state->prev == NULL);

	*items = palloc(xact_state->pending_drops_count
					* sizeof(PgStat_PendingDroppedStatsItem));

	dlist_foreach(iter, &xact_state->pending_drops)
	{
		PgStat_PendingDroppedStatsItem *pending =
		dlist_container(PgStat_PendingDroppedStatsItem, node, iter.cur);

		if (isCommit && pending->is_create)
			continue;
		if (!isCommit && !pending->is_create)
			continue;

		Assert(nitems < xact_state->pending_drops_count);
		(*items)[nitems++] = pending->item;
	}

	return nitems;
}

/*
 * Execute scheduled drops post-commit. Called from xact_redo_commit() during
 * recovery, and from FinishPreparedTransaction() during normal commit
 * processing.
 */
void
pgstat_perform_drops(int ndrops, struct xl_xact_stats_item *items, bool is_redo)
{
	int			not_freed_count = 0;

	if (ndrops == 0)
		return;

	for (int i = 0; i < ndrops; i++)
	{
		PgStatHashKey key = {
			.kind = items[i].kind,
			.dboid = items[i].dboid,
			.objoid = items[i].objoid
		};

		if (!pgstat_shared_stat_drop(&key))
			not_freed_count++;
	}

	if (not_freed_count > 0)
		pgstat_shared_refs_request_gc();
}

/*
 * Ensure (sub)transaction stack entry for the given nest_level exists, adding
 * it if needed.
 */
PgStat_SubXactStatus *
pgstat_xact_stack_level_get(int nest_level)
{
	PgStat_SubXactStatus *xact_state;

	xact_state = pgStatXactStack;
	if (xact_state == NULL || xact_state->nest_level != nest_level)
	{
		xact_state = (PgStat_SubXactStatus *)
			MemoryContextAlloc(TopTransactionContext,
							   sizeof(PgStat_SubXactStatus));
		dlist_init(&xact_state->pending_drops);
		xact_state->pending_drops_count = 0;
		xact_state->nest_level = nest_level;
		xact_state->prev = pgStatXactStack;
		xact_state->first = NULL;
		pgStatXactStack = xact_state;
	}
	return xact_state;
}

static void
pgstat_schedule_stat_internal(PgStatKind kind, Oid dboid, Oid objoid, bool is_create)
{
	int			nest_level = GetCurrentTransactionNestLevel();
	PgStat_SubXactStatus *xact_state;
	PgStat_PendingDroppedStatsItem *drop = (PgStat_PendingDroppedStatsItem *)
	MemoryContextAlloc(TopTransactionContext, sizeof(PgStat_PendingDroppedStatsItem));

	xact_state = pgstat_xact_stack_level_get(nest_level);

	drop->is_create = is_create;
	drop->item.kind = kind;
	drop->item.dboid = dboid;
	drop->item.objoid = objoid;

	dlist_push_tail(&xact_state->pending_drops, &drop->node);
	xact_state->pending_drops_count++;
}

void
pgstat_schedule_stat_create(PgStatKind kind, Oid dboid, Oid objoid)
{
	if (pgstat_shared_ref_get(kind, dboid, objoid, false, NULL))
	{
		Oid msg_oid = (kind == PGSTAT_KIND_DB) ? dboid : objoid;

		ereport(WARNING,
				errmsg("Resetting existing stats for %s with OID %d.",
					   (pgstat_kind_info_for(kind))->name, msg_oid));

		pgstat_reset_one(kind, dboid, objoid);
	}

	pgstat_schedule_stat_internal(kind, dboid, objoid, /* create */ true);
}

/*
 * AFIXME: comment
 */
void
pgstat_schedule_stat_drop(PgStatKind kind, Oid dboid, Oid objoid)
{
	pgstat_schedule_stat_internal(kind, dboid, objoid, /* create */ false);
}


/* ------------------------------------------------------------
 * Public functions used by backends follow
 * ------------------------------------------------------------
 */

/*
 * Must be called by processes that performs DML: tcop/postgres.c, logical
 * receiver processes, SPI worker, etc. to apply the so far collected
 * per-table and function usage statistics to the shared statistics hashes.
 *
 * Updates are applied not more frequent than the interval of
 * PGSTAT_MIN_INTERVAL milliseconds. They are also postponed on lock
 * failure if force is false and there's no pending updates longer than
 * PGSTAT_MAX_INTERVAL milliseconds. Postponed updates are retried in
 * succeeding calls of this function.
 *
 * Returns the time until the next timing when updates are applied in
 * milliseconds if there are no updates held for more than
 * PGSTAT_MIN_INTERVAL milliseconds.
 *
 * Note that this is called only when not within a transaction, so it is fair
 * to use transaction stop time as an approximation of current time.
 */
long
pgstat_report_stat(bool force)
{
	static TimestampTz next_flush = 0;
	static TimestampTz pending_since = 0;
	static long retry_interval = 0;
	bool		partial_flush;
	TimestampTz now;
	bool		nowait;

	pgstat_assert_is_up();

	/* "absorb" the forced flush even if there's nothing to flush */
	if (pgStatForceNextFlush)
	{
		force = true;
		pgStatForceNextFlush = false;
	}

	/* Don't expend a clock check if nothing to do */
	if (dlist_is_empty(&pgStatPending) &&
		!have_slrustats &&
		!pgstat_wal_pending())
	{
		return 0;
	}

	/*
	 * There should never be stats to report once stats are shut down. Can't
	 * assert that before the checks above, as there is an unconditional
	 * pgstat_report_stat() call in pgstat_shutdown_hook() - which at least
	 * the process that ran pgstat_before_server_shutdown() will still call.
	 */
	Assert(!pgStatShmem->is_shutdown);

	now = GetCurrentTransactionStopTimestamp();

	if (!force)
	{
		/*
		 * Don't flush stats too frequently.  Return the time to the next
		 * flush.
		 */
		if (now < next_flush)
		{
			/* Record the epoch time if retrying. */
			if (pending_since == 0)
				pending_since = now;

			return (next_flush - now) / 1000;
		}

		/* But, don't keep pending updates longer than PGSTAT_MAX_INTERVAL. */

		if (pending_since > 0 &&
			TimestampDifferenceExceeds(pending_since, now, PGSTAT_MAX_INTERVAL))
			force = true;
	}

	pgstat_update_dbstats(now);

	/* don't wait for lock acquisition when !force */
	nowait = !force;

	partial_flush = false;

	/* flush database / relation / function / ... stats */
	partial_flush |= pgstat_pending_flush_stats(nowait);

	/* flush wal stats */
	partial_flush |= pgstat_wal_flush(nowait);

	/* flush SLRU stats */
	partial_flush |= pgstat_slru_flush(nowait);

	/*
	 * Some of the pending stats may have not been flushed due to lock
	 * contention.  If we have such pending stats here, let the caller know
	 * the retry interval.
	 */
	if (partial_flush)
	{
		/* Retain the epoch time */
		if (pending_since == 0)
			pending_since = now;

		/* The interval is doubled at every retry. */
		if (retry_interval == 0)
			retry_interval = PGSTAT_RETRY_MIN_INTERVAL * 1000;
		else
			retry_interval = retry_interval * 2;

		/*
		 * Determine the next retry interval so as not to get shorter than the
		 * previous interval.
		 */
		if (!TimestampDifferenceExceeds(pending_since,
										now + 2 * retry_interval,
										PGSTAT_MAX_INTERVAL))
			next_flush = now + retry_interval;
		else
		{
			next_flush = pending_since + PGSTAT_MAX_INTERVAL * 1000;
			retry_interval = next_flush - now;
		}

		return retry_interval / 1000;
	}

	/* Set the next time to update stats */
	next_flush = now + PGSTAT_MIN_INTERVAL * 1000;
	retry_interval = 0;
	pending_since = 0;

	return 0;
}

/*
 * Force locally pending stats to be flushed during the next
 * pgstat_report_stat() call. This is useful for writing tests.
 */
void
pgstat_force_next_flush(void)
{
	pgStatForceNextFlush = true;
}

/*
 * Only for use by pgstat_reset_counters()
 */
static bool
reset_this_db(PgStatShmHashEntry *entry)
{
	return entry->key.dboid == MyDatabaseId;
}

/* ----------
 * pgstat_reset_counters() -
 *
 *	Tell the statistics system to reset counters for our database.
 *
 *	Permission checking for this function is managed through the normal
 *	GRANT system.
 * ----------
 */
void
pgstat_reset_counters(void)
{
	pgstat_reset_matching(reset_this_db);
}

/* ----------
 * pgstat_reset_single_counter() -
 *
 *	Tell the statistics system to reset a single counter.
 *
 *	Permission checking for this function is managed through the normal
 *	GRANT system.
 * ----------
 */
void
pgstat_reset_single_counter(PgStatKind kind, Oid objoid)
{
	TimestampTz ts = GetCurrentTimestamp();

	Assert(!pgstat_kind_info_for(kind)->fixed_amount);

	/* XXX: isn't this pretty grotty behaviour? */
	/* Set the reset timestamp for the whole database */
	pgstat_database_reset_timestamp(MyDatabaseId, ts);

	/* reset the "single counter" */
	pgstat_reset_one(kind, MyDatabaseId, objoid);
}

/* ----------
 * pgstat_reset_shared_counters() -
 *
 *	Reset cluster-wide shared counters.
 *
 *	Permission checking for this function is managed through the normal
 *	GRANT system.
 *
 *  We don't scribble on shared stats while resetting to avoid locking on
 *  shared stats struct. Instead, just record the current counters in another
 *  shared struct, which is protected by StatsLock. See
 *  pgstat_fetch_stat_(archiver|bgwriter|checkpointer) for the reader side.
 * ----------
 */
void
pgstat_reset_shared_counters(PgStatKind kind)
{
	const PgStatKindInfo *kind_info = pgstat_kind_info_for(kind);
	TimestampTz now = GetCurrentTimestamp();

	Assert(kind_info->fixed_amount);

	kind_info->reset_all_cb(now);
}

/*
 * If a stats snapshot has been taken, return the timestamp at which that was
 * done, and set *have_snapshot to true. Otherwise *have_snapshot is set to
 * false.
 */
TimestampTz
pgstat_get_stat_snapshot_timestamp(bool *have_snapshot)
{
	if (stats_snapshot.mode == STATS_FETCH_CONSISTENCY_SNAPSHOT)
	{
		*have_snapshot = true;
		return stats_snapshot.snapshot_timestamp;
	}

	*have_snapshot = false;

	return 0;
}

bool
pgstat_shared_stat_exists(PgStatKind kind, Oid dboid, Oid objoid)
{
	if (pgstat_kind_info_for(kind)->fixed_amount)
		return true;

	return pgstat_fetch_entry(kind, dboid, objoid) != NULL;
}


/* ------------------------------------------------------------
 * Helper functions
 * ------------------------------------------------------------
 */

/* ----------
 * pgstat_setup_memcxt() -
 *
 *	Create pgStatSnapshotContext if not already done.
 * ----------
 */
static void
pgstat_setup_memcxt(void)
{
	if (unlikely(!pgStatSharedRefContext))
		pgStatSharedRefContext =
			AllocSetContextCreate(CacheMemoryContext,
								  "PgStat Shared Ref",
								  ALLOCSET_SMALL_SIZES);
	if (unlikely(!pgStatSharedRefHashContext))
		pgStatSharedRefHashContext =
			AllocSetContextCreate(CacheMemoryContext,
								  "PgStat Shared Ref Hash",
								  ALLOCSET_SMALL_SIZES);
	if (unlikely(!pgStatPendingContext))
		pgStatPendingContext =
			AllocSetContextCreate(CacheMemoryContext,
								  "PgStat Pending",
								  ALLOCSET_SMALL_SIZES);
}

PgStatKind
pgstat_kind_from_str(char *kind_str)
{
	for (int kind = 0; kind <= PGSTAT_KIND_LAST; kind++)
	{
		if (pg_strcasecmp(kind_str, pgstat_kind_infos[kind].name) == 0)
			return kind;
	}

	ereport(ERROR,
			(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
			 errmsg("invalid statistic kind: \"%s\"", kind_str)));
	return PGSTAT_KIND_DB;		/* avoid compiler warnings */
}

static inline const PgStatKindInfo *
pgstat_kind_info_for(PgStatKind kind)
{
	AssertArg(kind <= PGSTAT_KIND_LAST);

	return &pgstat_kind_infos[kind];
}

/*
 * The length of the data portion of a shared memory stats entry (i.e. without
 * transient data such as refcoutns, lwlocks, ...).
 */
static inline size_t
shared_stat_entry_len(PgStatKind kind)
{
	return pgstat_kind_info_for(kind)->shared_data_len;
}

/*
 * Returns a pointer to the data portion of a shared memory stats entry.
 */
static inline void *
shared_stat_entry_data(PgStatKind kind, PgStatShm_StatEntryHeader *entry)
{
	size_t		off = pgstat_kind_info_for(kind)->shared_data_off;

	Assert(off != 0 && off < PG_UINT32_MAX);

	return ((char *) (entry)) + off;
}

/*
 * Stats should only be reported after pgstat_initialize() and before
 * pgstat_shutdown(). This check is put in a few central places to catch
 * violations of this rule more easily.
 */
#ifdef USE_ASSERT_CHECKING
void
pgstat_assert_is_up(void)
{
	Assert(pgstat_is_initialized && !pgstat_is_shutdown);
}
#endif


/* ------------------------------------------------------------
 * reading and writing of on-disk stats file
 * ------------------------------------------------------------
 */

/* ----------
 * pgstat_write_statsfile() -
 *
 * This function is called in the last process that is accessing the shared
 * stats so locking is not required.
 * ----------
 */
static void
pgstat_write_statsfile(void)
{
	FILE	   *fpout;
	int32		format_id;
	const char *tmpfile = PGSTAT_STAT_PERMANENT_TMPFILE;
	const char *statfile = PGSTAT_STAT_PERMANENT_FILENAME;
	int			rc;
	dshash_seq_status hstat;
	PgStatShmHashEntry *ps;

	Assert(pgStatDSA);

	elog(DEBUG2, "writing stats file \"%s\"", statfile);

	/*
	 * Open the statistics temp file to write out the current values.
	 */
	fpout = AllocateFile(tmpfile, PG_BINARY_W);
	if (fpout == NULL)
	{
		ereport(LOG,
				(errcode_for_file_access(),
				 errmsg("could not open temporary statistics file \"%s\": %m",
						tmpfile)));
		return;
	}

	/*
	 * Write the file header --- currently just a format ID.
	 */
	format_id = PGSTAT_FILE_FORMAT_ID;
	rc = fwrite(&format_id, sizeof(format_id), 1, fpout);
	(void) rc;					/* we'll check for error with ferror */

	/*
	 * XXX: The following could now be generalized to just iterate over
	 * pgstat_kind_infos instead of knowing about the different kinds of
	 * stats.
	 */

	/*
	 * Write archiver stats struct
	 */
	rc = fwrite(&pgStatShmem->archiver.stats, sizeof(PgStat_ArchiverStats), 1,
				fpout);
	(void) rc;					/* we'll check for error with ferror */

	/*
	 * Write bgwriter stats struct
	 */
	rc = fwrite(&pgStatShmem->bgwriter.stats, sizeof(PgStat_BgWriterStats), 1, fpout);
	(void) rc;					/* we'll check for error with ferror */

	/*
	 * Write checkpointer stats struct
	 */
	rc = fwrite(&pgStatShmem->checkpointer.stats, sizeof(PgStat_CheckpointerStats), 1, fpout);
	(void) rc;					/* we'll check for error with ferror */

	/*
	 * Write SLRU stats struct
	 */
	rc = fwrite(&pgStatShmem->slru.stats,
				sizeof(PgStat_SLRUStats[SLRU_NUM_ELEMENTS]),
				1, fpout);
	(void) rc;					/* we'll check for error with ferror */

	/*
	 * Write WAL stats struct
	 */
	rc = fwrite(&pgStatShmem->wal.stats, sizeof(PgStat_WalStats), 1, fpout);
	(void) rc;					/* we'll check for error with ferror */

	/*
	 * Walk through the stats entries
	 */
	dshash_seq_init(&hstat, pgStatSharedHash, false);
	while ((ps = dshash_seq_next(&hstat)) != NULL)
	{
		PgStatShm_StatEntryHeader *shstats;
		size_t		len;

		CHECK_FOR_INTERRUPTS();

		/* we may have some "dropped" entries not yet removed, skip them */
		Assert(!ps->dropped);
		if (ps->dropped)
			continue;

		shstats = (PgStatShm_StatEntryHeader *) dsa_get_address(pgStatDSA, ps->body);

		/* if not dropped the valid-entry refcount should exist */
		Assert(pg_atomic_read_u32(&ps->refcount) > 0);

		fputc('S', fpout);
		rc = fwrite(&ps->key, sizeof(PgStatHashKey), 1, fpout);

		/* Write except the header part of the etnry */
		len = shared_stat_entry_len(ps->key.kind);
		rc = fwrite(shared_stat_entry_data(ps->key.kind, shstats), len, 1, fpout);
		(void) rc;				/* we'll check for error with ferror */
	}
	dshash_seq_term(&hstat);

	/*
	 * Write replication slot stats struct
	 */
	for (int i = 0; i < max_replication_slots; i++)
	{
		PgStat_StatReplSlotEntry *statent = &pgStatShmem->replslot.stats[i];

		if (NameStr(pgStatShmem->replslot.stats[i].slotname)[0] == 0)
			continue;

		fputc('R', fpout);
		rc = fwrite(statent, sizeof(*statent), 1, fpout);
		(void) rc;				/* we'll check for error with ferror */
	}

	/*
	 * No more output to be done. Close the temp file and replace the old
	 * pgstat.stat with it.  The ferror() check replaces testing for error
	 * after each individual fputc or fwrite above.
	 */
	fputc('E', fpout);

	if (ferror(fpout))
	{
		ereport(LOG,
				(errcode_for_file_access(),
				 errmsg("could not write temporary statistics file \"%s\": %m",
						tmpfile)));
		FreeFile(fpout);
		unlink(tmpfile);
	}
	else if (FreeFile(fpout) < 0)
	{
		ereport(LOG,
				(errcode_for_file_access(),
				 errmsg("could not close temporary statistics file \"%s\": %m",
						tmpfile)));
		unlink(tmpfile);
	}
	else if (rename(tmpfile, statfile) < 0)
	{
		ereport(LOG,
				(errcode_for_file_access(),
				 errmsg("could not rename temporary statistics file \"%s\" to \"%s\": %m",
						tmpfile, statfile)));
		unlink(tmpfile);
	}
}

/* ----------
 * pgstat_read_statsfile() -
 *
 *	Reads in existing activity statistics file into the shared stats hash.
 *
 * This function is called in the only process that is accessing the shared
 * stats so locking is not required.
 * ----------
 */
static void
pgstat_read_statsfile(void)
{
	FILE	   *fpin;
	int32		format_id;
	bool		found;
	const char *statfile = PGSTAT_STAT_PERMANENT_FILENAME;
	TimestampTz ts = GetCurrentTimestamp();

	/* shouldn't be called from postmaster */
	Assert(IsUnderPostmaster || !IsPostmasterEnvironment);

	elog(DEBUG2, "reading stats file \"%s\"", statfile);

	/*
	 * Try to open the stats file. If it doesn't exist, the backends simply
	 * returns zero for anything and the activity statistics simply starts
	 * from scratch with empty counters.
	 *
	 * ENOENT is a possibility if stats collection was previously disabled or
	 * has not yet written the stats file for the first time.  Any other
	 * failure condition is suspicious.
	 */
	if ((fpin = AllocateFile(statfile, PG_BINARY_R)) == NULL)
	{
		if (errno != ENOENT)
			ereport(LOG,
					(errcode_for_file_access(),
					 errmsg("could not open statistics file \"%s\": %m",
							statfile)));
		pgstat_reset_all_stats(ts);
		return;
	}

	/*
	 * Verify it's of the expected format.
	 */
	if (fread(&format_id, 1, sizeof(format_id), fpin) != sizeof(format_id) ||
		format_id != PGSTAT_FILE_FORMAT_ID)
		goto error;

	/*
	 * XXX: The following could now be generalized to just iterate over
	 * pgstat_kind_infos instead of knowing about the different kinds of
	 * stats.
	 */

	/*
	 * Read archiver stats struct
	 */
	if (fread(&pgStatShmem->archiver.stats, 1, sizeof(PgStat_ArchiverStats),
			  fpin) != sizeof(PgStat_ArchiverStats))
	{
		MemSet(&pgStatShmem->archiver.stats, 0, sizeof(PgStat_ArchiverStats));
		goto error;
	}

	/*
	 * Read bgwriter stats struct
	 */
	if (fread(&pgStatShmem->bgwriter.stats, 1, sizeof(PgStat_BgWriterStats), fpin) !=
		sizeof(PgStat_BgWriterStats))
	{
		MemSet(&pgStatShmem->bgwriter.stats, 0, sizeof(PgStat_BgWriterStats));
		goto error;
	}

	/*
	 * Read checkpointer stats struct
	 */
	if (fread(&pgStatShmem->checkpointer.stats, 1, sizeof(PgStat_CheckpointerStats), fpin) !=
		sizeof(PgStat_CheckpointerStats))
	{
		MemSet(&pgStatShmem->checkpointer.stats, 0, sizeof(PgStat_CheckpointerStats));
		goto error;
	}

	/*
	 * Read SLRU stats struct
	 */
	if (fread(&pgStatShmem->slru.stats, 1, SizeOfSlruStats, fpin) != SizeOfSlruStats)
	{
		goto error;
	}

	/*
	 * Read WAL stats struct
	 */
	if (fread(&pgStatShmem->wal.stats, 1, sizeof(PgStat_WalStats), fpin)
		!= sizeof(PgStat_WalStats))
	{
		MemSet(&pgStatShmem->wal.stats, 0, sizeof(PgStat_WalStats));
		goto error;
	}

	/*
	 * We found an existing activity statistics file. Read it and put all the
	 * hash table entries into place.
	 */
	for (;;)
	{
		switch (fgetc(fpin))
		{
			case 'S':
				{
					PgStatHashKey key;
					PgStatShmHashEntry *p;
					PgStatShm_StatEntryHeader *header;
					size_t		len;

					CHECK_FOR_INTERRUPTS();

					if (fread(&key, 1, sizeof(key), fpin) != sizeof(key))
						goto error;

					/*
					 * This intentionally doesn't use pgstat_shared_ref_get()
					 * - putting all stats into checkpointer's
					 * pgStatSharedRefHash would be wasted effort and memory.
					 */
					p = dshash_find_or_insert(pgStatSharedHash, &key, &found);

					/* don't allow duplicate entries */
					if (found)
					{
						elog(WARNING, "found duplicate stats entry");
						dshash_release_lock(pgStatSharedHash, p);
						goto error;
					}

					header = pgstat_shared_stat_init(key.kind, p, 1);
					dshash_release_lock(pgStatSharedHash, p);

					/* Avoid overwriting header part */
					len = shared_stat_entry_len(key.kind);

					if (fread(shared_stat_entry_data(key.kind, header), 1, len, fpin) != len)
						goto error;

					break;
				}

				/*
				 * 'R'	A PgStat_StatReplSlotEntry struct describing a
				 * replication slot follows.
				 */
			case 'R':
				{
					PgStat_StatReplSlotEntry tmp;

					if (fread(&tmp, 1, sizeof(tmp), fpin) != sizeof(tmp))
						goto error;

					pgstat_replslot_restore(&tmp);
				}
				break;

			case 'E':
				goto done;

			default:
				goto error;
		}
	}

done:
	FreeFile(fpin);

	elog(DEBUG2, "removing permanent stats file \"%s\"", statfile);
	unlink(statfile);

	return;

error:
	ereport(LOG,
			(errmsg("corrupted statistics file \"%s\"", statfile)));

	/* Set the current timestamp as reset timestamp */
	pgstat_reset_all_stats(ts);

	goto done;
}

/*
 * Helper to initially reset all stats
 */
static void
pgstat_reset_all_stats(TimestampTz ts)
{
	for (int kind = 0; kind <= PGSTAT_KIND_LAST; kind++)
	{
		const PgStatKindInfo *kind_info = pgstat_kind_info_for(kind);

		if (!kind_info->fixed_amount)
			continue;

		kind_info->reset_all_cb(ts);
	}
}


/* ------------------------------------------------------------
 * Maintenance of shared memory stats entries
 * ------------------------------------------------------------
 */

static PgStatShm_StatEntryHeader *
pgstat_shared_stat_init(PgStatKind kind,
						PgStatShmHashEntry *shhashent,
						int init_refcount)
{
	/* Create new stats entry. */
	dsa_pointer chunk;
	PgStatShm_StatEntryHeader *shheader;

	LWLockInitialize(&shhashent->lock, LWTRANCHE_STATS);
	pg_atomic_init_u32(&shhashent->refcount, init_refcount);
	shhashent->dropped = false;

	chunk = dsa_allocate0(pgStatDSA, pgstat_kind_info_for(kind)->shared_size);
	shheader = dsa_get_address(pgStatDSA, chunk);
	shheader->magic = 0xdeadbeef;

	/* Link the new entry from the hash entry. */
	shhashent->body = chunk;

	return shheader;
}

static void
pgstat_setup_shared_refs(void)
{
	if (likely(pgStatSharedRefHash != NULL))
		return;

	pgStatSharedRefHash =
		pgstat_shared_ref_hash_create(pgStatSharedRefHashContext,
									  PGSTAT_SHARED_REF_HASH_SIZE, NULL);
	pgStatSharedRefAge = pg_atomic_read_u64(&pgStatShmem->gc_count);
	Assert(pgStatSharedRefAge != 0);
}

/*
 * Helper function for pgstat_shared_ref_get().
 */
static bool
pgstat_shared_ref_get_cached(PgStatHashKey key, PgStatSharedRef **shared_ref_p)
{
	bool		found;
	PgStatSharedRefHashEntry *cache_entry;

	pgstat_assert_is_up();

	pgstat_setup_memcxt();
	pgstat_setup_shared_refs();

	/*
	 * pgStatSharedRefAge increments quite slowly than the time the following
	 * loop takes so this is expected to iterate no more than twice.
	 *
	 * AFIXME: Why is this a good place to do this?
	 */
	while (pgstat_shared_refs_need_gc())
		pgstat_shared_refs_gc();

	/*
	 * We immediately insert a cache entry, because it avoids 1) multiple
	 * hashtable lookups in case of a cache miss 2) having to deal with
	 * out-of-memory errors after incrementing
	 * PgStatShm_StatEntryHeader->refcount.
	 */

	cache_entry = pgstat_shared_ref_hash_insert(pgStatSharedRefHash, key, &found);

	if (!found || !cache_entry->shared_ref)
	{
		PgStatSharedRef *shared_ref;

		cache_entry->shared_ref = shared_ref =
			MemoryContextAlloc(pgStatSharedRefContext,
							   sizeof(PgStatSharedRef));
		shared_ref->shared_stats = NULL;
		shared_ref->shared_entry = NULL;
		shared_ref->pending = NULL;

		found = false;
	}
	else if (cache_entry->shared_ref->shared_stats == NULL)
	{
		Assert(cache_entry->shared_ref->shared_entry == NULL);
		found = false;
	}
	else
	{
		PgStatSharedRef *shared_ref PG_USED_FOR_ASSERTS_ONLY;

		shared_ref = cache_entry->shared_ref;
		Assert(shared_ref->shared_entry != NULL);
		Assert(shared_ref->shared_stats != NULL);

		Assert(shared_ref->shared_stats->magic == 0xdeadbeef);
		/* should have at least our reference */
		Assert(pg_atomic_read_u32(&shared_ref->shared_entry->refcount) > 0);
	}

	*shared_ref_p = cache_entry->shared_ref;
	return found;
}

/*
 * Get a shared stats reference. If create is true, the shared stats object is
 * created if it does not exist.
 *
 * When create is true, and created_shared is non-NULL, it'll be set to true
 * if the entry is newly created, false otherwise.
 */
PgStatSharedRef *
pgstat_shared_ref_get(PgStatKind kind, Oid dboid, Oid objoid, bool create,
					  bool *created_shared)
{
	PgStatHashKey key;
	PgStatShmHashEntry *shhashent;
	PgStatShm_StatEntryHeader *shheader = NULL;
	PgStatSharedRef *shared_ref;
	bool		shfound;

	/*
	 * passing created_shared only makes sense if we possibly could create
	 * entry
	 */
	AssertArg(create || created_shared == NULL);
	pgstat_assert_is_up();

	if (created_shared != NULL)
		*created_shared = false;

	key.kind = kind;
	key.dboid = dboid;
	key.objoid = objoid;

	Assert(pgStatSharedHash != NULL);
	Assert(!pgStatShmem->is_shutdown);

	/*
	 * First check the lookup cache hashtable in local memory. If we find a
	 * match here we can avoid taking locks / contention.
	 */
	if (pgstat_shared_ref_get_cached(key, &shared_ref))
		return shared_ref;

	Assert(shared_ref != NULL);

	/*
	 * Do a lookup in the hash table first - it's quite likely that the entry
	 * already exists, and that way we only need a shared lock.
	 */
	shhashent = dshash_find(pgStatSharedHash, &key, false);

	if (shhashent)
		shfound = true;
	else if (create)
	{
		/*
		 * It's possible that somebody created the entry since the above
		 * lookup, fall through to the same path as before if so.
		 */
		shhashent = dshash_find_or_insert(pgStatSharedHash, &key, &shfound);
		if (!shfound)
		{
			/*
			 * Initialize refcount to 2, (1 marking it as valid, one for the
			 * local reference). That prevents another backend from freeing
			 * the entry once we release the lock below. The entry can't be
			 * freed before the initialization because it can't be found as
			 * long as we hold the dshash partition lock.
			 */
			shheader = pgstat_shared_stat_init(kind, shhashent, 2);

			dshash_release_lock(pgStatSharedHash, shhashent);

			shared_ref->shared_stats = shheader;
			shared_ref->shared_entry = shhashent;

			if (created_shared != NULL)
				*created_shared = true;

			return shared_ref;
		}
	}
	else
	{
		shfound = false;

		/*
		 * If we're not creating, delete the reference again. In all
		 * likelihood it's just a stats lookup - no point wasting memory for a
		 * shared ref to nothing...
		 */
		pgstat_shared_ref_release(key, shared_ref, false);

		return NULL;
	}

	/*
	 * Can get here either if dshash_find() returned true, or if
	 * dshash_find_or_insert() found a concurrently inserted entry.
	 */
	Assert(shfound);

	if (shhashent->dropped)
	{
		if (create)
			elog(ERROR, "encountered dropped entry when asked to create new one");

		dshash_release_lock(pgStatSharedHash, shhashent);
		pgstat_shared_ref_release(key, shared_ref, false);

		return NULL;
	}
	else
	{
		shheader = dsa_get_address(pgStatDSA, shhashent->body);

		Assert(shheader->magic == 0xdeadbeef);
		Assert(pg_atomic_read_u32(&shhashent->refcount) > 0);

		pg_atomic_fetch_add_u32(&shhashent->refcount, 1);

		dshash_release_lock(pgStatSharedHash, shhashent);

		shared_ref->shared_stats = shheader;
		shared_ref->shared_entry = shhashent;
	}

	return shared_ref;
}

static void
pgstat_shared_ref_release(PgStatHashKey key, PgStatSharedRef *shared_ref, bool discard_pending)
{
	if (shared_ref && shared_ref->pending)
	{
		if (discard_pending)
			pgstat_pending_delete(shared_ref);
		else
			elog(ERROR, "releasing ref with pending data");
	}

	if (shared_ref && shared_ref->shared_stats)
	{
		Assert(shared_ref->shared_stats->magic == 0xdeadbeef);
		Assert(shared_ref->pending == NULL);

		/*
		 * This can't race with another backend looking up the stats entry and
		 * increasing the refcount because it is not "legal" to create
		 * additional references to dropped entries.
		 */
		if (pg_atomic_fetch_sub_u32(&shared_ref->shared_entry->refcount, 1) == 1)
		{
			PgStatShmHashEntry *shent;
			dsa_pointer dsap;

			/*
			 * We're the last referrer to this entry, try to drop the shared
			 * entry.
			 */

			/* only dropped entries can reach a 0 refcount */
			Assert(shared_ref->shared_entry->dropped);

			shent = dshash_find(pgStatSharedHash,
								&shared_ref->shared_entry->key,
								true);
			if (!shent)
				elog(PANIC, "could not find just referenced shared stats entry");

			/* ATODO: convert to assertion before commit */
			if (pg_atomic_read_u32(&shared_ref->shared_entry->refcount) != 0)
				elog(PANIC, "concurrent access to stats entry during deletion");

			Assert(shared_ref->shared_entry == shent);

			/*
			 * Fetch dsa pointer before deleting entry - that way we can free
			 * the memory after releasing the lock.
			 */
			dsap = shent->body;

			dshash_delete_entry(pgStatSharedHash, shent);

			dsa_free(pgStatDSA, dsap);
			shared_ref->shared_stats = NULL;
		}
	}

	if (!pgstat_shared_ref_hash_delete(pgStatSharedRefHash, key))
		elog(PANIC, "something has gone wrong");

	if (shared_ref)
		pfree(shared_ref);
}

bool
pgstat_shared_stat_lock(PgStatSharedRef *shared_ref, bool nowait)
{
	LWLock	   *lock = &shared_ref->shared_entry->lock;

	if (nowait)
		return LWLockConditionalAcquire(lock, LW_EXCLUSIVE);

	LWLockAcquire(lock, LW_EXCLUSIVE);
	return true;
}

void
pgstat_shared_stat_unlock(PgStatSharedRef *shared_ref)
{
	LWLockRelease(&shared_ref->shared_entry->lock);
}

/*
 * Helper function to fetch and lock shared stats.
 */
PgStatSharedRef *
pgstat_shared_stat_locked(PgStatKind kind, Oid dboid, Oid objoid, bool nowait)
{
	PgStatSharedRef *shared_ref;

	/* find shared table stats entry corresponding to the local entry */
	shared_ref = pgstat_shared_ref_get(kind, dboid, objoid, true, NULL);

	/* lock the shared entry to protect the content, skip if failed */
	if (!pgstat_shared_stat_lock(shared_ref, nowait))
		return NULL;

	return shared_ref;
}

static void
pgstat_shared_refs_request_gc(void)
{
	pg_atomic_fetch_add_u64(&pgStatShmem->gc_count, 1);
}

static bool
pgstat_shared_refs_need_gc(void)
{
	uint64		curage;

	if (!pgStatSharedRefHash)
		return false;

	/* should have been initialized when creating pgStatSharedRefHash */
	Assert(pgStatSharedRefAge != 0);

	curage = pg_atomic_read_u64(&pgStatShmem->gc_count);

	return pgStatSharedRefAge != curage;
}

static void
pgstat_shared_refs_gc(void)
{
	pgstat_shared_ref_hash_iterator i;
	PgStatSharedRefHashEntry *ent;
	uint64		curage;

	curage = pg_atomic_read_u64(&pgStatShmem->gc_count);
	Assert(curage != 0);

	/*
	 * Some entries have been dropped. Invalidate cache pointer to them.
	 */
	pgstat_shared_ref_hash_start_iterate(pgStatSharedRefHash, &i);
	while ((ent = pgstat_shared_ref_hash_iterate(pgStatSharedRefHash, &i)) != NULL)
	{
		PgStatSharedRef *shared_ref = ent->shared_ref;

		Assert(!shared_ref->shared_stats || shared_ref->shared_stats->magic == 0xdeadbeef);

		if (!shared_ref->shared_entry->dropped)
			continue;

		/* cannot gc shared ref that has pending data */
		if (shared_ref->pending != NULL)
			continue;

		pgstat_shared_ref_release(ent->key, shared_ref, false);
	}

	pgStatSharedRefAge = curage;
}

static void
pgstat_shared_refs_release_matching(PgStatSharedRefReleaseMatch match, Datum match_data)
{
	pgstat_shared_ref_hash_iterator i;
	PgStatSharedRefHashEntry *ent;

	if (pgStatSharedRefHash == NULL)
		return;

	pgstat_shared_ref_hash_start_iterate(pgStatSharedRefHash, &i);

	while ((ent = pgstat_shared_ref_hash_iterate(pgStatSharedRefHash, &i))
		   != NULL)
	{
		Assert(ent->shared_ref != NULL);

		if (match && !match(ent, match_data))
			continue;

		/* AFIXME: what if we have pending data? */
		pgstat_shared_ref_release(ent->key, ent->shared_ref, false);
	}
}

/*
 * Release all local references to shared stats entries.
 *
 * When a process exits it cannot do so while still holding references onto
 * stats entries, otherwise the shared stats entries could never be freed.
 */
static void
pgstat_shared_refs_release_all(void)
{
	if (pgStatSharedRefHash == NULL)
		return;

	pgstat_shared_refs_release_matching(NULL, 0);
	Assert(pgStatSharedRefHash->members == 0);
	pgstat_shared_ref_hash_destroy(pgStatSharedRefHash);
	pgStatSharedRefHash = NULL;
}

static bool
pgstat_shared_refs_release_match_db(PgStatSharedRefHashEntry *ent, Datum match_data)
{
	Oid			dboid = DatumGetObjectId(match_data);

	return ent->key.dboid == dboid;
}

static void
pgstat_shared_refs_release_db(Oid dboid)
{
	pgstat_shared_refs_release_matching(pgstat_shared_refs_release_match_db,
										ObjectIdGetDatum(dboid));
}


/* ------------------------------------------------------------
 * Backend-local pending stats infrastructure
 * ------------------------------------------------------------
 */

/*
 * Returns the appropriate PgStatSharedRef, preparing it to receive pending
 * stats if not already done.
 */
PgStatSharedRef *
pgstat_pending_prepare(PgStatKind kind, Oid dboid, Oid objoid, bool *created_shared)
{
	PgStatSharedRef *shared_ref;

	shared_ref = pgstat_shared_ref_get(kind, dboid, objoid,
									   true, created_shared);

	if (shared_ref->pending == NULL)
	{
		size_t		entrysize = pgstat_kind_info_for(kind)->pending_size;

		Assert(entrysize != (size_t) -1);

		shared_ref->pending = MemoryContextAllocZero(pgStatPendingContext, entrysize);
		dlist_push_tail(&pgStatPending, &shared_ref->pending_node);
	}

	return shared_ref;
}

/*
 * Return an existing stats entry, or NULL.
 *
 * This should only be used for helper function for pgstatfuncs.c - outside of
 * that it shouldn't be needed.
 */
PgStatSharedRef *
pgstat_pending_fetch(PgStatKind kind, Oid dboid, Oid objoid)
{
	PgStatSharedRef *shared_ref;

	shared_ref = pgstat_shared_ref_get(kind, dboid, objoid, false, NULL);

	if (shared_ref == NULL || shared_ref->pending == NULL)
		return NULL;

	return shared_ref;
}

static void
pgstat_pending_delete(PgStatSharedRef *shared_ref)
{
	PgStatKind kind = shared_ref->shared_entry->key.kind;
	const PgStatKindInfo *kind_info = pgstat_kind_info_for(kind);
	void	   *pending_data = shared_ref->pending;

	Assert(pending_data != NULL);
	/* !fixed_amount stats should be handled explicitly */
	Assert(!pgstat_kind_info_for(kind)->fixed_amount);

	if (kind_info->pending_delete_cb)
		kind_info->pending_delete_cb(shared_ref);

	pfree(pending_data);
	shared_ref->pending = NULL;

	dlist_delete(&shared_ref->pending_node);
}

/*
 * Flush out pending stats for database objects (databases, relations,
 * functions).
 */
static bool
pgstat_pending_flush_stats(bool nowait)
{
	bool		have_pending = false;
	dlist_node *cur = NULL;

	/*
	 * Need to be a bit careful iterating over the list of pending entries.
	 * Processing a pending entry may queue further pending entries to the end
	 * of the list that we want to process, so a simple iteration won't do.
	 * Further complicating matters is that we want to delete the current
	 * entry in each iteration from the list if we flushed successfully.
	 *
	 * So we just keep track of the next pointer in each loop iteration.
	 */
	if (!dlist_is_empty(&pgStatPending))
		cur = dlist_head_node(&pgStatPending);

	while (cur)
	{
		PgStatSharedRef *shared_ref =
		dlist_container(PgStatSharedRef, pending_node, cur);
		PgStatHashKey key = shared_ref->shared_entry->key;
		PgStatKind	kind = key.kind;
		const PgStatKindInfo *kind_info = pgstat_kind_info_for(kind);
		bool		did_flush;
		dlist_node *next;

		Assert(!kind_info->fixed_amount);
		Assert(kind_info->flush_pending_cb != NULL);

		/* flush the stats, if possible */
		did_flush = kind_info->flush_pending_cb(shared_ref, nowait);

		Assert(did_flush || nowait);

		/* determine next entry, before deleting the pending entry */
		if (dlist_has_next(&pgStatPending, cur))
			next = dlist_next_node(&pgStatPending, cur);
		else
			next = NULL;

		/* if successfully flushed, remove entry */
		if (did_flush)
			pgstat_pending_delete(shared_ref);
		else
			have_pending = true;

		cur = next;
	}

	Assert(dlist_is_empty(&pgStatPending) == !have_pending);

	return have_pending;
}


/* ------------------------------------------------------------
 * Dropping and resetting of stats entries
 * ------------------------------------------------------------
 */

static void
pgstat_shared_stat_free(PgStatShmHashEntry *shent, dshash_seq_status *hstat)
{
	dsa_pointer pdsa = shent->body;

	if (!hstat)
		dshash_delete_entry(pgStatSharedHash, shent);
	else
		dshash_delete_current(hstat);

	dsa_free(pgStatDSA, pdsa);
}

/*
 * Helper for both pgstat_drop_database_and_contents() and
 * pgstat_shared_stat_drop(). If hstat is non-null delete the shared entry using
 * dshash_delete_current(), otherwise use dshash_delete_entry(). In either
 * case the entry needs to be already locked.
 */
static bool
pgstat_shared_stat_drop_internal(PgStatShmHashEntry *shent, dshash_seq_status *hstat)
{
	Assert(shent->body != InvalidDsaPointer);

	/* should already have released local reference */
	if (pgStatSharedRefHash)
		Assert(!pgstat_shared_ref_hash_lookup(pgStatSharedRefHash, shent->key));

	/*
	 * Signal that the entry is dropped - this will eventually cause other
	 * backends to release their references.
	 */
	if (shent->dropped)
		elog(ERROR, "can only drop stats once");
	shent->dropped = true;

	/* release refcount marking entry as not dropped */
	if (pg_atomic_sub_fetch_u32(&shent->refcount, 1) == 0)
	{
		pgstat_shared_stat_free(shent, hstat);
		return true;
	}
	else
	{
		if (!hstat)
			dshash_release_lock(pgStatSharedHash, shent);
		return false;
	}
}

/*
 * Drop stats for the database and all the objects inside that database.
 */
void
pgstat_drop_database_and_contents(Oid dboid)
{
	dshash_seq_status hstat;
	PgStatShmHashEntry *p;
	uint64		not_freed_count = 0;

	Assert(OidIsValid(dboid));

	Assert(pgStatSharedHash != NULL);

	/*
	 * This backend might very well be the only backend holding a reference to
	 * about-to-be-dropped entries. Ensure that we're not preventing it from
	 * being cleaned up till later.
	 *
	 * Doing this separately from the dshash iteration below avoids having to
	 * do so while holding a partition lock on the shared hashtable.
	 */
	pgstat_shared_refs_release_db(dboid);

	/* some of the dshash entries are to be removed, take exclusive lock. */
	dshash_seq_init(&hstat, pgStatSharedHash, true);
	while ((p = dshash_seq_next(&hstat)) != NULL)
	{
		if (p->dropped)
			continue;

		if (p->key.dboid != dboid)
			continue;

		if (!pgstat_shared_stat_drop_internal(p, &hstat))
		{
			/*
			 * Even statistics for a dropped database might currently be
			 * accessed (consider e.g. database stats for pg_stat_database).
			 */
			not_freed_count++;
		}
	}
	dshash_seq_term(&hstat);

	/*
	 * If some of the stats data could not be freed, signal the reference
	 * holders to run garbage collection of their cached pgStatShmLookupCache.
	 */
	if (not_freed_count > 0)
		pgstat_shared_refs_request_gc();
}

static bool
pgstat_shared_stat_drop(const PgStatHashKey *key)
{
	PgStatShmHashEntry *shent;
	bool		freed = true;

	/* delete local reference */
	if (pgStatSharedRefHash)
	{
		PgStatSharedRefHashEntry *lohashent =
		pgstat_shared_ref_hash_lookup(pgStatSharedRefHash, *key);

		if (lohashent)
			pgstat_shared_ref_release(lohashent->key, lohashent->shared_ref, true);
	}

	/* mark entry in shared hashtable as deleted, drop if possible */
	shent = dshash_find(pgStatSharedHash, key, true);
	if (shent)
	{
		freed = pgstat_shared_stat_drop_internal(shent, NULL);

		/*
		 * Database stats contain other stats. Drop those as well when
		 * dropping the database. AFIXME: Perhaps this should be done in a
		 * slightly more principled way?
		 */
		if (key->kind == PGSTAT_KIND_DB)
			pgstat_drop_database_and_contents(key->dboid);
	}

	return freed;
}

void
pgstat_shared_stat_drop_ref(PgStatSharedRef *shared_ref)
{
	PgStatHashKey key = shared_ref->shared_entry->key;

	pgstat_shared_stat_drop(&key);
}

static void
pgstat_shared_stat_reset_one(PgStatKind kind, PgStatShm_StatEntryHeader *header)
{
	const PgStatKindInfo *kind_info = pgstat_kind_info_for(kind);
	TimestampTz ts = GetCurrentTimestamp();

	memset(shared_stat_entry_data(kind, header), 0,
		   shared_stat_entry_len(kind));

	if (kind_info->reset_timestamp_cb)
		kind_info->reset_timestamp_cb(header, ts);
}

/*
 * Reset one non-global shared stat entry.
 */
void
pgstat_reset_one(PgStatKind kind, Oid dboid, Oid objoid)
{
	PgStatSharedRef *shared_ref;

	Assert(!pgstat_kind_info_for(kind)->fixed_amount);

	shared_ref = pgstat_shared_ref_get(kind, dboid, objoid, false, NULL);
	if (!shared_ref || shared_ref->shared_entry->dropped)
		return;

	pgstat_shared_stat_lock(shared_ref, false);
	pgstat_shared_stat_reset_one(kind, shared_ref->shared_stats);
	pgstat_shared_stat_unlock(shared_ref);
}

/*
 * Scan through the shared hashtable for activity stats, resetting statistics
 * approved for resetting by the provided do_reset() function.
 */
void
pgstat_reset_matching(bool (*do_reset) (PgStatShmHashEntry *))
{
	dshash_seq_status hstat;
	PgStatShmHashEntry *p;

	/* dshash entry is not modified, take shared lock */
	dshash_seq_init(&hstat, pgStatSharedHash, false);
	while ((p = dshash_seq_next(&hstat)) != NULL)
	{
		PgStatShm_StatEntryHeader *header;

		if (p->dropped)
			continue;

		if (!do_reset(p))
			continue;

		header = dsa_get_address(pgStatDSA, p->body);

		LWLockAcquire(&p->lock, LW_EXCLUSIVE);

		pgstat_shared_stat_reset_one(p->key.kind, header);

		LWLockRelease(&p->lock);
	}
	dshash_seq_term(&hstat);
}


/* ------------------------------------------------------------
 * Fetching of stats
 * ------------------------------------------------------------
 */

static void
pgstat_fetch_prepare(void)
{
	if (pgstat_fetch_consistency == STATS_FETCH_CONSISTENCY_NONE ||
		stats_snapshot.stats != NULL)
		return;

	if (!pgStatSnapshotContext)
		pgStatSnapshotContext = AllocSetContextCreate(TopMemoryContext,
													  "PgStat Snapshot",
													  ALLOCSET_SMALL_SIZES);

	stats_snapshot.stats = pgstat_snapshot_create(pgStatSnapshotContext,
												  PGSTAT_SNAPSHOT_HASH_SIZE,
												  NULL);
}

static void
pgstat_snapshot_build(void)
{
	dshash_seq_status hstat;
	PgStatShmHashEntry *p;

	/* should only be called when we need a snapshot */
	Assert(pgstat_fetch_consistency == STATS_FETCH_CONSISTENCY_SNAPSHOT);

	/* snapshot already built */
	if (stats_snapshot.mode == STATS_FETCH_CONSISTENCY_SNAPSHOT)
		return;

	pgstat_fetch_prepare();

	Assert(stats_snapshot.stats->members == 0);

	stats_snapshot.snapshot_timestamp = GetCurrentTimestamp();

	/*
	 * Build snapshot all variable stats.
	 */
	dshash_seq_init(&hstat, pgStatSharedHash, false);
	while ((p = dshash_seq_next(&hstat)) != NULL)
	{
		PgStatKind	kind = p->key.kind;
		const PgStatKindInfo *kind_info = pgstat_kind_info_for(kind);
		bool		found;
		PgStatSnapshotEntry *entry;
		PgStatShm_StatEntryHeader *stats_data;

		/*
		 * Check if the stats object should be included in the snapshot.
		 * Unless the stats kind can be accessed from all databases (e.g.
		 * database stats themselves), we only include stats for the current
		 * database or objects not associated with a database (e.g. shared
		 * relations).
		 */
		if (p->key.dboid != MyDatabaseId &&
			p->key.dboid != InvalidOid &&
			!kind_info->accessed_across_databases)
			continue;

		if (p->dropped)
			continue;

		Assert(pg_atomic_read_u32(&p->refcount) > 0);

		stats_data = dsa_get_address(pgStatDSA, p->body);
		Assert(stats_data);

		entry = pgstat_snapshot_insert(stats_snapshot.stats, p->key, &found);
		Assert(!found);

		entry->data = MemoryContextAlloc(pgStatSnapshotContext,
										 kind_info->shared_size);
		memcpy(entry->data,
			   shared_stat_entry_data(kind, stats_data),
			   kind_info->shared_size);
	}
	dshash_seq_term(&hstat);

	/*
	 * Build snapshot of all global stats.
	 */
	for (int kind = 0; kind <= PGSTAT_KIND_LAST; kind++)
	{
		const PgStatKindInfo *kind_info = pgstat_kind_info_for(kind);

		if (!kind_info->fixed_amount)
		{
			Assert(kind_info->snapshot_cb == NULL);
			continue;
		}

		pgstat_snapshot_global_build(kind);
	}

	stats_snapshot.mode = STATS_FETCH_CONSISTENCY_SNAPSHOT;
}

static void
pgstat_snapshot_global_build(PgStatKind kind)
{
	const PgStatKindInfo *kind_info = pgstat_kind_info_for(kind);

	Assert(kind_info->fixed_amount);
	Assert(kind_info->snapshot_cb != NULL);

	if (pgstat_fetch_consistency == STATS_FETCH_CONSISTENCY_NONE)
	{
		/* rebuild every time */
		stats_snapshot.global_valid[kind] = false;
	}
	else if (stats_snapshot.global_valid[kind])
	{
		/* in snapshot mode we shouldn't get called again */
		Assert(pgstat_fetch_consistency == STATS_FETCH_CONSISTENCY_CACHE);
		return;
	}

	Assert(!stats_snapshot.global_valid[kind]);

	kind_info->snapshot_cb();

	Assert(!stats_snapshot.global_valid[kind]);
	stats_snapshot.global_valid[kind] = true;
}

/*
 * Ensure snapshot for a kind of global stats exist.
 *
 * Typically used by the pgstat_fetch_* functions for a kind of stats, before
 * massaging the data into the desired format.
 */
void
pgstat_snapshot_global(PgStatKind kind)
{
	AssertArg(kind <= PGSTAT_KIND_LAST);
	AssertArg(pgstat_kind_info_for(kind)->fixed_amount);

	if (pgstat_fetch_consistency == STATS_FETCH_CONSISTENCY_SNAPSHOT)
		pgstat_snapshot_build();
	else
		pgstat_snapshot_global_build(kind);

	Assert(stats_snapshot.global_valid[kind]);
}

void *
pgstat_fetch_entry(PgStatKind kind, Oid dboid, Oid objoid)
{
	PgStatHashKey key;
	PgStatSharedRef *shared_ref;
	void	   *stats_data;
	const PgStatKindInfo *kind_info = pgstat_kind_info_for(kind);

	/* should be called from backends */
	Assert(IsUnderPostmaster || !IsPostmasterEnvironment);
	AssertArg(!kind_info->fixed_amount);

	pgstat_fetch_prepare();

	key.kind = kind;
	key.dboid = dboid;
	key.objoid = objoid;

	/* if we need to build a full snapshot, do so */
	if (pgstat_fetch_consistency == STATS_FETCH_CONSISTENCY_SNAPSHOT)
		pgstat_snapshot_build();

	/* if caching is desired, look up in cache */
	if (pgstat_fetch_consistency > STATS_FETCH_CONSISTENCY_NONE)
	{
		PgStatSnapshotEntry *entry = NULL;

		entry = pgstat_snapshot_lookup(stats_snapshot.stats, key);

		if (entry)
			return entry->data;

		/*
		 * If we built a full snapshot and the key is not in stats_snapshot.stats,
		 * there are no matching stats.
		 */
		if (pgstat_fetch_consistency == STATS_FETCH_CONSISTENCY_SNAPSHOT)
			return NULL;
	}

	stats_snapshot.mode = pgstat_fetch_consistency;

	shared_ref = pgstat_shared_ref_get(kind, dboid, objoid, false, NULL);

	if (shared_ref == NULL || shared_ref->shared_entry->dropped)
	{
		/*
		 * AFIXME: create empty entry when using STATS_FETCH_CONSISTENCY_CACHE
		 */
		return NULL;
	}

	/*
	 * Allocate in caller's context for STATS_FETCH_CONSISTENCY_NONE,
	 * otherwise we could quickly end up with a fair bit of memory used due to
	 * repeated accesses.
	 */
	if (pgstat_fetch_consistency == STATS_FETCH_CONSISTENCY_NONE)
		stats_data = palloc(kind_info->shared_data_len);
	else
		stats_data = MemoryContextAlloc(pgStatSnapshotContext,
										kind_info->shared_data_len);
	memcpy(stats_data,
		   shared_stat_entry_data(kind, shared_ref->shared_stats),
		   kind_info->shared_data_len);

	if (pgstat_fetch_consistency > STATS_FETCH_CONSISTENCY_NONE)
	{
		PgStatSnapshotEntry *entry = NULL;
		bool		found;

		entry = pgstat_snapshot_insert(stats_snapshot.stats, key, &found);
		entry->data = stats_data;
	}

	return stats_data;
}
