/*-------------------------------------------------------------------------
 *
 * policy.h
 *	  prototypes for policy.c.
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/commands/policy.h
 *
 *-------------------------------------------------------------------------
 */

#ifndef POLICY_H
#define POLICY_H

#include "catalog/objectaddress.h"
#include "nodes/parsenodes.h"
#include "utils/relcache.h"

extern PGDLLIMPORT void RelationBuildRowSecurity(Relation relation);

extern PGDLLIMPORT void RemovePolicyById(Oid policy_id);

extern PGDLLIMPORT bool RemoveRoleFromObjectPolicy(Oid roleid, Oid classid, Oid objid);

extern PGDLLIMPORT ObjectAddress CreatePolicy(CreatePolicyStmt *stmt);
extern PGDLLIMPORT ObjectAddress AlterPolicy(AlterPolicyStmt *stmt);

extern PGDLLIMPORT Oid	get_relation_policy_oid(Oid relid, const char *policy_name,
									bool missing_ok);

extern PGDLLIMPORT ObjectAddress rename_policy(RenameStmt *stmt);

extern PGDLLIMPORT bool relation_has_policies(Relation rel);

#endif							/* POLICY_H */
