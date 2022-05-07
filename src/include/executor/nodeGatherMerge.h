/*-------------------------------------------------------------------------
 *
 * nodeGatherMerge.h
 *		prototypes for nodeGatherMerge.c
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/executor/nodeGatherMerge.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef NODEGATHERMERGE_H
#define NODEGATHERMERGE_H

#include "nodes/execnodes.h"

extern PGDLLIMPORT GatherMergeState *ExecInitGatherMerge(GatherMerge *node,
											 EState *estate,
											 int eflags);
extern PGDLLIMPORT void ExecEndGatherMerge(GatherMergeState *node);
extern PGDLLIMPORT void ExecReScanGatherMerge(GatherMergeState *node);
extern PGDLLIMPORT void ExecShutdownGatherMerge(GatherMergeState *node);

#endif							/* NODEGATHERMERGE_H */
