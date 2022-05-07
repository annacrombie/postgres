/*-------------------------------------------------------------------------
 *
 * nodeIndexonlyscan.h
 *
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/executor/nodeIndexonlyscan.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef NODEINDEXONLYSCAN_H
#define NODEINDEXONLYSCAN_H

#include "access/parallel.h"
#include "nodes/execnodes.h"

extern PGDLLIMPORT IndexOnlyScanState *ExecInitIndexOnlyScan(IndexOnlyScan *node, EState *estate, int eflags);
extern PGDLLIMPORT void ExecEndIndexOnlyScan(IndexOnlyScanState *node);
extern PGDLLIMPORT void ExecIndexOnlyMarkPos(IndexOnlyScanState *node);
extern PGDLLIMPORT void ExecIndexOnlyRestrPos(IndexOnlyScanState *node);
extern PGDLLIMPORT void ExecReScanIndexOnlyScan(IndexOnlyScanState *node);

/* Support functions for parallel index-only scans */
extern PGDLLIMPORT void ExecIndexOnlyScanEstimate(IndexOnlyScanState *node,
									  ParallelContext *pcxt);
extern PGDLLIMPORT void ExecIndexOnlyScanInitializeDSM(IndexOnlyScanState *node,
										   ParallelContext *pcxt);
extern PGDLLIMPORT void ExecIndexOnlyScanReInitializeDSM(IndexOnlyScanState *node,
											 ParallelContext *pcxt);
extern PGDLLIMPORT void ExecIndexOnlyScanInitializeWorker(IndexOnlyScanState *node,
											  ParallelWorkerContext *pwcxt);

#endif							/* NODEINDEXONLYSCAN_H */
