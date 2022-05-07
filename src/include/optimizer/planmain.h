/*-------------------------------------------------------------------------
 *
 * planmain.h
 *	  prototypes for various files in optimizer/plan
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/optimizer/planmain.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PLANMAIN_H
#define PLANMAIN_H

#include "nodes/pathnodes.h"
#include "nodes/plannodes.h"

/* GUC parameters */
#define DEFAULT_CURSOR_TUPLE_FRACTION 0.1
extern PGDLLIMPORT double cursor_tuple_fraction;

/* query_planner callback to compute query_pathkeys */
typedef void (*query_pathkeys_callback) (PlannerInfo *root, void *extra);

/*
 * prototypes for plan/planmain.c
 */
extern PGDLLIMPORT RelOptInfo *query_planner(PlannerInfo *root,
								 query_pathkeys_callback qp_callback, void *qp_extra);

/*
 * prototypes for plan/planagg.c
 */
extern PGDLLIMPORT void preprocess_minmax_aggregates(PlannerInfo *root);

/*
 * prototypes for plan/createplan.c
 */
extern PGDLLIMPORT Plan *create_plan(PlannerInfo *root, Path *best_path);
extern PGDLLIMPORT ForeignScan *make_foreignscan(List *qptlist, List *qpqual,
									 Index scanrelid, List *fdw_exprs, List *fdw_private,
									 List *fdw_scan_tlist, List *fdw_recheck_quals,
									 Plan *outer_plan);
extern PGDLLIMPORT Plan *change_plan_targetlist(Plan *subplan, List *tlist,
									bool tlist_parallel_safe);
extern PGDLLIMPORT Plan *materialize_finished_plan(Plan *subplan);
extern PGDLLIMPORT bool is_projection_capable_path(Path *path);
extern PGDLLIMPORT bool is_projection_capable_plan(Plan *plan);

/* External use of these functions is deprecated: */
extern PGDLLIMPORT Sort *make_sort_from_sortclauses(List *sortcls, Plan *lefttree);
extern PGDLLIMPORT Agg *make_agg(List *tlist, List *qual,
					 AggStrategy aggstrategy, AggSplit aggsplit,
					 int numGroupCols, AttrNumber *grpColIdx, Oid *grpOperators, Oid *grpCollations,
					 List *groupingSets, List *chain, double dNumGroups,
					 Size transitionSpace, Plan *lefttree);
extern PGDLLIMPORT Limit *make_limit(Plan *lefttree, Node *limitOffset, Node *limitCount,
						 LimitOption limitOption, int uniqNumCols,
						 AttrNumber *uniqColIdx, Oid *uniqOperators,
						 Oid *uniqCollations);

/*
 * prototypes for plan/initsplan.c
 */
extern PGDLLIMPORT int from_collapse_limit;
extern PGDLLIMPORT int join_collapse_limit;

extern PGDLLIMPORT void add_base_rels_to_query(PlannerInfo *root, Node *jtnode);
extern PGDLLIMPORT void add_other_rels_to_query(PlannerInfo *root);
extern PGDLLIMPORT void build_base_rel_tlists(PlannerInfo *root, List *final_tlist);
extern PGDLLIMPORT void add_vars_to_targetlist(PlannerInfo *root, List *vars,
								   Relids where_needed, bool create_new_ph);
extern PGDLLIMPORT void find_lateral_references(PlannerInfo *root);
extern PGDLLIMPORT void create_lateral_join_info(PlannerInfo *root);
extern PGDLLIMPORT List *deconstruct_jointree(PlannerInfo *root);
extern PGDLLIMPORT void distribute_restrictinfo_to_rels(PlannerInfo *root,
											RestrictInfo *restrictinfo);
extern PGDLLIMPORT RestrictInfo *process_implied_equality(PlannerInfo *root,
											  Oid opno,
											  Oid collation,
											  Expr *item1,
											  Expr *item2,
											  Relids qualscope,
											  Relids nullable_relids,
											  Index security_level,
											  bool below_outer_join,
											  bool both_const);
extern PGDLLIMPORT RestrictInfo *build_implied_join_equality(PlannerInfo *root,
												 Oid opno,
												 Oid collation,
												 Expr *item1,
												 Expr *item2,
												 Relids qualscope,
												 Relids nullable_relids,
												 Index security_level);
extern PGDLLIMPORT void match_foreign_keys_to_quals(PlannerInfo *root);

/*
 * prototypes for plan/analyzejoins.c
 */
extern PGDLLIMPORT List *remove_useless_joins(PlannerInfo *root, List *joinlist);
extern PGDLLIMPORT void reduce_unique_semijoins(PlannerInfo *root);
extern PGDLLIMPORT bool query_supports_distinctness(Query *query);
extern PGDLLIMPORT bool query_is_distinct_for(Query *query, List *colnos, List *opids);
extern PGDLLIMPORT bool innerrel_is_unique(PlannerInfo *root,
							   Relids joinrelids, Relids outerrelids, RelOptInfo *innerrel,
							   JoinType jointype, List *restrictlist, bool force_cache);

/*
 * prototypes for plan/setrefs.c
 */
extern PGDLLIMPORT Plan *set_plan_references(PlannerInfo *root, Plan *plan);
extern PGDLLIMPORT bool trivial_subqueryscan(SubqueryScan *plan);
extern PGDLLIMPORT void record_plan_function_dependency(PlannerInfo *root, Oid funcid);
extern PGDLLIMPORT void record_plan_type_dependency(PlannerInfo *root, Oid typid);
extern PGDLLIMPORT bool extract_query_dependencies_walker(Node *node, PlannerInfo *root);

#endif							/* PLANMAIN_H */
