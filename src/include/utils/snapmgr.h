/*-------------------------------------------------------------------------
 *
 * snapmgr.h
 *	  POSTGRES snapshot manager
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/utils/snapmgr.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef SNAPMGR_H
#define SNAPMGR_H

#include "access/transam.h"
#include "utils/relcache.h"
#include "utils/resowner.h"
#include "utils/snapshot.h"


/*
 * The structure used to map times to TransactionId values for the "snapshot
 * too old" feature must have a few entries at the tail to hold old values;
 * otherwise the lookup will often fail and the expected early pruning or
 * vacuum will not usually occur.  It is best if this padding is for a number
 * of minutes greater than a thread would normally be stalled, but it's OK if
 * early vacuum opportunities are occasionally missed, so there's no need to
 * use an extreme value or get too fancy.  10 minutes seems plenty.
 */
#define OLD_SNAPSHOT_PADDING_ENTRIES 10
#define OLD_SNAPSHOT_TIME_MAP_ENTRIES (old_snapshot_threshold + OLD_SNAPSHOT_PADDING_ENTRIES)

/*
 * Common definition of relation properties that allow early pruning/vacuuming
 * when old_snapshot_threshold >= 0.
 */
#define RelationAllowsEarlyPruning(rel) \
( \
	 RelationIsPermanent(rel) && !IsCatalogRelation(rel) \
  && !RelationIsAccessibleInLogicalDecoding(rel) \
)

#define EarlyPruningEnabled(rel) (old_snapshot_threshold >= 0 && RelationAllowsEarlyPruning(rel))

/* GUC variables */
extern PGDLLIMPORT int old_snapshot_threshold;


extern PGDLLIMPORT Size SnapMgrShmemSize(void);
extern PGDLLIMPORT void SnapMgrInit(void);
extern PGDLLIMPORT TimestampTz GetSnapshotCurrentTimestamp(void);
extern PGDLLIMPORT TimestampTz GetOldSnapshotThresholdTimestamp(void);
extern PGDLLIMPORT void SnapshotTooOldMagicForTest(void);

extern PGDLLIMPORT bool FirstSnapshotSet;

extern PGDLLIMPORT TransactionId TransactionXmin;
extern PGDLLIMPORT TransactionId RecentXmin;

/* Variables representing various special snapshot semantics */
extern PGDLLIMPORT SnapshotData SnapshotSelfData;
extern PGDLLIMPORT SnapshotData SnapshotAnyData;
extern PGDLLIMPORT SnapshotData CatalogSnapshotData;

#define SnapshotSelf		(&SnapshotSelfData)
#define SnapshotAny			(&SnapshotAnyData)

/*
 * We don't provide a static SnapshotDirty variable because it would be
 * non-reentrant.  Instead, users of that snapshot type should declare a
 * local variable of type SnapshotData, and initialize it with this macro.
 */
#define InitDirtySnapshot(snapshotdata)  \
	((snapshotdata).snapshot_type = SNAPSHOT_DIRTY)

/*
 * Similarly, some initialization is required for a NonVacuumable snapshot.
 * The caller must supply the visibility cutoff state to use (c.f.
 * GlobalVisTestFor()).
 */
#define InitNonVacuumableSnapshot(snapshotdata, vistestp)  \
	((snapshotdata).snapshot_type = SNAPSHOT_NON_VACUUMABLE, \
	 (snapshotdata).vistest = (vistestp))

/*
 * Similarly, some initialization is required for SnapshotToast.  We need
 * to set lsn and whenTaken correctly to support snapshot_too_old.
 */
#define InitToastSnapshot(snapshotdata, l, w)  \
	((snapshotdata).snapshot_type = SNAPSHOT_TOAST, \
	 (snapshotdata).lsn = (l),					\
	 (snapshotdata).whenTaken = (w))

/* This macro encodes the knowledge of which snapshots are MVCC-safe */
#define IsMVCCSnapshot(snapshot)  \
	((snapshot)->snapshot_type == SNAPSHOT_MVCC || \
	 (snapshot)->snapshot_type == SNAPSHOT_HISTORIC_MVCC)

static inline bool
OldSnapshotThresholdActive(void)
{
	return old_snapshot_threshold >= 0;
}

extern PGDLLIMPORT Snapshot GetTransactionSnapshot(void);
extern PGDLLIMPORT Snapshot GetLatestSnapshot(void);
extern PGDLLIMPORT void SnapshotSetCommandId(CommandId curcid);
extern PGDLLIMPORT Snapshot GetOldestSnapshot(void);

