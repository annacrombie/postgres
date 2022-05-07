/*-------------------------------------------------------------------------
 *
 * nodeBitmapIndexscan.h
 *
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/executor/nodeBitmapIndexscan.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef NODEBITMAPINDEXSCAN_H
#define NODEBITMAPINDEXSCAN_H

#include "nodes/execnodes.h"

extern PGDLLIMPORT BitmapIndexScanState *ExecInitBitmapIndexScan(BitmapIndexScan *node, EState *estate, int eflags);
extern PGDLLIMPORT Node *MultiExecBitmapIndexScan(BitmapIndexScanState *node);
extern PGDLLIMPORT void ExecEndBitmapIndexScan(BitmapIndexScanState *node);
extern PGDLLIMPORT void ExecReScanBitmapIndexScan(BitmapIndexScanState *node);

#endif							/* NODEBITMAPINDEXSCAN_H */
