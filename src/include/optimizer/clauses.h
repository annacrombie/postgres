/*-------------------------------------------------------------------------
 *
 * clauses.h
 *	  prototypes for clauses.c.
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/optimizer/clauses.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef CLAUSES_H
#define CLAUSES_H

#include "nodes/pathnodes.h"

typedef struct
{
	int			numWindowFuncs; /* total number of WindowFuncs found */
	Index		maxWinRef;		/* windowFuncs[] is indexed 0 .. maxWinRef */
	List	  **windowFuncs;	/* lists of WindowFuncs for each winref */
} WindowFuncLists;

extern PGDLLIMPORT bool contain_agg_clause(Node *clause);

extern PGDLLIMPORT bool contain_window_function(Node *clause);
extern PGDLLIMPORT WindowFuncLists *find_window_functions(Node *clause, Index maxWinRef);

extern PGDLLIMPORT double expression_returns_set_rows(PlannerInfo *root, Node *clause);

extern PGDLLIMPORT bool contain_subplans(Node *clause);

extern PGDLLIMPORT char max_parallel_hazard(Query *parse);
extern PGDLLIMPORT bool is_parallel_safe(PlannerInfo *root, Node *node);
extern PGDLLIMPORT bool contain_nonstrict_functions(Node *clause);
extern PGDLLIMPORT bool contain_exec_param(Node *clause, List *param_ids);
extern PGDLLIMPORT bool contain_leaked_vars(Node *clause);

extern PGDLLIMPORT Relids find_nonnullable_rels(Node *clause);
extern PGDLLIMPORT List *find_nonnullable_vars(Node *clause);
extern PGDLLIMPORT List *find_forced_null_vars(Node *clause);
extern PGDLLIMPORT Var *find_forced_null_var(Node *clause);

extern PGDLLIMPORT bool is_pseudo_constant_clause(Node *clause);
extern PGDLLIMPORT bool is_pseudo_constant_clause_relids(Node *clause, Relids relids);

extern PGDLLIMPORT int	NumRelids(PlannerInfo *root, Node *clause);

extern PGDLLIMPORT void CommuteOpExpr(OpExpr *clause);

extern PGDLLIMPORT Query *inline_set_returning_function(PlannerInfo *root,
											RangeTblEntry *rte);

extern PGDLLIMPORT Bitmapset *pull_paramids(Expr *expr);

#endif							/* CLAUSES_H */