extern PGDLLIMPORT Snapshot GetCatalogSnapshot(Oid relid);
extern PGDLLIMPORT Snapshot GetNonHistoricCatalogSnapshot(Oid relid);
extern PGDLLIMPORT void InvalidateCatalogSnapshot(void);
extern PGDLLIMPORT void InvalidateCatalogSnapshotConditionally(void);

extern PGDLLIMPORT void PushActiveSnapshot(Snapshot snapshot);
extern PGDLLIMPORT void PushActiveSnapshotWithLevel(Snapshot snapshot, int snap_level);
extern PGDLLIMPORT void PushCopiedSnapshot(Snapshot snapshot);
extern PGDLLIMPORT void UpdateActiveSnapshotCommandId(void);
extern PGDLLIMPORT void PopActiveSnapshot(void);
extern PGDLLIMPORT Snapshot GetActiveSnapshot(void);
extern PGDLLIMPORT bool ActiveSnapshotSet(void);

extern PGDLLIMPORT Snapshot RegisterSnapshot(Snapshot snapshot);
extern PGDLLIMPORT void UnregisterSnapshot(Snapshot snapshot);
extern PGDLLIMPORT Snapshot RegisterSnapshotOnOwner(Snapshot snapshot, ResourceOwner owner);
extern PGDLLIMPORT void UnregisterSnapshotFromOwner(Snapshot snapshot, ResourceOwner owner);

extern PGDLLIMPORT void AtSubCommit_Snapshot(int level);
extern PGDLLIMPORT void AtSubAbort_Snapshot(int level);
extern PGDLLIMPORT void AtEOXact_Snapshot(bool isCommit, bool resetXmin);

extern PGDLLIMPORT void ImportSnapshot(const char *idstr);
extern PGDLLIMPORT bool XactHasExportedSnapshots(void);
extern PGDLLIMPORT void DeleteAllExportedSnapshotFiles(void);
extern PGDLLIMPORT void WaitForOlderSnapshots(TransactionId limitXmin, bool progress);
extern PGDLLIMPORT bool ThereAreNoPriorRegisteredSnapshots(void);
extern PGDLLIMPORT bool HaveRegisteredOrActiveSnapshot(void);
extern PGDLLIMPORT bool TransactionIdLimitedForOldSnapshots(TransactionId recentXmin,
												Relation relation,
												TransactionId *limit_xid,
												TimestampTz *limit_ts);
extern PGDLLIMPORT void SetOldSnapshotThresholdTimestamp(TimestampTz ts, TransactionId xlimit);
extern PGDLLIMPORT void MaintainOldSnapshotTimeMapping(TimestampTz whenTaken,
										   TransactionId xmin);

extern PGDLLIMPORT char *ExportSnapshot(Snapshot snapshot);

/*
 * These live in procarray.c because they're intimately linked to the
 * procarray contents, but thematically they better fit into snapmgr.h.
 */
typedef struct GlobalVisState GlobalVisState;
extern PGDLLIMPORT GlobalVisState *GlobalVisTestFor(Relation rel);
extern PGDLLIMPORT bool GlobalVisTestIsRemovableXid(GlobalVisState *state, TransactionId xid);
extern PGDLLIMPORT bool GlobalVisTestIsRemovableFullXid(GlobalVisState *state, FullTransactionId fxid);
extern PGDLLIMPORT FullTransactionId GlobalVisTestNonRemovableFullHorizon(GlobalVisState *state);
extern PGDLLIMPORT TransactionId GlobalVisTestNonRemovableHorizon(GlobalVisState *state);
extern PGDLLIMPORT bool GlobalVisCheckRemovableXid(Relation rel, TransactionId xid);
extern PGDLLIMPORT bool GlobalVisCheckRemovableFullXid(Relation rel, FullTransactionId fxid);

/*
 * Utility functions for implementing visibility routines in table AMs.
 */
extern PGDLLIMPORT bool XidInMVCCSnapshot(TransactionId xid, Snapshot snapshot);

/* Support for catalog timetravel for logical decoding */
struct HTAB;
extern PGDLLIMPORT struct HTAB *HistoricSnapshotGetTupleCids(void);
extern PGDLLIMPORT void SetupHistoricSnapshot(Snapshot snapshot_now, struct HTAB *tuplecids);
extern PGDLLIMPORT void TeardownHistoricSnapshot(bool is_error);
extern PGDLLIMPORT bool HistoricSnapshotActive(void);

extern PGDLLIMPORT Size EstimateSnapshotSpace(Snapshot snapshot);
extern PGDLLIMPORT void SerializeSnapshot(Snapshot snapshot, char *start_address);
extern PGDLLIMPORT Snapshot RestoreSnapshot(char *start_address);
extern PGDLLIMPORT void RestoreTransactionSnapshot(Snapshot snapshot, void *source_pgproc);

#endif							/* SNAPMGR_H */
