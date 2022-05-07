/*-------------------------------------------------------------------------
 *
 * resowner_private.h
 *	  POSTGRES resource owner private definitions.
 *
 * See utils/resowner/README for more info.
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/utils/resowner_private.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef RESOWNER_PRIVATE_H
#define RESOWNER_PRIVATE_H

#include "storage/dsm.h"
#include "storage/fd.h"
#include "storage/lock.h"
#include "utils/catcache.h"
#include "utils/plancache.h"
#include "utils/resowner.h"
#include "utils/snapshot.h"


/* support for buffer refcount management */
extern PGDLLIMPORT void ResourceOwnerEnlargeBuffers(ResourceOwner owner);
extern PGDLLIMPORT void ResourceOwnerRememberBuffer(ResourceOwner owner, Buffer buffer);
extern PGDLLIMPORT void ResourceOwnerForgetBuffer(ResourceOwner owner, Buffer buffer);

/* support for local lock management */
extern PGDLLIMPORT void ResourceOwnerRememberLock(ResourceOwner owner, LOCALLOCK *locallock);
extern PGDLLIMPORT void ResourceOwnerForgetLock(ResourceOwner owner, LOCALLOCK *locallock);

/* support for catcache refcount management */
extern PGDLLIMPORT void ResourceOwnerEnlargeCatCacheRefs(ResourceOwner owner);
extern PGDLLIMPORT void ResourceOwnerRememberCatCacheRef(ResourceOwner owner,
											 HeapTuple tuple);
extern PGDLLIMPORT void ResourceOwnerForgetCatCacheRef(ResourceOwner owner,
										   HeapTuple tuple);
extern PGDLLIMPORT void ResourceOwnerEnlargeCatCacheListRefs(ResourceOwner owner);
extern PGDLLIMPORT void ResourceOwnerRememberCatCacheListRef(ResourceOwner owner,
												 CatCList *list);
extern PGDLLIMPORT void ResourceOwnerForgetCatCacheListRef(ResourceOwner owner,
											   CatCList *list);

/* support for relcache refcount management */
extern PGDLLIMPORT void ResourceOwnerEnlargeRelationRefs(ResourceOwner owner);
extern PGDLLIMPORT void ResourceOwnerRememberRelationRef(ResourceOwner owner,
											 Relation rel);
extern PGDLLIMPORT void ResourceOwnerForgetRelationRef(ResourceOwner owner,
										   Relation rel);

/* support for plancache refcount management */
extern PGDLLIMPORT void ResourceOwnerEnlargePlanCacheRefs(ResourceOwner owner);
extern PGDLLIMPORT void ResourceOwnerRememberPlanCacheRef(ResourceOwner owner,
											  CachedPlan *plan);
extern PGDLLIMPORT void ResourceOwnerForgetPlanCacheRef(ResourceOwner owner,
											CachedPlan *plan);

/* support for tupledesc refcount management */
extern PGDLLIMPORT void ResourceOwnerEnlargeTupleDescs(ResourceOwner owner);
extern PGDLLIMPORT void ResourceOwnerRememberTupleDesc(ResourceOwner owner,
										   TupleDesc tupdesc);
extern PGDLLIMPORT void ResourceOwnerForgetTupleDesc(ResourceOwner owner,
										 TupleDesc tupdesc);

/* support for snapshot refcount management */
extern PGDLLIMPORT void ResourceOwnerEnlargeSnapshots(ResourceOwner owner);
extern PGDLLIMPORT void ResourceOwnerRememberSnapshot(ResourceOwner owner,
										  Snapshot snapshot);
extern PGDLLIMPORT void ResourceOwnerForgetSnapshot(ResourceOwner owner,
										Snapshot snapshot);

/* support for temporary file management */
extern PGDLLIMPORT void ResourceOwnerEnlargeFiles(ResourceOwner owner);
extern PGDLLIMPORT void ResourceOwnerRememberFile(ResourceOwner owner,
									  File file);
extern PGDLLIMPORT void ResourceOwnerForgetFile(ResourceOwner owner,
									File file);

/* support for dynamic shared memory management */
extern PGDLLIMPORT void ResourceOwnerEnlargeDSMs(ResourceOwner owner);
extern PGDLLIMPORT void ResourceOwnerRememberDSM(ResourceOwner owner,
									 dsm_segment *);
extern PGDLLIMPORT void ResourceOwnerForgetDSM(ResourceOwner owner,
								   dsm_segment *);

/* support for JITContext management */
extern PGDLLIMPORT void ResourceOwnerEnlargeJIT(ResourceOwner owner);
extern PGDLLIMPORT void ResourceOwnerRememberJIT(ResourceOwner owner,
									 Datum handle);
extern PGDLLIMPORT void ResourceOwnerForgetJIT(ResourceOwner owner,
								   Datum handle);

/* support for cryptohash context management */
extern PGDLLIMPORT void ResourceOwnerEnlargeCryptoHash(ResourceOwner owner);
extern PGDLLIMPORT void ResourceOwnerRememberCryptoHash(ResourceOwner owner,
											Datum handle);
extern PGDLLIMPORT void ResourceOwnerForgetCryptoHash(ResourceOwner owner,
										  Datum handle);

/* support for HMAC context management */
extern PGDLLIMPORT void ResourceOwnerEnlargeHMAC(ResourceOwner owner);
extern PGDLLIMPORT void ResourceOwnerRememberHMAC(ResourceOwner owner,
									  Datum handle);
extern PGDLLIMPORT void ResourceOwnerForgetHMAC(ResourceOwner owner,
									Datum handle);

#endif							/* RESOWNER_PRIVATE_H */
