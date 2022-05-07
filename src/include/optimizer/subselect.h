/*-------------------------------------------------------------------------
 *
 * subselect.h
 *	  Planning routines for subselects.
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/optimizer/subselect.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef SUBSELECT_H
#define SUBSELECT_H

#include "nodes/pathnodes.h"
#include "nodes/plannodes.h"

extern PGDLLIMPORT void SS_process_ctes(PlannerInfo *root);
extern PGDLLIMPORT JoinExpr *convert_ANY_sublink_to_join(PlannerInfo *root,
											 SubLink *sublink,
											 Relids available_rels);
extern PGDLLIMPORT JoinExpr *convert_EXISTS_sublink_to_join(PlannerInfo *root,
												SubLink *sublink,
												bool under_not,
												Relids available_rels);
extern PGDLLIMPORT Node *SS_replace_correlation_vars(PlannerInfo *root, Node *expr);
extern PGDLLIMPORT Node *SS_process_sublinks(PlannerInfo *root, Node *expr, bool isQual);
extern PGDLLIMPORT void SS_identify_outer_params(PlannerInfo *root);
extern PGDLLIMPORT void SS_charge_for_initplans(PlannerInfo *root, RelOptInfo *final_rel);
extern PGDLLIMPORT void SS_attach_initplans(PlannerInfo *root, Plan *plan);
extern PGDLLIMPORT void SS_finalize_plan(PlannerInfo *root, Plan *plan);
extern PGDLLIMPORT Param *SS_make_initplan_output_param(PlannerInfo *root,
											Oid resulttype, int32 resulttypmod,
											Oid resultcollation);
extern PGDLLIMPORT void SS_make_initplan_from_plan(PlannerInfo *root,
									   PlannerInfo *subroot, Plan *plan,
									   Param *prm);

#endif							/* SUBSELECT_H */
