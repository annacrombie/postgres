/* -------------------------------------------------------------------------
 *
 * pgstat_archiver.c
 *	  Implementation of archiver statistics.
 *
 * This file contains the implementation of archiver statistics. It is kept
 * separate from pgstat.c to enforce the line between the statistics access /
 * storage implementation and the details about individual types of
 * statistics.
 *
 * Copyright (c) 2001-2022, PostgreSQL Global Development Group
 *
 * IDENTIFICATION
 *	  src/backend/utils/activity/pgstat_archiver.c
 * -------------------------------------------------------------------------
 */

#include "postgres.h"

#include "utils/pgstat_internal.h"
#include "utils/timestamp.h"


/* ----------
 * pgstat_report_archiver() -
 *
 * Report archiver statistics
 * ----------
 */
void
pgstat_report_archiver(const char *xlog, bool failed)
{
	TimestampTz now = GetCurrentTimestamp();

	changecount_before_write(&pgStatShmem->archiver.changecount);

	if (failed)
	{
		++pgStatShmem->archiver.stats.failed_count;
		memcpy(&pgStatShmem->archiver.stats.last_failed_wal, xlog,
			   sizeof(pgStatShmem->archiver.stats.last_failed_wal));
		pgStatShmem->archiver.stats.last_failed_timestamp = now;
	}
	else
	{
		++pgStatShmem->archiver.stats.archived_count;
		memcpy(&pgStatShmem->archiver.stats.last_archived_wal, xlog,
			   sizeof(pgStatShmem->archiver.stats.last_archived_wal));
		pgStatShmem->archiver.stats.last_archived_timestamp = now;
	}

	changecount_after_write(&pgStatShmem->archiver.changecount);
}

/*
 * ---------
 * pgstat_fetch_stat_archiver() -
 *
 *	Support function for the SQL-callable pgstat* functions. Returns
 *	a pointer to the archiver statistics struct.
 * ---------
 */
PgStat_ArchiverStats *
pgstat_fetch_stat_archiver(void)
{
	pgstat_snapshot_global(PGSTAT_KIND_ARCHIVER);

	return &stats_snapshot.archiver;
}

void
pgstat_archiver_reset_all_cb(TimestampTz now)
{
	LWLockAcquire(StatsLock, LW_EXCLUSIVE);
	pgstat_copy_global_stats(&pgStatShmem->archiver.reset_offset,
							 &pgStatShmem->archiver.stats,
							 sizeof(PgStat_ArchiverStats),
							 &pgStatShmem->archiver.changecount);
	pgStatShmem->archiver.stats.stat_reset_timestamp = now;
	LWLockRelease(StatsLock);
}

void
pgstat_archiver_snapshot_cb(void)
{
	PgStat_ArchiverStats reset;
	PgStat_ArchiverStats *reset_offset = &pgStatShmem->archiver.reset_offset;

	pgstat_copy_global_stats(&stats_snapshot.archiver,
							 &pgStatShmem->archiver.stats,
							 sizeof(PgStat_ArchiverStats),
							 &pgStatShmem->archiver.changecount);

	LWLockAcquire(StatsLock, LW_SHARED);
	memcpy(&reset, reset_offset, sizeof(PgStat_ArchiverStats));
	LWLockRelease(StatsLock);

	/* compensate by reset offsets */
	if (stats_snapshot.archiver.archived_count == reset.archived_count)
	{
		stats_snapshot.archiver.last_archived_wal[0] = 0;
		stats_snapshot.archiver.last_archived_timestamp = 0;
	}
	stats_snapshot.archiver.archived_count -= reset.archived_count;

	if (stats_snapshot.archiver.failed_count == reset.failed_count)
	{
		stats_snapshot.archiver.last_failed_wal[0] = 0;
		stats_snapshot.archiver.last_failed_timestamp = 0;
	}
	stats_snapshot.archiver.failed_count -= reset.failed_count;
}
