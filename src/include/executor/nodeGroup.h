/*-------------------------------------------------------------------------
 *
 * nodeGroup.h
 *	  prototypes for nodeGroup.c
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/executor/nodeGroup.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef NODEGROUP_H
#define NODEGROUP_H

#include "nodes/execnodes.h"

extern PGDLLIMPORT GroupState *ExecInitGroup(Group *node, EState *estate, int eflags);
extern PGDLLIMPORT void ExecEndGroup(GroupState *node);
extern PGDLLIMPORT void ExecReScanGroup(GroupState *node);

#endif							/* NODEGROUP_H */
