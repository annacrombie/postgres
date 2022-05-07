/*-------------------------------------------------------------------------
 *
 * storage.h
 *	  prototypes for functions in backend/catalog/storage.c
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/storage.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef STORAGE_H
#define STORAGE_H

#include "storage/block.h"
#include "storage/relfilenode.h"
#include "storage/smgr.h"
#include "utils/relcache.h"

/* GUC variables */
extern PGDLLIMPORT int wal_skip_threshold;

extern PGDLLIMPORT SMgrRelation RelationCreateStorage(RelFileNode rnode,
										  char relpersistence,
										  bool register_delete);
extern PGDLLIMPORT void RelationDropStorage(Relation rel);
extern PGDLLIMPORT void RelationPreserveStorage(RelFileNode rnode, bool atCommit);
extern PGDLLIMPORT void RelationPreTruncate(Relation rel);
extern PGDLLIMPORT void RelationTruncate(Relation rel, BlockNumber nblocks);
extern PGDLLIMPORT void RelationCopyStorage(SMgrRelation src, SMgrRelation dst,
								ForkNumber forkNum, char relpersistence);
extern PGDLLIMPORT bool RelFileNodeSkippingWAL(RelFileNode rnode);
extern PGDLLIMPORT Size EstimatePendingSyncsSpace(void);
extern PGDLLIMPORT void SerializePendingSyncs(Size maxSize, char *startAddress);
extern PGDLLIMPORT void RestorePendingSyncs(char *startAddress);

/*
 * These functions used to be in storage/smgr/smgr.c, which explains the
 * naming
 */
extern PGDLLIMPORT void smgrDoPendingDeletes(bool isCommit);
extern PGDLLIMPORT void smgrDoPendingSyncs(bool isCommit, bool isParallelWorker);
extern PGDLLIMPORT int	smgrGetPendingDeletes(bool forCommit, RelFileNode **ptr);
extern PGDLLIMPORT void AtSubCommit_smgr(void);
extern PGDLLIMPORT void AtSubAbort_smgr(void);
extern PGDLLIMPORT void PostPrepare_smgr(void);

#endif							/* STORAGE_H */
