/* -------------------------------------------------------------------------
 *
 * pgstat_replslot.c
 *	  Implementation of replication slot statistics.
 *
 * This file contains the implementation of replication slot statistics. It is kept
 * separate from pgstat.c to enforce the line between the statistics access /
 * storage implementation and the details about individual types of
 * statistics.
 *
 * Copyright (c) 2001-2022, PostgreSQL Global Development Group
 *
 * IDENTIFICATION
 *	  src/backend/utils/activity/pgstat_replslot.c
 * -------------------------------------------------------------------------
 */

#include "postgres.h"

#include "replication/slot.h"
#include "utils/builtins.h"		/* for namestrcpy() */
#include "utils/memutils.h"
#include "utils/pgstat_internal.h"


static inline bool pgstat_replslot_in_use(int idx);


/* ----------
 * pgstat_reset_replslot_counter() -
 *
 *	Tell the statistics subsystem to reset a single replication slot
 *	counter, or all replication slots counters (when name is null).
 *
 *	Permission checking for this function is managed through the normal
 *	GRANT system.
 * ----------
 */
void
pgstat_reset_replslot_counter(const char *name)
{
	int			idx;
	TimestampTz ts;
	ReplicationSlot *slot;
	PgStat_StatReplSlotEntry *statent;
	size_t		off;

	AssertArg(name != NULL);

	/*
	 * AFIXME: pgstats has business no looking into slot.c structures at this
	 * level of detail.
	 */

	/* Check if the slot exits with the given name. */
	slot = SearchNamedReplicationSlot(name, true);

	if (!slot)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				 errmsg("replication slot \"%s\" does not exist",
						name)));

	/*
	 * Nothing to do for physical slots as we collect stats only for logical
	 * slots.
	 */
	if (SlotIsPhysical(slot))
		return;

	/* reset this one entry */
	idx = slot - ReplicationSlotCtl->replication_slots;

	ts = GetCurrentTimestamp();

	LWLockAcquire(&pgStatShmem->replslot.lock, LW_EXCLUSIVE);

	statent = &pgStatShmem->replslot.stats[idx];
	off = offsetof(PgStat_StatReplSlotEntry, slotname) + sizeof(NameData);
	memset(((char *) statent) + off, 0, sizeof(pgStatShmem->replslot.stats[idx]) - off);
	statent->stat_reset_timestamp = ts;

	LWLockRelease(&pgStatShmem->replslot.lock);
}

/* ----------
 * pgstat_report_replslot() -
 *
 * Report replication slot activity.
 * ----------
 */
void
pgstat_report_replslot(uint32 index, const PgStat_StatReplSlotEntry *repSlotStat)
{
	PgStat_StatReplSlotEntry *statent;

	Assert(index < max_replication_slots);

	if (!pgstat_track_counts)
		return;

	statent = &pgStatShmem->replslot.stats[index];

	LWLockAcquire(&pgStatShmem->replslot.lock, LW_EXCLUSIVE);

	/* clear the counters if not used */
	if (!pgstat_replslot_in_use(index))
	{
		memset(statent, 0, sizeof(*statent));
		namestrcpy(&statent->slotname, NameStr(repSlotStat->slotname));
	}
	else if (namestrcmp(&statent->slotname, NameStr(statent->slotname)) != 0)
	{
		/* AFIXME: Is there a valid way this can happen? */
		elog(ERROR, "stats out of sync");
	}

	/* Update the replication slot statistics */
	statent->spill_txns += repSlotStat->spill_txns;
	statent->spill_count += repSlotStat->spill_count;
	statent->spill_bytes += repSlotStat->spill_bytes;
	statent->stream_txns += repSlotStat->stream_txns;
	statent->stream_count += repSlotStat->stream_count;
	statent->stream_bytes += repSlotStat->stream_bytes;
	statent->total_txns += repSlotStat->total_txns;
	statent->total_bytes += repSlotStat->total_bytes;

	LWLockRelease(&pgStatShmem->replslot.lock);
}

/* ----------
 * pgstat_report_replslot_create() -
 *
 * Report replication slot creation.
 * ----------
 */
