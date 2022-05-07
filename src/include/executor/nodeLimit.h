/*-------------------------------------------------------------------------
 *
 * nodeLimit.h
 *
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/executor/nodeLimit.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef NODELIMIT_H
#define NODELIMIT_H

#include "nodes/execnodes.h"

extern PGDLLIMPORT LimitState *ExecInitLimit(Limit *node, EState *estate, int eflags);
extern PGDLLIMPORT void ExecEndLimit(LimitState *node);
extern PGDLLIMPORT void ExecReScanLimit(LimitState *node);

#endif							/* NODELIMIT_H */
