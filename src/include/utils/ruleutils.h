/*-------------------------------------------------------------------------
 *
 * ruleutils.h
 *		Declarations for ruleutils.c
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/utils/ruleutils.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef RULEUTILS_H
#define RULEUTILS_H

#include "nodes/nodes.h"
#include "nodes/parsenodes.h"
#include "nodes/pg_list.h"

struct Plan;					/* avoid including plannodes.h here */
struct PlannedStmt;


extern PGDLLIMPORT char *pg_get_indexdef_string(Oid indexrelid);
extern PGDLLIMPORT char *pg_get_indexdef_columns(Oid indexrelid, bool pretty);
extern PGDLLIMPORT char *pg_get_querydef(Query *query, bool pretty);

extern PGDLLIMPORT char *pg_get_partkeydef_columns(Oid relid, bool pretty);
extern PGDLLIMPORT char *pg_get_partconstrdef_string(Oid partitionId, char *aliasname);

extern PGDLLIMPORT char *pg_get_constraintdef_command(Oid constraintId);
extern PGDLLIMPORT char *deparse_expression(Node *expr, List *dpcontext,
								bool forceprefix, bool showimplicit);
extern PGDLLIMPORT List *deparse_context_for(const char *aliasname, Oid relid);
extern PGDLLIMPORT List *deparse_context_for_plan_tree(struct PlannedStmt *pstmt,
										   List *rtable_names);
extern PGDLLIMPORT List *set_deparse_context_plan(List *dpcontext,
									  struct Plan *plan, List *ancestors);
extern PGDLLIMPORT List *select_rtable_names_for_explain(List *rtable,
											 Bitmapset *rels_used);
extern PGDLLIMPORT char *generate_collation_name(Oid collid);
extern PGDLLIMPORT char *generate_opclass_name(Oid opclass);
extern PGDLLIMPORT char *get_range_partbound_string(List *bound_datums);

extern PGDLLIMPORT char *pg_get_statisticsobjdef_string(Oid statextid);

#endif							/* RULEUTILS_H */
