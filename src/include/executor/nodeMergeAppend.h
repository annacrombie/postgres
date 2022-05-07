/*-------------------------------------------------------------------------
 *
 * nodeMergeAppend.h
 *
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/executor/nodeMergeAppend.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef NODEMERGEAPPEND_H
#define NODEMERGEAPPEND_H

#include "nodes/execnodes.h"

extern PGDLLIMPORT MergeAppendState *ExecInitMergeAppend(MergeAppend *node, EState *estate, int eflags);
extern PGDLLIMPORT void ExecEndMergeAppend(MergeAppendState *node);
extern PGDLLIMPORT void ExecReScanMergeAppend(MergeAppendState *node);

#endif							/* NODEMERGEAPPEND_H */
