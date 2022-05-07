/*-------------------------------------------------------------------------
 *
 * parse_clause.h
 *	  handle clauses in parser
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/parser/parse_clause.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PARSE_CLAUSE_H
#define PARSE_CLAUSE_H

#include "parser/parse_node.h"

extern PGDLLIMPORT void transformFromClause(ParseState *pstate, List *frmList);
extern PGDLLIMPORT int	setTargetTable(ParseState *pstate, RangeVar *relation,
						   bool inh, bool alsoSource, AclMode requiredPerms);

extern PGDLLIMPORT Node *transformWhereClause(ParseState *pstate, Node *clause,
								  ParseExprKind exprKind, const char *constructName);
extern PGDLLIMPORT Node *transformLimitClause(ParseState *pstate, Node *clause,
								  ParseExprKind exprKind, const char *constructName,
								  LimitOption limitOption);
extern PGDLLIMPORT List *transformGroupClause(ParseState *pstate, List *grouplist,
								  List **groupingSets,
								  List **targetlist, List *sortClause,
								  ParseExprKind exprKind, bool useSQL99);
extern PGDLLIMPORT List *transformSortClause(ParseState *pstate, List *orderlist,
								 List **targetlist, ParseExprKind exprKind,
								 bool useSQL99);

extern PGDLLIMPORT List *transformWindowDefinitions(ParseState *pstate,
										List *windowdefs,
										List **targetlist);

extern PGDLLIMPORT List *transformDistinctClause(ParseState *pstate,
									 List **targetlist, List *sortClause, bool is_agg);
extern PGDLLIMPORT List *transformDistinctOnClause(ParseState *pstate, List *distinctlist,
									   List **targetlist, List *sortClause);
extern PGDLLIMPORT void transformOnConflictArbiter(ParseState *pstate,
									   OnConflictClause *onConflictClause,
									   List **arbiterExpr, Node **arbiterWhere,
									   Oid *constraint);

extern PGDLLIMPORT List *addTargetToSortList(ParseState *pstate, TargetEntry *tle,
								 List *sortlist, List *targetlist, SortBy *sortby);
extern PGDLLIMPORT Index assignSortGroupRef(TargetEntry *tle, List *tlist);
extern PGDLLIMPORT bool targetIsInSortList(TargetEntry *tle, Oid sortop, List *sortList);

/* functions in parse_jsontable.c */
extern PGDLLIMPORT ParseNamespaceItem *transformJsonTable(ParseState *pstate, JsonTable *jt);

#endif							/* PARSE_CLAUSE_H */
