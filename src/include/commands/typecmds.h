/*-------------------------------------------------------------------------
 *
 * typecmds.h
 *	  prototypes for typecmds.c.
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/commands/typecmds.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef TYPECMDS_H
#define TYPECMDS_H

#include "access/htup.h"
#include "catalog/dependency.h"
#include "parser/parse_node.h"


#define DEFAULT_TYPDELIM		','

extern PGDLLIMPORT ObjectAddress DefineType(ParseState *pstate, List *names, List *parameters);
extern PGDLLIMPORT void RemoveTypeById(Oid typeOid);
extern PGDLLIMPORT ObjectAddress DefineDomain(CreateDomainStmt *stmt);
extern PGDLLIMPORT ObjectAddress DefineEnum(CreateEnumStmt *stmt);
extern PGDLLIMPORT ObjectAddress DefineRange(ParseState *pstate, CreateRangeStmt *stmt);
extern PGDLLIMPORT ObjectAddress AlterEnum(AlterEnumStmt *stmt);
extern PGDLLIMPORT ObjectAddress DefineCompositeType(RangeVar *typevar, List *coldeflist);
extern PGDLLIMPORT Oid	AssignTypeArrayOid(void);
extern PGDLLIMPORT Oid	AssignTypeMultirangeOid(void);
extern PGDLLIMPORT Oid	AssignTypeMultirangeArrayOid(void);

extern PGDLLIMPORT ObjectAddress AlterDomainDefault(List *names, Node *defaultRaw);
extern PGDLLIMPORT ObjectAddress AlterDomainNotNull(List *names, bool notNull);
extern PGDLLIMPORT ObjectAddress AlterDomainAddConstraint(List *names, Node *constr,
											  ObjectAddress *constrAddr);
extern PGDLLIMPORT ObjectAddress AlterDomainValidateConstraint(List *names, const char *constrName);
extern PGDLLIMPORT ObjectAddress AlterDomainDropConstraint(List *names, const char *constrName,
											   DropBehavior behavior, bool missing_ok);

extern PGDLLIMPORT void checkDomainOwner(HeapTuple tup);

extern PGDLLIMPORT ObjectAddress RenameType(RenameStmt *stmt);

extern PGDLLIMPORT ObjectAddress AlterTypeOwner(List *names, Oid newOwnerId, ObjectType objecttype);
extern PGDLLIMPORT void AlterTypeOwner_oid(Oid typeOid, Oid newOwnerId, bool hasDependEntry);
extern PGDLLIMPORT void AlterTypeOwnerInternal(Oid typeOid, Oid newOwnerId);

extern PGDLLIMPORT ObjectAddress AlterTypeNamespace(List *names, const char *newschema,
										ObjectType objecttype, Oid *oldschema);
extern PGDLLIMPORT Oid	AlterTypeNamespace_oid(Oid typeOid, Oid nspOid, ObjectAddresses *objsMoved);
extern PGDLLIMPORT Oid	AlterTypeNamespaceInternal(Oid typeOid, Oid nspOid,
									   bool isImplicitArray,
									   bool errorOnTableType,
									   ObjectAddresses *objsMoved);

extern PGDLLIMPORT ObjectAddress AlterType(AlterTypeStmt *stmt);

#endif							/* TYPECMDS_H */
