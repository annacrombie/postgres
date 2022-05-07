/*-------------------------------------------------------------------------
 *
 * nodeSubplan.h
 *
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/executor/nodeSubplan.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef NODESUBPLAN_H
#define NODESUBPLAN_H

#include "nodes/execnodes.h"

extern PGDLLIMPORT SubPlanState *ExecInitSubPlan(SubPlan *subplan, PlanState *parent);

extern PGDLLIMPORT Datum ExecSubPlan(SubPlanState *node, ExprContext *econtext, bool *isNull);

extern PGDLLIMPORT void ExecReScanSetParamPlan(SubPlanState *node, PlanState *parent);

extern PGDLLIMPORT void ExecSetParamPlan(SubPlanState *node, ExprContext *econtext);

extern PGDLLIMPORT void ExecSetParamPlanMulti(const Bitmapset *params, ExprContext *econtext);

#endif							/* NODESUBPLAN_H */
