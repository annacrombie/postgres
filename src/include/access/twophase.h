/*-------------------------------------------------------------------------
 *
 * twophase.h
 *	  Two-phase-commit related declarations.
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/access/twophase.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef TWOPHASE_H
#define TWOPHASE_H

#include "access/xact.h"
#include "access/xlogdefs.h"
#include "datatype/timestamp.h"
#include "storage/lock.h"

/*
 * GlobalTransactionData is defined in twophase.c; other places have no
 * business knowing the internal definition.
 */
typedef struct GlobalTransactionData *GlobalTransaction;

/* GUC variable */
extern PGDLLIMPORT int max_prepared_xacts;

extern PGDLLIMPORT Size TwoPhaseShmemSize(void);
extern PGDLLIMPORT void TwoPhaseShmemInit(void);

extern PGDLLIMPORT void AtAbort_Twophase(void);
extern PGDLLIMPORT void PostPrepare_Twophase(void);

extern PGDLLIMPORT TransactionId TwoPhaseGetXidByVirtualXID(VirtualTransactionId vxid,
												bool *have_more);
extern PGDLLIMPORT PGPROC *TwoPhaseGetDummyProc(TransactionId xid, bool lock_held);
extern PGDLLIMPORT BackendId TwoPhaseGetDummyBackendId(TransactionId xid, bool lock_held);

extern PGDLLIMPORT GlobalTransaction MarkAsPreparing(TransactionId xid, const char *gid,
										 TimestampTz prepared_at,
										 Oid owner, Oid databaseid);

extern PGDLLIMPORT void StartPrepare(GlobalTransaction gxact);
extern PGDLLIMPORT void EndPrepare(GlobalTransaction gxact);
extern PGDLLIMPORT bool StandbyTransactionIdIsPrepared(TransactionId xid);

extern PGDLLIMPORT TransactionId PrescanPreparedTransactions(TransactionId **xids_p,
												 int *nxids_p);
extern PGDLLIMPORT void StandbyRecoverPreparedTransactions(void);
extern PGDLLIMPORT void RecoverPreparedTransactions(void);

extern PGDLLIMPORT void CheckPointTwoPhase(XLogRecPtr redo_horizon);

extern PGDLLIMPORT void FinishPreparedTransaction(const char *gid, bool isCommit);

extern PGDLLIMPORT void PrepareRedoAdd(char *buf, XLogRecPtr start_lsn,
						   XLogRecPtr end_lsn, RepOriginId origin_id);
extern PGDLLIMPORT void PrepareRedoRemove(TransactionId xid, bool giveWarning);
extern PGDLLIMPORT void restoreTwoPhaseData(void);
extern PGDLLIMPORT bool LookupGXact(const char *gid, XLogRecPtr prepare_at_lsn,
						TimestampTz origin_prepare_timestamp);
#endif							/* TWOPHASE_H */
