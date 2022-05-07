/*-------------------------------------------------------------------------
 *
 * nodeMemoize.h
 *
 *
 *
 * Portions Copyright (c) 2021-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/executor/nodeMemoize.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef NODEMEMOIZE_H
#define NODEMEMOIZE_H

#include "access/parallel.h"
#include "nodes/execnodes.h"

extern PGDLLIMPORT MemoizeState *ExecInitMemoize(Memoize *node, EState *estate, int eflags);
extern PGDLLIMPORT void ExecEndMemoize(MemoizeState *node);
extern PGDLLIMPORT void ExecReScanMemoize(MemoizeState *node);
extern PGDLLIMPORT double ExecEstimateCacheEntryOverheadBytes(double ntuples);
extern PGDLLIMPORT void ExecMemoizeEstimate(MemoizeState *node,
								ParallelContext *pcxt);
extern PGDLLIMPORT void ExecMemoizeInitializeDSM(MemoizeState *node,
									 ParallelContext *pcxt);
extern PGDLLIMPORT void ExecMemoizeInitializeWorker(MemoizeState *node,
										ParallelWorkerContext *pwcxt);
extern PGDLLIMPORT void ExecMemoizeRetrieveInstrumentation(MemoizeState *node);

#endif							/* NODEMEMOIZE_H */
