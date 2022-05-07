/*-------------------------------------------------------------------------
 *
 * pathnode.h
 *	  prototypes for pathnode.c, relnode.c.
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/optimizer/pathnode.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PATHNODE_H
#define PATHNODE_H

#include "nodes/bitmapset.h"
#include "nodes/pathnodes.h"


/*
 * prototypes for pathnode.c
 */
extern PGDLLIMPORT int	compare_path_costs(Path *path1, Path *path2,
							   CostSelector criterion);
extern PGDLLIMPORT int	compare_fractional_path_costs(Path *path1, Path *path2,
										  double fraction);
extern PGDLLIMPORT void set_cheapest(RelOptInfo *parent_rel);
extern PGDLLIMPORT void add_path(RelOptInfo *parent_rel, Path *new_path);
extern PGDLLIMPORT bool add_path_precheck(RelOptInfo *parent_rel,
							  Cost startup_cost, Cost total_cost,
							  List *pathkeys, Relids required_outer);
extern PGDLLIMPORT void add_partial_path(RelOptInfo *parent_rel, Path *new_path);
extern PGDLLIMPORT bool add_partial_path_precheck(RelOptInfo *parent_rel,
									  Cost total_cost, List *pathkeys);

extern PGDLLIMPORT Path *create_seqscan_path(PlannerInfo *root, RelOptInfo *rel,
								 Relids required_outer, int parallel_workers);
extern PGDLLIMPORT Path *create_samplescan_path(PlannerInfo *root, RelOptInfo *rel,
									Relids required_outer);
extern PGDLLIMPORT IndexPath *create_index_path(PlannerInfo *root,
									IndexOptInfo *index,
									List *indexclauses,
									List *indexorderbys,
									List *indexorderbycols,
									List *pathkeys,
									ScanDirection indexscandir,
									bool indexonly,
									Relids required_outer,
									double loop_count,
									bool partial_path);
extern PGDLLIMPORT BitmapHeapPath *create_bitmap_heap_path(PlannerInfo *root,
											   RelOptInfo *rel,
											   Path *bitmapqual,
											   Relids required_outer,
											   double loop_count,
											   int parallel_degree);
extern PGDLLIMPORT BitmapAndPath *create_bitmap_and_path(PlannerInfo *root,
											 RelOptInfo *rel,
											 List *bitmapquals);
extern PGDLLIMPORT BitmapOrPath *create_bitmap_or_path(PlannerInfo *root,
										   RelOptInfo *rel,
										   List *bitmapquals);
extern PGDLLIMPORT TidPath *create_tidscan_path(PlannerInfo *root, RelOptInfo *rel,
									List *tidquals, Relids required_outer);
extern PGDLLIMPORT TidRangePath *create_tidrangescan_path(PlannerInfo *root,
											  RelOptInfo *rel,
											  List *tidrangequals,
											  Relids required_outer);
extern PGDLLIMPORT AppendPath *create_append_path(PlannerInfo *root, RelOptInfo *rel,
									  List *subpaths, List *partial_subpaths,
									  List *pathkeys, Relids required_outer,
									  int parallel_workers, bool parallel_aware,
									  double rows);
extern PGDLLIMPORT MergeAppendPath *create_merge_append_path(PlannerInfo *root,
												 RelOptInfo *rel,
												 List *subpaths,
												 List *pathkeys,
												 Relids required_outer);
extern PGDLLIMPORT GroupResultPath *create_group_result_path(PlannerInfo *root,
												 RelOptInfo *rel,
												 PathTarget *target,
												 List *havingqual);
extern PGDLLIMPORT MaterialPath *create_material_path(RelOptInfo *rel, Path *subpath);
extern PGDLLIMPORT MemoizePath *create_memoize_path(PlannerInfo *root,
										RelOptInfo *rel,
										Path *subpath,
										List *param_exprs,
										List *hash_operators,
										bool singlerow,
										bool binary_mode,
										double calls);
