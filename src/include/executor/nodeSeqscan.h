/*-------------------------------------------------------------------------
 *
 * nodeSeqscan.h
 *
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/executor/nodeSeqscan.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef NODESEQSCAN_H
#define NODESEQSCAN_H

#include "access/parallel.h"
#include "nodes/execnodes.h"

extern PGDLLIMPORT SeqScanState *ExecInitSeqScan(SeqScan *node, EState *estate, int eflags);
extern PGDLLIMPORT void ExecEndSeqScan(SeqScanState *node);
extern PGDLLIMPORT void ExecReScanSeqScan(SeqScanState *node);

/* parallel scan support */
extern PGDLLIMPORT void ExecSeqScanEstimate(SeqScanState *node, ParallelContext *pcxt);
extern PGDLLIMPORT void ExecSeqScanInitializeDSM(SeqScanState *node, ParallelContext *pcxt);
extern PGDLLIMPORT void ExecSeqScanReInitializeDSM(SeqScanState *node, ParallelContext *pcxt);
extern PGDLLIMPORT void ExecSeqScanInitializeWorker(SeqScanState *node,
										ParallelWorkerContext *pwcxt);

#endif							/* NODESEQSCAN_H */
