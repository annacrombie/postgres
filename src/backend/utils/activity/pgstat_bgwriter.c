/* -------------------------------------------------------------------------
 *
 * pgstat_bgwriter.c
 *	  Implementation of bgwriter statistics.
 *
 * This file contains the implementation of bgwriter statistics. It is kept
 * separate from pgstat.c to enforce the line between the statistics access /
 * storage implementation and the details about individual types of
 * statistics.
 *
 * Copyright (c) 2001-2022, PostgreSQL Global Development Group
 *
 * IDENTIFICATION
 *	  src/backend/utils/activity/pgstat_bgwriter.c
 * -------------------------------------------------------------------------
 */

#include "postgres.h"

#include "utils/pgstat_internal.h"


PgStat_BgWriterStats PendingBgWriterStats = {0};


/* ----------
 * pgstat_report_bgwriter() -
 *
 *		Report bgwriter statistics
 * ----------
 */
void
pgstat_report_bgwriter(void)
{
	static const PgStat_BgWriterStats all_zeroes;

	Assert(!pgStatShmem->is_shutdown);
	pgstat_assert_is_up();

	/*
	 * This function can be called even if nothing at all has happened. In
	 * this case, avoid taking lock for a completely empty stats.
	 */
	if (memcmp(&PendingBgWriterStats, &all_zeroes, sizeof(PgStat_BgWriterStats)) == 0)
		return;

	changecount_before_write(&pgStatShmem->bgwriter.changecount);

#define BGWRITER_ACC(fld) pgStatShmem->bgwriter.stats.fld += PendingBgWriterStats.fld
	BGWRITER_ACC(buf_written_clean);
	BGWRITER_ACC(maxwritten_clean);
	BGWRITER_ACC(buf_alloc);
#undef BGWRITER_ACC

	changecount_after_write(&pgStatShmem->bgwriter.changecount);

	/*
	 * Clear out the statistics buffer, so it can be re-used.
	 */
	MemSet(&PendingBgWriterStats, 0, sizeof(PendingBgWriterStats));
}

/*
 * ---------
 * pgstat_fetch_stat_bgwriter() -
 *
 *	Support function for the SQL-callable pgstat* functions. Returns
 *	a pointer to the bgwriter statistics struct.
 * ---------
 */
PgStat_BgWriterStats *
pgstat_fetch_stat_bgwriter(void)
{
	pgstat_snapshot_global(PGSTAT_KIND_BGWRITER);

	return &stats_snapshot.bgwriter;
}

void
pgstat_bgwriter_reset_all_cb(TimestampTz now)
{
	LWLockAcquire(StatsLock, LW_EXCLUSIVE);
	pgstat_copy_global_stats(&pgStatShmem->bgwriter.reset_offset,
							 &pgStatShmem->bgwriter.stats,
							 sizeof(PgStat_BgWriterStats),
							 &pgStatShmem->bgwriter.changecount);
	pgStatShmem->bgwriter.stats.stat_reset_timestamp = now;
	LWLockRelease(StatsLock);
}

void
pgstat_bgwriter_snapshot_cb(void)
{
	PgStat_BgWriterStats reset;
	PgStat_BgWriterStats *reset_offset = &pgStatShmem->bgwriter.reset_offset;

	pgstat_copy_global_stats(&stats_snapshot.bgwriter,
							 &pgStatShmem->bgwriter.stats,
							 sizeof(PgStat_BgWriterStats),
							 &pgStatShmem->bgwriter.changecount);

	LWLockAcquire(StatsLock, LW_SHARED);
	memcpy(&reset, reset_offset, sizeof(PgStat_BgWriterStats));
	LWLockRelease(StatsLock);

	/* compensate by reset offsets */
#define BGWRITER_COMP(fld) stats_snapshot.bgwriter.fld -= reset.fld;
	BGWRITER_COMP(buf_written_clean);
	BGWRITER_COMP(maxwritten_clean);
	BGWRITER_COMP(buf_alloc);
#undef BGWRITER_COMP
}
