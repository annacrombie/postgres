/*-------------------------------------------------------------------------
 *
 * nodeProjectSet.h
 *
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/executor/nodeProjectSet.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef NODEPROJECTSET_H
#define NODEPROJECTSET_H

#include "nodes/execnodes.h"

extern PGDLLIMPORT ProjectSetState *ExecInitProjectSet(ProjectSet *node, EState *estate, int eflags);
extern PGDLLIMPORT void ExecEndProjectSet(ProjectSetState *node);
extern PGDLLIMPORT void ExecReScanProjectSet(ProjectSetState *node);

#endif							/* NODEPROJECTSET_H */
