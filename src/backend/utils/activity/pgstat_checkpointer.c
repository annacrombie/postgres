/* -------------------------------------------------------------------------
 *
 * pgstat_checkpointer.c
 *	  Implementation of checkpoint statistics.
 *
 * This file contains the implementation of checkpoint statistics. It is kept
 * separate from pgstat.c to enforce the line between the statistics access /
 * storage implementation and the details about individual types of
 * statistics.
 *
 * Copyright (c) 2001-2022, PostgreSQL Global Development Group
 *
 * IDENTIFICATION
 *	  src/backend/utils/activity/pgstat_checkpointer.c
 * -------------------------------------------------------------------------
 */

#include "postgres.h"

#include "utils/pgstat_internal.h"


PgStat_CheckpointerStats PendingCheckpointerStats = {0};


/* ----------
 * pgstat_report_checkpointer() -
 *
 * Report checkpointer statistics
 * ----------
 */
void
pgstat_report_checkpointer(void)
{
	/* We assume this initializes to zeroes */
	static const PgStat_CheckpointerStats all_zeroes;

	Assert(!pgStatShmem->is_shutdown);
	pgstat_assert_is_up();

	/*
	 * This function can be called even if nothing at all has happened. In
	 * this case, avoid taking lock for a completely empty stats.
	 */
	if (memcmp(&PendingCheckpointerStats, &all_zeroes,
			   sizeof(PgStat_CheckpointerStats)) == 0)
		return;

	changecount_before_write(&pgStatShmem->checkpointer.changecount);

#define CHECKPOINTER_ACC(fld) pgStatShmem->checkpointer.stats.fld += PendingCheckpointerStats.fld
	CHECKPOINTER_ACC(timed_checkpoints);
	CHECKPOINTER_ACC(requested_checkpoints);
	CHECKPOINTER_ACC(checkpoint_write_time);
	CHECKPOINTER_ACC(checkpoint_sync_time);
	CHECKPOINTER_ACC(buf_written_checkpoints);
	CHECKPOINTER_ACC(buf_written_backend);
	CHECKPOINTER_ACC(buf_fsync_backend);
#undef CHECKPOINTER_ACC

	changecount_after_write(&pgStatShmem->checkpointer.changecount);

	/*
	 * Clear out the statistics buffer, so it can be re-used.
	 */
	MemSet(&PendingCheckpointerStats, 0, sizeof(PendingCheckpointerStats));
}

/*
 * ---------
 * pgstat_fetch_stat_checkpointer() -
 *
 *	Support function for the SQL-callable pgstat* functions. Returns
 *	a pointer to the checkpointer statistics struct.
 * ---------
 */
PgStat_CheckpointerStats *
pgstat_fetch_stat_checkpointer(void)
{
	pgstat_snapshot_global(PGSTAT_KIND_CHECKPOINTER);

	return &stats_snapshot.checkpointer;
}

void
pgstat_checkpointer_reset_all_cb(TimestampTz now)
{
	LWLockAcquire(StatsLock, LW_EXCLUSIVE);
	pgstat_copy_global_stats(&pgStatShmem->checkpointer.reset_offset,
							 &pgStatShmem->checkpointer.stats,
							 sizeof(PgStat_CheckpointerStats),
							 &pgStatShmem->checkpointer.changecount);
	pgStatShmem->bgwriter.reset_offset.stat_reset_timestamp = now;
	LWLockRelease(StatsLock);
}

void
pgstat_checkpointer_snapshot_cb(void)
{
	PgStat_CheckpointerStats reset;
	PgStat_CheckpointerStats *reset_offset = &pgStatShmem->checkpointer.reset_offset;

	pgstat_copy_global_stats(&stats_snapshot.checkpointer,
							 &pgStatShmem->checkpointer.stats,
							 sizeof(PgStat_CheckpointerStats),
							 &pgStatShmem->checkpointer.changecount);

	LWLockAcquire(StatsLock, LW_SHARED);
	memcpy(&reset, reset_offset, sizeof(PgStat_CheckpointerStats));
	LWLockRelease(StatsLock);

	/* compensate by reset offsets */
#define CHECKPOINTER_COMP(fld) stats_snapshot.checkpointer.fld -= reset.fld;
	CHECKPOINTER_COMP(timed_checkpoints);
	CHECKPOINTER_COMP(requested_checkpoints);
	CHECKPOINTER_COMP(checkpoint_write_time);
	CHECKPOINTER_COMP(checkpoint_sync_time);
	CHECKPOINTER_COMP(buf_written_checkpoints);
	CHECKPOINTER_COMP(buf_written_backend);
	CHECKPOINTER_COMP(buf_fsync_backend);
#undef CHECKPOINTER_COMP
}
