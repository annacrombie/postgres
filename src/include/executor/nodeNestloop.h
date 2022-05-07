/*-------------------------------------------------------------------------
 *
 * nodeNestloop.h
 *
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/executor/nodeNestloop.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef NODENESTLOOP_H
#define NODENESTLOOP_H

#include "nodes/execnodes.h"

extern PGDLLIMPORT NestLoopState *ExecInitNestLoop(NestLoop *node, EState *estate, int eflags);
extern PGDLLIMPORT void ExecEndNestLoop(NestLoopState *node);
extern PGDLLIMPORT void ExecReScanNestLoop(NestLoopState *node);

#endif							/* NODENESTLOOP_H */