extern PGDLLIMPORT UniquePath *create_unique_path(PlannerInfo *root, RelOptInfo *rel,
									  Path *subpath, SpecialJoinInfo *sjinfo);
extern PGDLLIMPORT GatherPath *create_gather_path(PlannerInfo *root,
									  RelOptInfo *rel, Path *subpath, PathTarget *target,
									  Relids required_outer, double *rows);
extern PGDLLIMPORT GatherMergePath *create_gather_merge_path(PlannerInfo *root,
												 RelOptInfo *rel,
												 Path *subpath,
												 PathTarget *target,
												 List *pathkeys,
												 Relids required_outer,
												 double *rows);
extern PGDLLIMPORT SubqueryScanPath *create_subqueryscan_path(PlannerInfo *root,
												  RelOptInfo *rel, Path *subpath,
												  List *pathkeys, Relids required_outer);
extern PGDLLIMPORT Path *create_functionscan_path(PlannerInfo *root, RelOptInfo *rel,
									  List *pathkeys, Relids required_outer);
extern PGDLLIMPORT Path *create_valuesscan_path(PlannerInfo *root, RelOptInfo *rel,
									Relids required_outer);
extern PGDLLIMPORT Path *create_tablefuncscan_path(PlannerInfo *root, RelOptInfo *rel,
									   Relids required_outer);
extern PGDLLIMPORT Path *create_ctescan_path(PlannerInfo *root, RelOptInfo *rel,
								 Relids required_outer);
extern PGDLLIMPORT Path *create_namedtuplestorescan_path(PlannerInfo *root, RelOptInfo *rel,
											 Relids required_outer);
extern PGDLLIMPORT Path *create_resultscan_path(PlannerInfo *root, RelOptInfo *rel,
									Relids required_outer);
extern PGDLLIMPORT Path *create_worktablescan_path(PlannerInfo *root, RelOptInfo *rel,
									   Relids required_outer);
extern PGDLLIMPORT ForeignPath *create_foreignscan_path(PlannerInfo *root, RelOptInfo *rel,
											PathTarget *target,
											double rows, Cost startup_cost, Cost total_cost,
											List *pathkeys,
											Relids required_outer,
											Path *fdw_outerpath,
											List *fdw_private);
extern PGDLLIMPORT ForeignPath *create_foreign_join_path(PlannerInfo *root, RelOptInfo *rel,
											 PathTarget *target,
											 double rows, Cost startup_cost, Cost total_cost,
											 List *pathkeys,
											 Relids required_outer,
											 Path *fdw_outerpath,
											 List *fdw_private);
extern PGDLLIMPORT ForeignPath *create_foreign_upper_path(PlannerInfo *root, RelOptInfo *rel,
											  PathTarget *target,
											  double rows, Cost startup_cost, Cost total_cost,
											  List *pathkeys,
											  Path *fdw_outerpath,
											  List *fdw_private);

extern PGDLLIMPORT Relids calc_nestloop_required_outer(Relids outerrelids,
										   Relids outer_paramrels,
										   Relids innerrelids,
										   Relids inner_paramrels);
extern PGDLLIMPORT Relids calc_non_nestloop_required_outer(Path *outer_path, Path *inner_path);

extern PGDLLIMPORT NestPath *create_nestloop_path(PlannerInfo *root,
									  RelOptInfo *joinrel,
									  JoinType jointype,
									  JoinCostWorkspace *workspace,
									  JoinPathExtraData *extra,
									  Path *outer_path,
									  Path *inner_path,
									  List *restrict_clauses,
									  List *pathkeys,
									  Relids required_outer);

extern PGDLLIMPORT MergePath *create_mergejoin_path(PlannerInfo *root,
										RelOptInfo *joinrel,
										JoinType jointype,
										JoinCostWorkspace *workspace,
										JoinPathExtraData *extra,
										Path *outer_path,
										Path *inner_path,
										List *restrict_clauses,
										List *pathkeys,
										Relids required_outer,
										List *mergeclauses,
										List *outersortkeys,
										List *innersortkeys);

