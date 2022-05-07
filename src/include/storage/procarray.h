/*-------------------------------------------------------------------------
 *
 * procarray.h
 *	  POSTGRES process array definitions.
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/storage/procarray.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PROCARRAY_H
#define PROCARRAY_H

#include "storage/lock.h"
#include "storage/standby.h"
#include "utils/relcache.h"
#include "utils/snapshot.h"


extern PGDLLIMPORT Size ProcArrayShmemSize(void);
extern PGDLLIMPORT void CreateSharedProcArray(void);
extern PGDLLIMPORT void ProcArrayAdd(PGPROC *proc);
extern PGDLLIMPORT void ProcArrayRemove(PGPROC *proc, TransactionId latestXid);

extern PGDLLIMPORT void ProcArrayEndTransaction(PGPROC *proc, TransactionId latestXid);
extern PGDLLIMPORT void ProcArrayClearTransaction(PGPROC *proc);

extern PGDLLIMPORT void ProcArrayInitRecovery(TransactionId initializedUptoXID);
extern PGDLLIMPORT void ProcArrayApplyRecoveryInfo(RunningTransactions running);
extern PGDLLIMPORT void ProcArrayApplyXidAssignment(TransactionId topxid,
										int nsubxids, TransactionId *subxids);

extern PGDLLIMPORT void RecordKnownAssignedTransactionIds(TransactionId xid);
extern PGDLLIMPORT void ExpireTreeKnownAssignedTransactionIds(TransactionId xid,
												  int nsubxids, TransactionId *subxids,
												  TransactionId max_xid);
extern PGDLLIMPORT void ExpireAllKnownAssignedTransactionIds(void);
extern PGDLLIMPORT void ExpireOldKnownAssignedTransactionIds(TransactionId xid);

extern PGDLLIMPORT int	GetMaxSnapshotXidCount(void);
extern PGDLLIMPORT int	GetMaxSnapshotSubxidCount(void);

extern PGDLLIMPORT Snapshot GetSnapshotData(Snapshot snapshot);

extern PGDLLIMPORT bool ProcArrayInstallImportedXmin(TransactionId xmin,
										 VirtualTransactionId *sourcevxid);
extern PGDLLIMPORT bool ProcArrayInstallRestoredXmin(TransactionId xmin, PGPROC *proc);

extern PGDLLIMPORT RunningTransactions GetRunningTransactionData(void);

extern PGDLLIMPORT bool TransactionIdIsInProgress(TransactionId xid);
extern PGDLLIMPORT bool TransactionIdIsActive(TransactionId xid);
extern PGDLLIMPORT TransactionId GetOldestNonRemovableTransactionId(Relation rel);
extern PGDLLIMPORT TransactionId GetOldestTransactionIdConsideredRunning(void);
extern PGDLLIMPORT TransactionId GetOldestActiveTransactionId(void);
extern PGDLLIMPORT TransactionId GetOldestSafeDecodingTransactionId(bool catalogOnly);
extern PGDLLIMPORT void GetReplicationHorizons(TransactionId *slot_xmin, TransactionId *catalog_xmin);

extern PGDLLIMPORT VirtualTransactionId *GetVirtualXIDsDelayingChkpt(int *nvxids, int type);
extern PGDLLIMPORT bool HaveVirtualXIDsDelayingChkpt(VirtualTransactionId *vxids,
										 int nvxids, int type);

extern PGDLLIMPORT PGPROC *BackendPidGetProc(int pid);
extern PGDLLIMPORT PGPROC *BackendPidGetProcWithLock(int pid);
extern PGDLLIMPORT int	BackendXidGetPid(TransactionId xid);
extern PGDLLIMPORT bool IsBackendPid(int pid);

extern PGDLLIMPORT VirtualTransactionId *GetCurrentVirtualXIDs(TransactionId limitXmin,
												   bool excludeXmin0, bool allDbs, int excludeVacuum,
												   int *nvxids);
extern PGDLLIMPORT VirtualTransactionId *GetConflictingVirtualXIDs(TransactionId limitXmin, Oid dbOid);
extern PGDLLIMPORT pid_t CancelVirtualTransaction(VirtualTransactionId vxid, ProcSignalReason sigmode);
extern PGDLLIMPORT pid_t SignalVirtualTransaction(VirtualTransactionId vxid, ProcSignalReason sigmode,
									  bool conflictPending);

extern PGDLLIMPORT bool MinimumActiveBackends(int min);
extern PGDLLIMPORT int	CountDBBackends(Oid databaseid);
extern PGDLLIMPORT int	CountDBConnections(Oid databaseid);
extern PGDLLIMPORT void CancelDBBackends(Oid databaseid, ProcSignalReason sigmode, bool conflictPending);
extern PGDLLIMPORT int	CountUserBackends(Oid roleid);
extern PGDLLIMPORT bool CountOtherDBBackends(Oid databaseId,
								 int *nbackends, int *nprepared);
extern PGDLLIMPORT void TerminateOtherDBBackends(Oid databaseId);

extern PGDLLIMPORT void XidCacheRemoveRunningXids(TransactionId xid,
									  int nxids, const TransactionId *xids,
									  TransactionId latestXid);

extern PGDLLIMPORT void ProcArraySetReplicationSlotXmin(TransactionId xmin,
											TransactionId catalog_xmin, bool already_locked);

extern PGDLLIMPORT void ProcArrayGetReplicationSlotXmin(TransactionId *xmin,
											TransactionId *catalog_xmin);

#endif							/* PROCARRAY_H */
