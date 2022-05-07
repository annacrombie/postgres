/*-------------------------------------------------------------------------
 *
 * inval.h
 *	  POSTGRES cache invalidation dispatcher definitions.
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/utils/inval.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef INVAL_H
#define INVAL_H

#include "access/htup.h"
#include "storage/relfilenode.h"
#include "utils/relcache.h"

extern PGDLLIMPORT int debug_discard_caches;

typedef void (*SyscacheCallbackFunction) (Datum arg, int cacheid, uint32 hashvalue);
typedef void (*RelcacheCallbackFunction) (Datum arg, Oid relid);


extern PGDLLIMPORT void AcceptInvalidationMessages(void);

extern PGDLLIMPORT void AtEOXact_Inval(bool isCommit);

extern PGDLLIMPORT void AtEOSubXact_Inval(bool isCommit);

extern PGDLLIMPORT void PostPrepare_Inval(void);

extern PGDLLIMPORT void CommandEndInvalidationMessages(void);

extern PGDLLIMPORT void CacheInvalidateHeapTuple(Relation relation,
									 HeapTuple tuple,
									 HeapTuple newtuple);

extern PGDLLIMPORT void CacheInvalidateCatalog(Oid catalogId);

extern PGDLLIMPORT void CacheInvalidateRelcache(Relation relation);

extern PGDLLIMPORT void CacheInvalidateRelcacheAll(void);

extern PGDLLIMPORT void CacheInvalidateRelcacheByTuple(HeapTuple classTuple);

extern PGDLLIMPORT void CacheInvalidateRelcacheByRelid(Oid relid);

extern PGDLLIMPORT void CacheInvalidateSmgr(RelFileNodeBackend rnode);

extern PGDLLIMPORT void CacheInvalidateRelmap(Oid databaseId);

extern PGDLLIMPORT void CacheRegisterSyscacheCallback(int cacheid,
										  SyscacheCallbackFunction func,
										  Datum arg);

extern PGDLLIMPORT void CacheRegisterRelcacheCallback(RelcacheCallbackFunction func,
										  Datum arg);

extern PGDLLIMPORT void CallSyscacheCallbacks(int cacheid, uint32 hashvalue);

extern PGDLLIMPORT void InvalidateSystemCaches(void);
extern PGDLLIMPORT void InvalidateSystemCachesExtended(bool debug_discard);

extern PGDLLIMPORT void LogLogicalInvalidations(void);
#endif							/* INVAL_H */