extern PGDLLIMPORT HashPath *create_hashjoin_path(PlannerInfo *root,
									  RelOptInfo *joinrel,
									  JoinType jointype,
									  JoinCostWorkspace *workspace,
									  JoinPathExtraData *extra,
									  Path *outer_path,
									  Path *inner_path,
									  bool parallel_hash,
									  List *restrict_clauses,
									  Relids required_outer,
									  List *hashclauses);

extern PGDLLIMPORT ProjectionPath *create_projection_path(PlannerInfo *root,
											  RelOptInfo *rel,
											  Path *subpath,
											  PathTarget *target);
extern PGDLLIMPORT Path *apply_projection_to_path(PlannerInfo *root,
									  RelOptInfo *rel,
									  Path *path,
									  PathTarget *target);
extern PGDLLIMPORT ProjectSetPath *create_set_projection_path(PlannerInfo *root,
												  RelOptInfo *rel,
												  Path *subpath,
												  PathTarget *target);
extern PGDLLIMPORT SortPath *create_sort_path(PlannerInfo *root,
								  RelOptInfo *rel,
								  Path *subpath,
								  List *pathkeys,
								  double limit_tuples);
extern PGDLLIMPORT IncrementalSortPath *create_incremental_sort_path(PlannerInfo *root,
														 RelOptInfo *rel,
														 Path *subpath,
														 List *pathkeys,
														 int presorted_keys,
														 double limit_tuples);
extern PGDLLIMPORT GroupPath *create_group_path(PlannerInfo *root,
									RelOptInfo *rel,
									Path *subpath,
									List *groupClause,
									List *qual,
									double numGroups);
extern PGDLLIMPORT UpperUniquePath *create_upper_unique_path(PlannerInfo *root,
												 RelOptInfo *rel,
												 Path *subpath,
												 int numCols,
												 double numGroups);
extern PGDLLIMPORT AggPath *create_agg_path(PlannerInfo *root,
								RelOptInfo *rel,
								Path *subpath,
								PathTarget *target,
								AggStrategy aggstrategy,
								AggSplit aggsplit,
								List *groupClause,
								List *qual,
								const AggClauseCosts *aggcosts,
								double numGroups);
extern PGDLLIMPORT GroupingSetsPath *create_groupingsets_path(PlannerInfo *root,
												  RelOptInfo *rel,
												  Path *subpath,
												  List *having_qual,
												  AggStrategy aggstrategy,
												  List *rollups,
												  const AggClauseCosts *agg_costs,
												  double numGroups);
extern PGDLLIMPORT MinMaxAggPath *create_minmaxagg_path(PlannerInfo *root,
											RelOptInfo *rel,
											PathTarget *target,
											List *mmaggregates,
											List *quals);
extern PGDLLIMPORT WindowAggPath *create_windowagg_path(PlannerInfo *root,
											RelOptInfo *rel,
											Path *subpath,
											PathTarget *target,
											List *windowFuncs,
											WindowClause *winclause,
											List *qual,
											bool topwindow);
extern PGDLLIMPORT SetOpPath *create_setop_path(PlannerInfo *root,
									RelOptInfo *rel,
									Path *subpath,
									SetOpCmd cmd,
									SetOpStrategy strategy,
									List *distinctList,
									AttrNumber flagColIdx,
									int firstFlag,
									double numGroups,
									double outputRows);
extern PGDLLIMPORT RecursiveUnionPath *create_recursiveunion_path(PlannerInfo *root,
													  RelOptInfo *rel,
													  Path *leftpath,
													  Path *rightpath,
													  PathTarget *target,
													  List *distinctList,
													  int wtParam,
													  double numGroups);
