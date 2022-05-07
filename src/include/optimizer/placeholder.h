/*-------------------------------------------------------------------------
 *
 * placeholder.h
 *	  prototypes for optimizer/util/placeholder.c.
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/optimizer/placeholder.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PLACEHOLDER_H
#define PLACEHOLDER_H

#include "nodes/pathnodes.h"


extern PGDLLIMPORT PlaceHolderVar *make_placeholder_expr(PlannerInfo *root, Expr *expr,
											 Relids phrels);
extern PGDLLIMPORT PlaceHolderInfo *find_placeholder_info(PlannerInfo *root,
											  PlaceHolderVar *phv, bool create_new_ph);
extern PGDLLIMPORT void find_placeholders_in_jointree(PlannerInfo *root);
extern PGDLLIMPORT void update_placeholder_eval_levels(PlannerInfo *root,
										   SpecialJoinInfo *new_sjinfo);
extern PGDLLIMPORT void fix_placeholder_input_needed_levels(PlannerInfo *root);
extern PGDLLIMPORT void add_placeholders_to_base_rels(PlannerInfo *root);
extern PGDLLIMPORT void add_placeholders_to_joinrel(PlannerInfo *root, RelOptInfo *joinrel,
										RelOptInfo *outer_rel, RelOptInfo *inner_rel);

#endif							/* PLACEHOLDER_H */
