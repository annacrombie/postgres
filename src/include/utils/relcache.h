/*-------------------------------------------------------------------------
 *
 * relcache.h
 *	  Relation descriptor cache definitions.
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/utils/relcache.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef RELCACHE_H
#define RELCACHE_H

#include "access/tupdesc.h"
#include "nodes/bitmapset.h"


/*
 * Name of relcache init file(s), used to speed up backend startup
 */
#define RELCACHE_INIT_FILENAME	"pg_internal.init"

typedef struct RelationData *Relation;

/* ----------------
 *		RelationPtr is used in the executor to support index scans
 *		where we have to keep track of several index relations in an
 *		array.  -cim 9/10/89
 * ----------------
 */
typedef Relation *RelationPtr;

/*
 * Routines to open (lookup) and close a relcache entry
 */
extern PGDLLIMPORT Relation RelationIdGetRelation(Oid relationId);
extern PGDLLIMPORT void RelationClose(Relation relation);

/*
 * Routines to compute/retrieve additional cached information
 */
extern PGDLLIMPORT List *RelationGetFKeyList(Relation relation);
extern PGDLLIMPORT List *RelationGetIndexList(Relation relation);
extern PGDLLIMPORT List *RelationGetStatExtList(Relation relation);
extern PGDLLIMPORT Oid	RelationGetPrimaryKeyIndex(Relation relation);
extern PGDLLIMPORT Oid	RelationGetReplicaIndex(Relation relation);
extern PGDLLIMPORT List *RelationGetIndexExpressions(Relation relation);
extern PGDLLIMPORT List *RelationGetDummyIndexExpressions(Relation relation);
extern PGDLLIMPORT List *RelationGetIndexPredicate(Relation relation);
extern PGDLLIMPORT Datum *RelationGetIndexRawAttOptions(Relation relation);
extern PGDLLIMPORT bytea **RelationGetIndexAttOptions(Relation relation, bool copy);

typedef enum IndexAttrBitmapKind
{
	INDEX_ATTR_BITMAP_KEY,
	INDEX_ATTR_BITMAP_PRIMARY_KEY,
	INDEX_ATTR_BITMAP_IDENTITY_KEY,
	INDEX_ATTR_BITMAP_HOT_BLOCKING
} IndexAttrBitmapKind;

extern PGDLLIMPORT Bitmapset *RelationGetIndexAttrBitmap(Relation relation,
											 IndexAttrBitmapKind attrKind);

extern PGDLLIMPORT Bitmapset *RelationGetIdentityKeyBitmap(Relation relation);

extern PGDLLIMPORT void RelationGetExclusionInfo(Relation indexRelation,
									 Oid **operators,
									 Oid **procs,
									 uint16 **strategies);

extern PGDLLIMPORT void RelationInitIndexAccessInfo(Relation relation);

/* caller must include pg_publication.h */
struct PublicationDesc;
extern PGDLLIMPORT void RelationBuildPublicationDesc(Relation relation,
										 struct PublicationDesc *pubdesc);

extern PGDLLIMPORT void RelationInitTableAccessMethod(Relation relation);

/*
 * Routines to support ereport() reports of relation-related errors
 */
extern PGDLLIMPORT int	errtable(Relation rel);
extern PGDLLIMPORT int	errtablecol(Relation rel, int attnum);
extern PGDLLIMPORT int	errtablecolname(Relation rel, const char *colname);
extern PGDLLIMPORT int	errtableconstraint(Relation rel, const char *conname);

/*
 * Routines for backend startup
 */
extern PGDLLIMPORT void RelationCacheInitialize(void);
extern PGDLLIMPORT void RelationCacheInitializePhase2(void);
extern PGDLLIMPORT void RelationCacheInitializePhase3(void);

/*
 * Routine to create a relcache entry for an about-to-be-created relation
 */
extern PGDLLIMPORT Relation RelationBuildLocalRelation(const char *relname,
										   Oid relnamespace,
										   TupleDesc tupDesc,
										   Oid relid,
										   Oid accessmtd,
										   Oid relfilenode,
										   Oid reltablespace,
										   bool shared_relation,
										   bool mapped_relation,
										   char relpersistence,
										   char relkind);

/*
 * Routines to manage assignment of new relfilenode to a relation
 */
extern PGDLLIMPORT void RelationSetNewRelfilenode(Relation relation, char persistence);
extern PGDLLIMPORT void RelationAssumeNewRelfilenode(Relation relation);

/*
 * Routines for flushing/rebuilding relcache entries in various scenarios
 */
extern PGDLLIMPORT void RelationForgetRelation(Oid rid);

extern PGDLLIMPORT void RelationCacheInvalidateEntry(Oid relationId);

extern PGDLLIMPORT void RelationCacheInvalidate(bool debug_discard);

extern PGDLLIMPORT void RelationCloseSmgrByOid(Oid relationId);

#ifdef USE_ASSERT_CHECKING
extern PGDLLIMPORT void AssertPendingSyncs_RelationCache(void);
#else
#define AssertPendingSyncs_RelationCache() do {} while (0)
#endif
extern PGDLLIMPORT void AtEOXact_RelationCache(bool isCommit);
extern PGDLLIMPORT void AtEOSubXact_RelationCache(bool isCommit, SubTransactionId mySubid,
									  SubTransactionId parentSubid);

/*
 * Routines to help manage rebuilding of relcache init files
 */
extern PGDLLIMPORT bool RelationIdIsInInitFile(Oid relationId);
extern PGDLLIMPORT void RelationCacheInitFilePreInvalidate(void);
extern PGDLLIMPORT void RelationCacheInitFilePostInvalidate(void);
extern PGDLLIMPORT void RelationCacheInitFileRemove(void);

/* should be used only by relcache.c and catcache.c */
extern PGDLLIMPORT bool criticalRelcachesBuilt;

/* should be used only by relcache.c and postinit.c */
extern PGDLLIMPORT bool criticalSharedRelcachesBuilt;

#endif							/* RELCACHE_H */
