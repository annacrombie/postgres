/*-------------------------------------------------------------------------
 *
 * lmgr.h
 *	  POSTGRES lock manager definitions.
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/storage/lmgr.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef LMGR_H
#define LMGR_H

#include "lib/stringinfo.h"
#include "storage/itemptr.h"
#include "storage/lock.h"
#include "utils/rel.h"


/* XactLockTableWait operations */
typedef enum XLTW_Oper
{
	XLTW_None,
	XLTW_Update,
	XLTW_Delete,
	XLTW_Lock,
	XLTW_LockUpdated,
	XLTW_InsertIndex,
	XLTW_InsertIndexUnique,
	XLTW_FetchUpdated,
	XLTW_RecheckExclusionConstr
} XLTW_Oper;

extern PGDLLIMPORT void RelationInitLockInfo(Relation relation);

/* Lock a relation */
extern PGDLLIMPORT void LockRelationOid(Oid relid, LOCKMODE lockmode);
extern PGDLLIMPORT void LockRelationId(LockRelId *relid, LOCKMODE lockmode);
extern PGDLLIMPORT bool ConditionalLockRelationOid(Oid relid, LOCKMODE lockmode);
extern PGDLLIMPORT void UnlockRelationId(LockRelId *relid, LOCKMODE lockmode);
extern PGDLLIMPORT void UnlockRelationOid(Oid relid, LOCKMODE lockmode);

extern PGDLLIMPORT void LockRelation(Relation relation, LOCKMODE lockmode);
extern PGDLLIMPORT bool ConditionalLockRelation(Relation relation, LOCKMODE lockmode);
extern PGDLLIMPORT void UnlockRelation(Relation relation, LOCKMODE lockmode);
extern PGDLLIMPORT bool CheckRelationLockedByMe(Relation relation, LOCKMODE lockmode,
									bool orstronger);
extern PGDLLIMPORT bool LockHasWaitersRelation(Relation relation, LOCKMODE lockmode);

extern PGDLLIMPORT void LockRelationIdForSession(LockRelId *relid, LOCKMODE lockmode);
extern PGDLLIMPORT void UnlockRelationIdForSession(LockRelId *relid, LOCKMODE lockmode);

/* Lock a relation for extension */
extern PGDLLIMPORT void LockRelationForExtension(Relation relation, LOCKMODE lockmode);
extern PGDLLIMPORT void UnlockRelationForExtension(Relation relation, LOCKMODE lockmode);
extern PGDLLIMPORT bool ConditionalLockRelationForExtension(Relation relation,
												LOCKMODE lockmode);
extern PGDLLIMPORT int	RelationExtensionLockWaiterCount(Relation relation);

/* Lock to recompute pg_database.datfrozenxid in the current database */
extern PGDLLIMPORT void LockDatabaseFrozenIds(LOCKMODE lockmode);

/* Lock a page (currently only used within indexes) */
extern PGDLLIMPORT void LockPage(Relation relation, BlockNumber blkno, LOCKMODE lockmode);
extern PGDLLIMPORT bool ConditionalLockPage(Relation relation, BlockNumber blkno, LOCKMODE lockmode);
extern PGDLLIMPORT void UnlockPage(Relation relation, BlockNumber blkno, LOCKMODE lockmode);

/* Lock a tuple (see heap_lock_tuple before assuming you understand this) */
extern PGDLLIMPORT void LockTuple(Relation relation, ItemPointer tid, LOCKMODE lockmode);
extern PGDLLIMPORT bool ConditionalLockTuple(Relation relation, ItemPointer tid,
								 LOCKMODE lockmode);
extern PGDLLIMPORT void UnlockTuple(Relation relation, ItemPointer tid, LOCKMODE lockmode);

/* Lock an XID (used to wait for a transaction to finish) */
extern PGDLLIMPORT void XactLockTableInsert(TransactionId xid);
extern PGDLLIMPORT void XactLockTableDelete(TransactionId xid);
extern PGDLLIMPORT void XactLockTableWait(TransactionId xid, Relation rel,
							  ItemPointer ctid, XLTW_Oper oper);
extern PGDLLIMPORT bool ConditionalXactLockTableWait(TransactionId xid);

/* Lock VXIDs, specified by conflicting locktags */
extern PGDLLIMPORT void WaitForLockers(LOCKTAG heaplocktag, LOCKMODE lockmode, bool progress);
extern PGDLLIMPORT void WaitForLockersMultiple(List *locktags, LOCKMODE lockmode, bool progress);

/* Lock an XID for tuple insertion (used to wait for an insertion to finish) */
extern PGDLLIMPORT uint32 SpeculativeInsertionLockAcquire(TransactionId xid);
extern PGDLLIMPORT void SpeculativeInsertionLockRelease(TransactionId xid);
extern PGDLLIMPORT void SpeculativeInsertionWait(TransactionId xid, uint32 token);

/* Lock a general object (other than a relation) of the current database */
extern PGDLLIMPORT void LockDatabaseObject(Oid classid, Oid objid, uint16 objsubid,
							   LOCKMODE lockmode);
extern PGDLLIMPORT void UnlockDatabaseObject(Oid classid, Oid objid, uint16 objsubid,
								 LOCKMODE lockmode);

/* Lock a shared-across-databases object (other than a relation) */
extern PGDLLIMPORT void LockSharedObject(Oid classid, Oid objid, uint16 objsubid,
							 LOCKMODE lockmode);
extern PGDLLIMPORT void UnlockSharedObject(Oid classid, Oid objid, uint16 objsubid,
							   LOCKMODE lockmode);

extern PGDLLIMPORT void LockSharedObjectForSession(Oid classid, Oid objid, uint16 objsubid,
									   LOCKMODE lockmode);
extern PGDLLIMPORT void UnlockSharedObjectForSession(Oid classid, Oid objid, uint16 objsubid,
										 LOCKMODE lockmode);

/* Describe a locktag for error messages */
extern PGDLLIMPORT void DescribeLockTag(StringInfo buf, const LOCKTAG *tag);

extern PGDLLIMPORT const char *GetLockNameFromTagType(uint16 locktag_type);

#endif							/* LMGR_H */
