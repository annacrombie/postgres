/*-------------------------------------------------------------------------
 *
 * catalog.h
 *	  prototypes for functions in backend/catalog/catalog.c
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/catalog.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef CATALOG_H
#define CATALOG_H

#include "catalog/pg_class.h"
#include "utils/relcache.h"


extern PGDLLIMPORT bool IsSystemRelation(Relation relation);
extern PGDLLIMPORT bool IsToastRelation(Relation relation);
extern PGDLLIMPORT bool IsCatalogRelation(Relation relation);

extern PGDLLIMPORT bool IsSystemClass(Oid relid, Form_pg_class reltuple);
extern PGDLLIMPORT bool IsToastClass(Form_pg_class reltuple);

extern PGDLLIMPORT bool IsCatalogRelationOid(Oid relid);

extern PGDLLIMPORT bool IsCatalogNamespace(Oid namespaceId);
extern PGDLLIMPORT bool IsToastNamespace(Oid namespaceId);

extern PGDLLIMPORT bool IsReservedName(const char *name);

extern PGDLLIMPORT bool IsSharedRelation(Oid relationId);

extern PGDLLIMPORT bool IsPinnedObject(Oid classId, Oid objectId);

extern PGDLLIMPORT Oid	GetNewOidWithIndex(Relation relation, Oid indexId,
							   AttrNumber oidcolumn);
extern PGDLLIMPORT Oid	GetNewRelFileNode(Oid reltablespace, Relation pg_class,
							  char relpersistence);

#endif							/* CATALOG_H */
