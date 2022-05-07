/*-------------------------------------------------------------------------
 *
 * tlist.h
 *	  prototypes for tlist.c.
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/optimizer/tlist.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef TLIST_H
#define TLIST_H

#include "nodes/pathnodes.h"


extern PGDLLIMPORT TargetEntry *tlist_member(Expr *node, List *targetlist);

extern PGDLLIMPORT List *add_to_flat_tlist(List *tlist, List *exprs);

extern PGDLLIMPORT List *get_tlist_exprs(List *tlist, bool includeJunk);

extern PGDLLIMPORT bool tlist_same_exprs(List *tlist1, List *tlist2);

extern PGDLLIMPORT bool tlist_same_datatypes(List *tlist, List *colTypes, bool junkOK);
extern PGDLLIMPORT bool tlist_same_collations(List *tlist, List *colCollations, bool junkOK);

extern PGDLLIMPORT void apply_tlist_labeling(List *dest_tlist, List *src_tlist);

extern PGDLLIMPORT Oid *extract_grouping_ops(List *groupClause);
extern PGDLLIMPORT Oid *extract_grouping_collations(List *groupClause, List *tlist);
extern PGDLLIMPORT AttrNumber *extract_grouping_cols(List *groupClause, List *tlist);
extern PGDLLIMPORT bool grouping_is_sortable(List *groupClause);
extern PGDLLIMPORT bool grouping_is_hashable(List *groupClause);

extern PGDLLIMPORT PathTarget *make_pathtarget_from_tlist(List *tlist);
extern PGDLLIMPORT List *make_tlist_from_pathtarget(PathTarget *target);
extern PGDLLIMPORT PathTarget *copy_pathtarget(PathTarget *src);
extern PGDLLIMPORT PathTarget *create_empty_pathtarget(void);
extern PGDLLIMPORT void add_column_to_pathtarget(PathTarget *target,
									 Expr *expr, Index sortgroupref);
extern PGDLLIMPORT void add_new_column_to_pathtarget(PathTarget *target, Expr *expr);
extern PGDLLIMPORT void add_new_columns_to_pathtarget(PathTarget *target, List *exprs);
extern PGDLLIMPORT void apply_pathtarget_labeling_to_tlist(List *tlist, PathTarget *target);
extern PGDLLIMPORT void split_pathtarget_at_srfs(PlannerInfo *root,
									 PathTarget *target, PathTarget *input_target,
									 List **targets, List **targets_contain_srfs);

/* Convenience macro to get a PathTarget with valid cost/width fields */
#define create_pathtarget(root, tlist) \
	set_pathtarget_cost_width(root, make_pathtarget_from_tlist(tlist))

#endif							/* TLIST_H */
