/*-------------------------------------------------------------------------
 *
 * nodeIndexscan.h
 *
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/executor/nodeIndexscan.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef NODEINDEXSCAN_H
#define NODEINDEXSCAN_H

#include "access/genam.h"
#include "access/parallel.h"
#include "nodes/execnodes.h"

extern PGDLLIMPORT IndexScanState *ExecInitIndexScan(IndexScan *node, EState *estate, int eflags);
extern PGDLLIMPORT void ExecEndIndexScan(IndexScanState *node);
extern PGDLLIMPORT void ExecIndexMarkPos(IndexScanState *node);
extern PGDLLIMPORT void ExecIndexRestrPos(IndexScanState *node);
extern PGDLLIMPORT void ExecReScanIndexScan(IndexScanState *node);
extern PGDLLIMPORT void ExecIndexScanEstimate(IndexScanState *node, ParallelContext *pcxt);
extern PGDLLIMPORT void ExecIndexScanInitializeDSM(IndexScanState *node, ParallelContext *pcxt);
extern PGDLLIMPORT void ExecIndexScanReInitializeDSM(IndexScanState *node, ParallelContext *pcxt);
extern PGDLLIMPORT void ExecIndexScanInitializeWorker(IndexScanState *node,
										  ParallelWorkerContext *pwcxt);

/*
 * These routines are exported to share code with nodeIndexonlyscan.c and
 * nodeBitmapIndexscan.c
 */
extern PGDLLIMPORT void ExecIndexBuildScanKeys(PlanState *planstate, Relation index,
								   List *quals, bool isorderby,
								   ScanKey *scanKeys, int *numScanKeys,
								   IndexRuntimeKeyInfo **runtimeKeys, int *numRuntimeKeys,
								   IndexArrayKeyInfo **arrayKeys, int *numArrayKeys);
extern PGDLLIMPORT void ExecIndexEvalRuntimeKeys(ExprContext *econtext,
									 IndexRuntimeKeyInfo *runtimeKeys, int numRuntimeKeys);
extern PGDLLIMPORT bool ExecIndexEvalArrayKeys(ExprContext *econtext,
								   IndexArrayKeyInfo *arrayKeys, int numArrayKeys);
extern PGDLLIMPORT bool ExecIndexAdvanceArrayKeys(IndexArrayKeyInfo *arrayKeys, int numArrayKeys);

#endif							/* NODEINDEXSCAN_H */
