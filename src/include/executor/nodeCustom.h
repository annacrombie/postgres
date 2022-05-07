/* ------------------------------------------------------------------------
 *
 * nodeCustom.h
 *
 * prototypes for CustomScan nodes
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * ------------------------------------------------------------------------
 */
#ifndef NODECUSTOM_H
#define NODECUSTOM_H

#include "access/parallel.h"
#include "nodes/execnodes.h"

/*
 * General executor code
 */
extern PGDLLIMPORT CustomScanState *ExecInitCustomScan(CustomScan *cscan,
										   EState *estate, int eflags);
extern PGDLLIMPORT void ExecEndCustomScan(CustomScanState *node);

extern PGDLLIMPORT void ExecReScanCustomScan(CustomScanState *node);
extern PGDLLIMPORT void ExecCustomMarkPos(CustomScanState *node);
extern PGDLLIMPORT void ExecCustomRestrPos(CustomScanState *node);

/*
 * Parallel execution support
 */
extern PGDLLIMPORT void ExecCustomScanEstimate(CustomScanState *node,
								   ParallelContext *pcxt);
extern PGDLLIMPORT void ExecCustomScanInitializeDSM(CustomScanState *node,
										ParallelContext *pcxt);
extern PGDLLIMPORT void ExecCustomScanReInitializeDSM(CustomScanState *node,
										  ParallelContext *pcxt);
extern PGDLLIMPORT void ExecCustomScanInitializeWorker(CustomScanState *node,
										   ParallelWorkerContext *pwcxt);
extern PGDLLIMPORT void ExecShutdownCustomScan(CustomScanState *node);

#endif							/* NODECUSTOM_H */
