/*-------------------------------------------------------------------------
 *
 * paramassign.h
 *		Functions for assigning PARAM_EXEC slots during planning.
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/optimizer/paramassign.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PARAMASSIGN_H
#define PARAMASSIGN_H

#include "nodes/pathnodes.h"

extern PGDLLIMPORT Param *replace_outer_var(PlannerInfo *root, Var *var);
extern PGDLLIMPORT Param *replace_outer_placeholdervar(PlannerInfo *root,
										   PlaceHolderVar *phv);
extern PGDLLIMPORT Param *replace_outer_agg(PlannerInfo *root, Aggref *agg);
extern PGDLLIMPORT Param *replace_outer_grouping(PlannerInfo *root, GroupingFunc *grp);
extern PGDLLIMPORT Param *replace_nestloop_param_var(PlannerInfo *root, Var *var);
extern PGDLLIMPORT Param *replace_nestloop_param_placeholdervar(PlannerInfo *root,
													PlaceHolderVar *phv);
extern PGDLLIMPORT void process_subquery_nestloop_params(PlannerInfo *root,
											 List *subplan_params);
extern PGDLLIMPORT List *identify_current_nestloop_params(PlannerInfo *root,
											  Relids leftrelids);
extern PGDLLIMPORT Param *generate_new_exec_param(PlannerInfo *root, Oid paramtype,
									  int32 paramtypmod, Oid paramcollation);
extern PGDLLIMPORT int	assign_special_exec_param(PlannerInfo *root);

#endif							/* PARAMASSIGN_H */
