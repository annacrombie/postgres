/*-------------------------------------------------------------------------
 *
 * nodeGather.h
 *		prototypes for nodeGather.c
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/executor/nodeGather.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef NODEGATHER_H
#define NODEGATHER_H

#include "nodes/execnodes.h"

extern PGDLLIMPORT GatherState *ExecInitGather(Gather *node, EState *estate, int eflags);
extern PGDLLIMPORT void ExecEndGather(GatherState *node);
extern PGDLLIMPORT void ExecShutdownGather(GatherState *node);
extern PGDLLIMPORT void ExecReScanGather(GatherState *node);

#endif							/* NODEGATHER_H */