void
pgstat_report_replslot_create(const char *name, uint32 index)
{
	PgStat_StatReplSlotEntry *statent;

	if (!pgstat_track_counts)
		return;

	statent = &pgStatShmem->replslot.stats[index];

	LWLockAcquire(&pgStatShmem->replslot.lock, LW_EXCLUSIVE);

	/*
	 * NB: need to accept that there might be stats from an older slot, e.g.
	 * if we previously crashed after dropping a slot.
	 */
	memset(statent, 0, sizeof(*statent));
	namestrcpy(&statent->slotname, name);

	LWLockRelease(&pgStatShmem->replslot.lock);
}

/* ----------
 * pgstat_report_replslot_drop() -
 *
 * Report replication slot drop.
 * ----------
 */
void
pgstat_report_replslot_drop(uint32 index)
{
	PgStat_StatReplSlotEntry *statent;

	if (!pgstat_track_counts)
		return;

	statent = &pgStatShmem->replslot.stats[index];

	LWLockAcquire(&pgStatShmem->replslot.lock, LW_EXCLUSIVE);

	/*
	 * NB: need to accept that there might not be any stats, e.g. if we threw
	 * away stats after a crash restart.
	 */
	memset(statent, 0, sizeof(*statent));
	LWLockRelease(&pgStatShmem->replslot.lock);
}

/*
 * ---------
 * pgstat_fetch_replslot() -
 *
 *	Support function for the SQL-callable pgstat* functions. Returns
 *	a pointer to the replication slot statistics struct.
 * ---------
 */
PgStat_StatReplSlotEntry *
pgstat_fetch_replslot(NameData slotname)
{
	pgstat_snapshot_global(PGSTAT_KIND_REPLSLOT);

	for (int i = 0; i < stats_snapshot.replslot_count; i++)
	{
		PgStat_StatReplSlotEntry *statentry = &stats_snapshot.replslot[i];

		if (namestrcmp(&statentry->slotname, NameStr(slotname)) == 0)
		{
			return statentry;
		}
	}

	return NULL;
}

void
pgstat_replslot_restore(const PgStat_StatReplSlotEntry *tmp)
{
	int			idx;
	ReplicationSlot *slot;

	slot = SearchNamedReplicationSlot(NameStr(tmp->slotname), true);

	/* slot might have been deleted */
	if (!slot)
		return;

	idx = slot - ReplicationSlotCtl->replication_slots;
	Assert(!pgstat_replslot_in_use(idx));
	pgStatShmem->replslot.stats[idx] = *tmp;
}

void
pgstat_replslot_reset_all_cb(TimestampTz now)
{
	LWLockAcquire(&pgStatShmem->replslot.lock, LW_EXCLUSIVE);
	for (int i = 0; i <= max_replication_slots - 1; i++)
	{
		PgStat_StatReplSlotEntry *statent;
		size_t		off;

		if (!pgstat_replslot_in_use(i))
			continue;

		statent = &pgStatShmem->replslot.stats[i];
		off = offsetof(PgStat_StatReplSlotEntry, slotname) + sizeof(NameData);

		memset(((char *) statent) + off, 0, sizeof(pgStatShmem->replslot.stats[i]) - off);
		statent->stat_reset_timestamp = now;
	}
	LWLockRelease(&pgStatShmem->replslot.lock);
}

void
pgstat_replslot_snapshot_cb(void)
{
	/* first time through, allocate space for snapshot */
	if (stats_snapshot.replslot == NULL)
	{
		stats_snapshot.replslot = (PgStat_StatReplSlotEntry *)
			MemoryContextAlloc(TopMemoryContext,
							   sizeof(PgStat_StatReplSlotEntry) * max_replication_slots);
	}

	stats_snapshot.replslot_count = 0;

	LWLockAcquire(&pgStatShmem->replslot.lock, LW_EXCLUSIVE);

	for (int i = 0; i < max_replication_slots; i++)
	{
		PgStat_StatReplSlotEntry *statent = &pgStatShmem->replslot.stats[i];

		if (!pgstat_replslot_in_use(i))
			continue;

		stats_snapshot.replslot[stats_snapshot.replslot_count++] = *statent;
	}

	LWLockRelease(&pgStatShmem->replslot.lock);
}

static inline bool
pgstat_replslot_in_use(int idx)
{
	return NameStr(pgStatShmem->replslot.stats[idx].slotname)[0] != 0;
}
