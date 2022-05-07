/*-------------------------------------------------------------------------
 *
 * nodeBitmapHeapscan.h
 *
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/executor/nodeBitmapHeapscan.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef NODEBITMAPHEAPSCAN_H
#define NODEBITMAPHEAPSCAN_H

#include "access/parallel.h"
#include "nodes/execnodes.h"

extern PGDLLIMPORT BitmapHeapScanState *ExecInitBitmapHeapScan(BitmapHeapScan *node, EState *estate, int eflags);
extern PGDLLIMPORT void ExecEndBitmapHeapScan(BitmapHeapScanState *node);
extern PGDLLIMPORT void ExecReScanBitmapHeapScan(BitmapHeapScanState *node);
extern PGDLLIMPORT void ExecBitmapHeapEstimate(BitmapHeapScanState *node,
								   ParallelContext *pcxt);
extern PGDLLIMPORT void ExecBitmapHeapInitializeDSM(BitmapHeapScanState *node,
										ParallelContext *pcxt);
extern PGDLLIMPORT void ExecBitmapHeapReInitializeDSM(BitmapHeapScanState *node,
										  ParallelContext *pcxt);
extern PGDLLIMPORT void ExecBitmapHeapInitializeWorker(BitmapHeapScanState *node,
										   ParallelWorkerContext *pwcxt);

#endif							/* NODEBITMAPHEAPSCAN_H */
