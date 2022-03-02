/* -------------------------------------------------------------------------
 *
 * pgstat_subscription.c
 *	  Implementation of subscription statistics.
 *
 * This file contains the implementation of subscription statistics. It is kept
 * separate from pgstat.c to enforce the line between the statistics access /
 * storage implementation and the details about individual types of
 * statistics.
 *
 * Copyright (c) 2001-2022, PostgreSQL Global Development Group
 *
 * IDENTIFICATION
 *	  src/backend/utils/activity/pgstat_subscription.c
 * -------------------------------------------------------------------------
 */

#include "postgres.h"

#include "utils/pgstat_internal.h"
#include "utils/timestamp.h"


/*
 * pgstat_flush_subscription - flush out a local subscription stats entry
 *
 * If nowait is true, this function returns false on lock failure. Otherwise
 * this function always returns true.
 *
 * Returns true if the entry is successfully flushed out.
 */
bool
pgstat_flush_subscription(PgStatSharedRef *shared_ref, bool nowait)
{
	PgStat_BackendSubEntry *localent;	/* local stats entry */
	PgStatShm_StatSubEntry *shsubent = NULL; /* shared stats entry */

	Assert(shared_ref->shared_entry->key.kind == PGSTAT_KIND_SUBSCRIPTION);
	localent = (PgStat_BackendSubEntry *) shared_ref->pending;

	/* localent always has non-zero content */

	if (!pgstat_shared_stat_lock(shared_ref, nowait))
		return false;			/* failed to acquire lock, skip */

	shsubent = (PgStatShm_StatSubEntry *) shared_ref->shared_stats;

	shsubent->stats.apply_error_count += localent->apply_error_count;
	shsubent->stats.sync_error_count += localent->sync_error_count;

	pgstat_shared_stat_unlock(shared_ref);
	return true;
}

/*
 * Ensure that stats are dropped if transaction commits.
 */
void
pgstat_report_subscription_drop(Oid subid)
{

	pgstat_schedule_drop(PGSTAT_KIND_SUBSCRIPTION,
						 subid, InvalidOid);
}

/* ----------
 * pgstat_report_subscription_error() -
 *
 *	Tell the collector about the subscription worker error.
 * ----------
 */
void
pgstat_report_subscription_error(Oid subid, bool is_apply_error)
{
	PgStatSharedRef *shared_ref;
	PgStat_BackendSubEntry *pending;

	if (!pgstat_track_counts)
		return;

	shared_ref = pgstat_pending_prepare(PGSTAT_KIND_SUBSCRIPTION,
										subid,
										InvalidOid);

	pending = shared_ref->pending;
	if (is_apply_error)
		pending->apply_error_count++;
	else
		pending->sync_error_count++;
}

/*
 * ---------
 * pgstat_fetch_stat_subscription() -
 *
 *	Support function for the SQL-callable pgstat* functions. Returns
 *	the collected statistics for one subscription or NULL.
 * ---------
 */
PgStat_StatSubEntry *
pgstat_fetch_stat_subscription(Oid subid)
{
	return (PgStat_StatSubEntry *)
		pgstat_fetch_entry(PGSTAT_KIND_SUBSCRIPTION, subid, InvalidOid);
}
