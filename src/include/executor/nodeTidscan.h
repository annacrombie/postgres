/*-------------------------------------------------------------------------
 *
 * nodeTidscan.h
 *
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/executor/nodeTidscan.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef NODETIDSCAN_H
#define NODETIDSCAN_H

#include "nodes/execnodes.h"

extern PGDLLIMPORT TidScanState *ExecInitTidScan(TidScan *node, EState *estate, int eflags);
extern PGDLLIMPORT void ExecEndTidScan(TidScanState *node);
extern PGDLLIMPORT void ExecReScanTidScan(TidScanState *node);

#endif							/* NODETIDSCAN_H */
