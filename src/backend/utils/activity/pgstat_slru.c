/* -------------------------------------------------------------------------
 *
 * pgstat_slru.c
 *	  Implementation of SLRU statistics.
 *
 * This file contains the implementation of SLRU statistics. It is kept
 * separate from pgstat.c to enforce the line between the statistics access /
 * storage implementation and the details about individual types of
 * statistics.
 *
 * Copyright (c) 2001-2022, PostgreSQL Global Development Group
 *
 * IDENTIFICATION
 *	  src/backend/utils/activity/pgstat_slru.c
 * -------------------------------------------------------------------------
 */

#include "postgres.h"

#include "utils/pgstat_internal.h"
#include "utils/timestamp.h"


static inline PgStat_SLRUStats *slru_entry(int slru_idx);
static void pgstat_reset_slru_counter_internal(int index, TimestampTz ts);


/*
 * SLRU statistics counts waiting to be written to the shared activity
 * statistics.  We assume this variable inits to zeroes.  Entries are
 * one-to-one with slru_names[].
 * Changes of SLRU counters are reported within critical sections so we use
 * static memory in order to avoid memory allocation.
 */
static PgStat_SLRUStats pending_SLRUStats[SLRU_NUM_ELEMENTS];
bool		have_slrustats = false;


/* ----------
 * pgstat_reset_slru_counter() -
 *
 *	Tell the statistics subsystem to reset a single SLRU counter.
 *
 *	Permission checking for this function is managed through the normal
 *	GRANT system.
 * ----------
 */
void
pgstat_reset_slru_counter(const char *name)
{
	TimestampTz ts = GetCurrentTimestamp();

	AssertArg(name != NULL);

	pgstat_reset_slru_counter_internal(pgstat_slru_index(name), ts);
}

/*
 * SLRU statistics count accumulation functions --- called from slru.c
 */

void
pgstat_count_slru_page_zeroed(int slru_idx)
{
	slru_entry(slru_idx)->blocks_zeroed += 1;
}

void
pgstat_count_slru_page_hit(int slru_idx)
{
	slru_entry(slru_idx)->blocks_hit += 1;
}

void
pgstat_count_slru_page_exists(int slru_idx)
{
	slru_entry(slru_idx)->blocks_exists += 1;
}

void
pgstat_count_slru_page_read(int slru_idx)
{
	slru_entry(slru_idx)->blocks_read += 1;
}

void
pgstat_count_slru_page_written(int slru_idx)
{
	slru_entry(slru_idx)->blocks_written += 1;
}

void
pgstat_count_slru_flush(int slru_idx)
{
	slru_entry(slru_idx)->flush += 1;
}

void
pgstat_count_slru_truncate(int slru_idx)
{
	slru_entry(slru_idx)->truncate += 1;
}

/*
 * ---------
 * pgstat_fetch_slru() -
 *
 *	Support function for the SQL-callable pgstat* functions. Returns
 *	a pointer to the slru statistics struct.
 * ---------
 */
PgStat_SLRUStats *
pgstat_fetch_slru(void)
{
	pgstat_snapshot_global(PGSTAT_KIND_SLRU);

	return stats_snapshot.slru;
}

/*
 * pgstat_slru_name
 *
 * Returns SLRU name for an index. The index may be above SLRU_NUM_ELEMENTS,
 * in which case this returns NULL. This allows writing code that does not
 * know the number of entries in advance.
 */
const char *
pgstat_slru_name(int slru_idx)
{
	if (slru_idx < 0 || slru_idx >= SLRU_NUM_ELEMENTS)
		return NULL;

	return slru_names[slru_idx];
}

/*
 * pgstat_slru_index
 *
 * Determine index of entry for a SLRU with a given name. If there's no exact
 * match, returns index of the last "other" entry used for SLRUs defined in
 * external projects.
 */
int
pgstat_slru_index(const char *name)
{
	int			i;

	for (i = 0; i < SLRU_NUM_ELEMENTS; i++)
	{
		if (strcmp(slru_names[i], name) == 0)
			return i;
	}

	/* return index of the last entry (which is the "other" one) */
	return (SLRU_NUM_ELEMENTS - 1);
}

/* ----------
 * pgstat_slru_flush - flush out locally pending SLRU stats entries
 *
 * If nowait is true, this function returns false on lock failure. Otherwise
 * this function always returns true. Writer processes are mutually excluded
 * using LWLock, but readers are expected to use change-count protocol to avoid
 * interference with writers.
 *
 * Returns true if not all pending stats have been flushed out.
 * ----------
 */
bool
pgstat_slru_flush(bool nowait)
{
	int			i;

	if (!have_slrustats)
		return false;

	/* lock the shared entry to protect the content, skip if failed */
	if (!nowait)
		LWLockAcquire(&pgStatShmem->slru.lock, LW_EXCLUSIVE);
	else if (!LWLockConditionalAcquire(&pgStatShmem->slru.lock, LW_EXCLUSIVE))
		return true;			/* failed to acquire lock, skip */


	for (i = 0; i < SLRU_NUM_ELEMENTS; i++)
	{
		PgStat_SLRUStats *sharedent = &pgStatShmem->slru.stats[i];
		PgStat_SLRUStats *pendingent = &pending_SLRUStats[i];

		sharedent->blocks_zeroed += pendingent->blocks_zeroed;
		sharedent->blocks_hit += pendingent->blocks_hit;
		sharedent->blocks_read += pendingent->blocks_read;
		sharedent->blocks_written += pendingent->blocks_written;
		sharedent->blocks_exists += pendingent->blocks_exists;
		sharedent->flush += pendingent->flush;
		sharedent->truncate += pendingent->truncate;
	}

	/* done, clear the pending entry */
	MemSet(pending_SLRUStats, 0, SizeOfSlruStats);

	LWLockRelease(&pgStatShmem->slru.lock);

	have_slrustats = false;

	return false;
}

void
pgstat_slru_reset_all_cb(TimestampTz now)
{
	for (int i = 0; i < SLRU_NUM_ELEMENTS; i++)
		pgstat_reset_slru_counter_internal(i, now);
}

void
pgstat_slru_snapshot_cb(void)
{
	LWLockAcquire(&pgStatShmem->slru.lock, LW_SHARED);

	memcpy(stats_snapshot.slru, &pgStatShmem->slru.stats, SizeOfSlruStats);

	LWLockRelease(&pgStatShmem->slru.lock);
}

/*
 * slru_entry
 *
 * Returns pointer to entry with counters for given SLRU (based on the name
 * stored in SlruCtl as lwlock tranche name).
 */
static inline PgStat_SLRUStats *
slru_entry(int slru_idx)
{
	pgstat_assert_is_up();

	/*
	 * The postmaster should never register any SLRU statistics counts; if it
	 * did, the counts would be duplicated into child processes via fork().
	 */
	Assert(IsUnderPostmaster || !IsPostmasterEnvironment);

	Assert((slru_idx >= 0) && (slru_idx < SLRU_NUM_ELEMENTS));

	have_slrustats = true;

	return &pending_SLRUStats[slru_idx];
}

static void
pgstat_reset_slru_counter_internal(int index, TimestampTz ts)
{
	LWLockAcquire(&pgStatShmem->slru.lock, LW_EXCLUSIVE);

	memset(&pgStatShmem->slru.stats[index], 0, sizeof(PgStat_SLRUStats));
	pgStatShmem->slru.stats[index].stat_reset_timestamp = ts;

	LWLockRelease(&pgStatShmem->slru.lock);
}