extern PGDLLIMPORT LockRowsPath *create_lockrows_path(PlannerInfo *root, RelOptInfo *rel,
										  Path *subpath, List *rowMarks, int epqParam);
extern PGDLLIMPORT ModifyTablePath *create_modifytable_path(PlannerInfo *root,
												RelOptInfo *rel,
												Path *subpath,
												CmdType operation, bool canSetTag,
												Index nominalRelation, Index rootRelation,
												bool partColsUpdated,
												List *resultRelations,
												List *updateColnosLists,
												List *withCheckOptionLists, List *returningLists,
												List *rowMarks, OnConflictExpr *onconflict,
												List *mergeActionLists, int epqParam);
extern PGDLLIMPORT LimitPath *create_limit_path(PlannerInfo *root, RelOptInfo *rel,
									Path *subpath,
									Node *limitOffset, Node *limitCount,
									LimitOption limitOption,
									int64 offset_est, int64 count_est);
extern PGDLLIMPORT void adjust_limit_rows_costs(double *rows,
									Cost *startup_cost, Cost *total_cost,
									int64 offset_est, int64 count_est);

extern PGDLLIMPORT Path *reparameterize_path(PlannerInfo *root, Path *path,
								 Relids required_outer,
								 double loop_count);
extern PGDLLIMPORT Path *reparameterize_path_by_child(PlannerInfo *root, Path *path,
										  RelOptInfo *child_rel);

/*
 * prototypes for relnode.c
 */
extern PGDLLIMPORT void setup_simple_rel_arrays(PlannerInfo *root);
extern PGDLLIMPORT void expand_planner_arrays(PlannerInfo *root, int add_size);
extern PGDLLIMPORT RelOptInfo *build_simple_rel(PlannerInfo *root, int relid,
									RelOptInfo *parent);
extern PGDLLIMPORT RelOptInfo *find_base_rel(PlannerInfo *root, int relid);
extern PGDLLIMPORT RelOptInfo *find_join_rel(PlannerInfo *root, Relids relids);
extern PGDLLIMPORT RelOptInfo *build_join_rel(PlannerInfo *root,
								  Relids joinrelids,
								  RelOptInfo *outer_rel,
								  RelOptInfo *inner_rel,
								  SpecialJoinInfo *sjinfo,
								  List **restrictlist_ptr);
extern PGDLLIMPORT Relids min_join_parameterization(PlannerInfo *root,
										Relids joinrelids,
										RelOptInfo *outer_rel,
										RelOptInfo *inner_rel);
extern PGDLLIMPORT RelOptInfo *fetch_upper_rel(PlannerInfo *root, UpperRelationKind kind,
								   Relids relids);
extern PGDLLIMPORT Relids find_childrel_parents(PlannerInfo *root, RelOptInfo *rel);
extern PGDLLIMPORT ParamPathInfo *get_baserel_parampathinfo(PlannerInfo *root,
												RelOptInfo *baserel,
												Relids required_outer);
extern PGDLLIMPORT ParamPathInfo *get_joinrel_parampathinfo(PlannerInfo *root,
												RelOptInfo *joinrel,
												Path *outer_path,
												Path *inner_path,
												SpecialJoinInfo *sjinfo,
												Relids required_outer,
												List **restrict_clauses);
extern PGDLLIMPORT ParamPathInfo *get_appendrel_parampathinfo(RelOptInfo *appendrel,
												  Relids required_outer);
extern PGDLLIMPORT ParamPathInfo *find_param_path_info(RelOptInfo *rel,
										   Relids required_outer);
extern PGDLLIMPORT RelOptInfo *build_child_join_rel(PlannerInfo *root,
										RelOptInfo *outer_rel, RelOptInfo *inner_rel,
										RelOptInfo *parent_joinrel, List *restrictlist,
										SpecialJoinInfo *sjinfo, JoinType jointype);

#endif							/* PATHNODE_H */
