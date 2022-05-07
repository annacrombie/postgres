/*-------------------------------------------------------------------------
 *
 * nodeSort.h
 *
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/executor/nodeSort.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef NODESORT_H
#define NODESORT_H

#include "access/parallel.h"
#include "nodes/execnodes.h"

extern PGDLLIMPORT SortState *ExecInitSort(Sort *node, EState *estate, int eflags);
extern PGDLLIMPORT void ExecEndSort(SortState *node);
extern PGDLLIMPORT void ExecSortMarkPos(SortState *node);
extern PGDLLIMPORT void ExecSortRestrPos(SortState *node);
extern PGDLLIMPORT void ExecReScanSort(SortState *node);

/* parallel instrumentation support */
extern PGDLLIMPORT void ExecSortEstimate(SortState *node, ParallelContext *pcxt);
extern PGDLLIMPORT void ExecSortInitializeDSM(SortState *node, ParallelContext *pcxt);
extern PGDLLIMPORT void ExecSortInitializeWorker(SortState *node, ParallelWorkerContext *pwcxt);
extern PGDLLIMPORT void ExecSortRetrieveInstrumentation(SortState *node);

#endif							/* NODESORT_H */
