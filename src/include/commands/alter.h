/*-------------------------------------------------------------------------
 *
 * alter.h
 *	  prototypes for commands/alter.c
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/commands/alter.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef ALTER_H
#define ALTER_H

#include "catalog/dependency.h"
#include "catalog/objectaddress.h"
#include "nodes/parsenodes.h"
#include "utils/relcache.h"

extern PGDLLIMPORT ObjectAddress ExecRenameStmt(RenameStmt *stmt);

extern PGDLLIMPORT ObjectAddress ExecAlterObjectDependsStmt(AlterObjectDependsStmt *stmt,
												ObjectAddress *refAddress);
extern PGDLLIMPORT ObjectAddress ExecAlterObjectSchemaStmt(AlterObjectSchemaStmt *stmt,
											   ObjectAddress *oldSchemaAddr);
extern PGDLLIMPORT Oid	AlterObjectNamespace_oid(Oid classId, Oid objid, Oid nspOid,
									 ObjectAddresses *objsMoved);

extern PGDLLIMPORT ObjectAddress ExecAlterOwnerStmt(AlterOwnerStmt *stmt);
extern PGDLLIMPORT void AlterObjectOwner_internal(Relation catalog, Oid objectId,
									  Oid new_ownerId);

#endif							/* ALTER_H */
