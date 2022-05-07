/*-------------------------------------------------------------------------
 *
 * parse_utilcmd.h
 *		parse analysis for utility commands
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/parser/parse_utilcmd.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PARSE_UTILCMD_H
#define PARSE_UTILCMD_H

#include "parser/parse_node.h"

struct AttrMap;					/* avoid including attmap.h here */


extern PGDLLIMPORT List *transformCreateStmt(CreateStmt *stmt, const char *queryString);
extern PGDLLIMPORT AlterTableStmt *transformAlterTableStmt(Oid relid, AlterTableStmt *stmt,
											   const char *queryString,
											   List **beforeStmts,
											   List **afterStmts);
extern PGDLLIMPORT IndexStmt *transformIndexStmt(Oid relid, IndexStmt *stmt,
									 const char *queryString);
extern PGDLLIMPORT CreateStatsStmt *transformStatsStmt(Oid relid, CreateStatsStmt *stmt,
										   const char *queryString);
extern PGDLLIMPORT void transformRuleStmt(RuleStmt *stmt, const char *queryString,
							  List **actions, Node **whereClause);
extern PGDLLIMPORT List *transformCreateSchemaStmt(CreateSchemaStmt *stmt);
extern PGDLLIMPORT PartitionBoundSpec *transformPartitionBound(ParseState *pstate, Relation parent,
												   PartitionBoundSpec *spec);
extern PGDLLIMPORT List *expandTableLikeClause(RangeVar *heapRel,
								   TableLikeClause *table_like_clause);
extern PGDLLIMPORT IndexStmt *generateClonedIndexStmt(RangeVar *heapRel,
										  Relation source_idx,
										  const struct AttrMap *attmap,
										  Oid *constraintOid);

#endif							/* PARSE_UTILCMD_H */
