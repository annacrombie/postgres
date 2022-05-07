/*-------------------------------------------------------------------------
 *
 * nodeResult.h
 *
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/executor/nodeResult.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef NODERESULT_H
#define NODERESULT_H

#include "nodes/execnodes.h"

extern PGDLLIMPORT ResultState *ExecInitResult(Result *node, EState *estate, int eflags);
extern PGDLLIMPORT void ExecEndResult(ResultState *node);
extern PGDLLIMPORT void ExecResultMarkPos(ResultState *node);
extern PGDLLIMPORT void ExecResultRestrPos(ResultState *node);
extern PGDLLIMPORT void ExecReScanResult(ResultState *node);

#endif							/* NODERESULT_H */
