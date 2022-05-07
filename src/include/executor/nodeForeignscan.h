/*-------------------------------------------------------------------------
 *
 * nodeForeignscan.h
 *
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/executor/nodeForeignscan.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef NODEFOREIGNSCAN_H
#define NODEFOREIGNSCAN_H

#include "access/parallel.h"
#include "nodes/execnodes.h"

extern PGDLLIMPORT ForeignScanState *ExecInitForeignScan(ForeignScan *node, EState *estate, int eflags);
extern PGDLLIMPORT void ExecEndForeignScan(ForeignScanState *node);
extern PGDLLIMPORT void ExecReScanForeignScan(ForeignScanState *node);

extern PGDLLIMPORT void ExecForeignScanEstimate(ForeignScanState *node,
									ParallelContext *pcxt);
extern PGDLLIMPORT void ExecForeignScanInitializeDSM(ForeignScanState *node,
										 ParallelContext *pcxt);
extern PGDLLIMPORT void ExecForeignScanReInitializeDSM(ForeignScanState *node,
										   ParallelContext *pcxt);
extern PGDLLIMPORT void ExecForeignScanInitializeWorker(ForeignScanState *node,
											ParallelWorkerContext *pwcxt);
extern PGDLLIMPORT void ExecShutdownForeignScan(ForeignScanState *node);

extern PGDLLIMPORT void ExecAsyncForeignScanRequest(AsyncRequest *areq);
extern PGDLLIMPORT void ExecAsyncForeignScanConfigureWait(AsyncRequest *areq);
extern PGDLLIMPORT void ExecAsyncForeignScanNotify(AsyncRequest *areq);

#endif							/* NODEFOREIGNSCAN_H */
