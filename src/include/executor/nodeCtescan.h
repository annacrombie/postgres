/*-------------------------------------------------------------------------
 *
 * nodeCtescan.h
 *
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/executor/nodeCtescan.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef NODECTESCAN_H
#define NODECTESCAN_H

#include "nodes/execnodes.h"

extern PGDLLIMPORT CteScanState *ExecInitCteScan(CteScan *node, EState *estate, int eflags);
extern PGDLLIMPORT void ExecEndCteScan(CteScanState *node);
extern PGDLLIMPORT void ExecReScanCteScan(CteScanState *node);

#endif							/* NODECTESCAN_H */
