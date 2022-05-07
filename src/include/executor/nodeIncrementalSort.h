/*-------------------------------------------------------------------------
 *
 * nodeIncrementalSort.h
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/executor/nodeIncrementalSort.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef NODEINCREMENTALSORT_H
#define NODEINCREMENTALSORT_H

#include "access/parallel.h"
#include "nodes/execnodes.h"

extern PGDLLIMPORT IncrementalSortState *ExecInitIncrementalSort(IncrementalSort *node, EState *estate, int eflags);
extern PGDLLIMPORT void ExecEndIncrementalSort(IncrementalSortState *node);
extern PGDLLIMPORT void ExecReScanIncrementalSort(IncrementalSortState *node);

/* parallel instrumentation support */
extern PGDLLIMPORT void ExecIncrementalSortEstimate(IncrementalSortState *node, ParallelContext *pcxt);
extern PGDLLIMPORT void ExecIncrementalSortInitializeDSM(IncrementalSortState *node, ParallelContext *pcxt);
extern PGDLLIMPORT void ExecIncrementalSortInitializeWorker(IncrementalSortState *node, ParallelWorkerContext *pcxt);
extern PGDLLIMPORT void ExecIncrementalSortRetrieveInstrumentation(IncrementalSortState *node);

#endif							/* NODEINCREMENTALSORT_H */
