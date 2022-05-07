/*-------------------------------------------------------------------------
 *
 * prep.h
 *	  prototypes for files in optimizer/prep/
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/optimizer/prep.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PREP_H
#define PREP_H

#include "nodes/pathnodes.h"
#include "nodes/plannodes.h"


/*
 * prototypes for prepjointree.c
 */
extern PGDLLIMPORT void transform_MERGE_to_join(Query *parse);
extern PGDLLIMPORT void replace_empty_jointree(Query *parse);
extern PGDLLIMPORT void pull_up_sublinks(PlannerInfo *root);
extern PGDLLIMPORT void preprocess_function_rtes(PlannerInfo *root);
extern PGDLLIMPORT void pull_up_subqueries(PlannerInfo *root);
extern PGDLLIMPORT void flatten_simple_union_all(PlannerInfo *root);
extern PGDLLIMPORT void reduce_outer_joins(PlannerInfo *root);
extern PGDLLIMPORT void remove_useless_result_rtes(PlannerInfo *root);
extern PGDLLIMPORT Relids get_relids_in_jointree(Node *jtnode, bool include_joins);
extern PGDLLIMPORT Relids get_relids_for_join(Query *query, int joinrelid);

/*
 * prototypes for preptlist.c
 */
extern PGDLLIMPORT void preprocess_targetlist(PlannerInfo *root);

extern PGDLLIMPORT List *extract_update_targetlist_colnos(List *tlist);

extern PGDLLIMPORT PlanRowMark *get_plan_rowmark(List *rowmarks, Index rtindex);

/*
 * prototypes for prepagg.c
 */
extern PGDLLIMPORT void get_agg_clause_costs(PlannerInfo *root, AggSplit aggsplit,
								 AggClauseCosts *agg_costs);
extern PGDLLIMPORT void preprocess_aggrefs(PlannerInfo *root, Node *clause);

/*
 * prototypes for prepunion.c
 */
extern PGDLLIMPORT RelOptInfo *plan_set_operations(PlannerInfo *root);


#endif							/* PREP_H */
