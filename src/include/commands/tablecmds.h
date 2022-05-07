/*-------------------------------------------------------------------------
 *
 * tablecmds.h
 *	  prototypes for tablecmds.c.
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/commands/tablecmds.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef TABLECMDS_H
#define TABLECMDS_H

#include "access/htup.h"
#include "catalog/dependency.h"
#include "catalog/objectaddress.h"
#include "nodes/parsenodes.h"
#include "storage/lock.h"
#include "utils/relcache.h"

struct AlterTableUtilityContext;	/* avoid including tcop/utility.h here */


extern PGDLLIMPORT ObjectAddress DefineRelation(CreateStmt *stmt, char relkind, Oid ownerId,
									ObjectAddress *typaddress, const char *queryString);

extern PGDLLIMPORT void RemoveRelations(DropStmt *drop);

extern PGDLLIMPORT Oid	AlterTableLookupRelation(AlterTableStmt *stmt, LOCKMODE lockmode);

extern PGDLLIMPORT void AlterTable(AlterTableStmt *stmt, LOCKMODE lockmode,
					   struct AlterTableUtilityContext *context);

extern PGDLLIMPORT LOCKMODE AlterTableGetLockLevel(List *cmds);

extern PGDLLIMPORT void ATExecChangeOwner(Oid relationOid, Oid newOwnerId, bool recursing, LOCKMODE lockmode);

extern PGDLLIMPORT void AlterTableInternal(Oid relid, List *cmds, bool recurse);

extern PGDLLIMPORT Oid	AlterTableMoveAll(AlterTableMoveAllStmt *stmt);

extern PGDLLIMPORT ObjectAddress AlterTableNamespace(AlterObjectSchemaStmt *stmt,
										 Oid *oldschema);

extern PGDLLIMPORT void AlterTableNamespaceInternal(Relation rel, Oid oldNspOid,
										Oid nspOid, ObjectAddresses *objsMoved);

extern PGDLLIMPORT void AlterRelationNamespaceInternal(Relation classRel, Oid relOid,
										   Oid oldNspOid, Oid newNspOid,
										   bool hasDependEntry,
										   ObjectAddresses *objsMoved);

extern PGDLLIMPORT void CheckTableNotInUse(Relation rel, const char *stmt);

extern PGDLLIMPORT void ExecuteTruncate(TruncateStmt *stmt);
extern PGDLLIMPORT void ExecuteTruncateGuts(List *explicit_rels,
								List *relids,
								List *relids_logged,
								DropBehavior behavior,
								bool restart_seqs);

extern PGDLLIMPORT void SetRelationHasSubclass(Oid relationId, bool relhassubclass);

extern PGDLLIMPORT bool CheckRelationTableSpaceMove(Relation rel, Oid newTableSpaceId);
extern PGDLLIMPORT void SetRelationTableSpace(Relation rel, Oid newTableSpaceId,
								  Oid newRelFileNode);

extern PGDLLIMPORT ObjectAddress renameatt(RenameStmt *stmt);

extern PGDLLIMPORT ObjectAddress RenameConstraint(RenameStmt *stmt);

extern PGDLLIMPORT ObjectAddress RenameRelation(RenameStmt *stmt);

extern PGDLLIMPORT void RenameRelationInternal(Oid myrelid,
								   const char *newrelname, bool is_internal,
								   bool is_index);

extern PGDLLIMPORT void ResetRelRewrite(Oid myrelid);

extern PGDLLIMPORT void find_composite_type_dependencies(Oid typeOid,
											 Relation origRelation,
											 const char *origTypeName);

extern PGDLLIMPORT void check_of_type(HeapTuple typetuple);

extern PGDLLIMPORT void register_on_commit_action(Oid relid, OnCommitAction action);
extern PGDLLIMPORT void remove_on_commit_action(Oid relid);

extern PGDLLIMPORT void PreCommit_on_commit_actions(void);
extern PGDLLIMPORT void AtEOXact_on_commit_actions(bool isCommit);
extern PGDLLIMPORT void AtEOSubXact_on_commit_actions(bool isCommit,
										  SubTransactionId mySubid,
										  SubTransactionId parentSubid);

extern PGDLLIMPORT void RangeVarCallbackOwnsTable(const RangeVar *relation,
									  Oid relId, Oid oldRelId, void *arg);

extern PGDLLIMPORT void RangeVarCallbackOwnsRelation(const RangeVar *relation,
										 Oid relId, Oid oldRelId, void *arg);
extern PGDLLIMPORT bool PartConstraintImpliedByRelConstraint(Relation scanrel,
												 List *partConstraint);

#endif							/* TABLECMDS_H */
