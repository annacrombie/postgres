/*-------------------------------------------------------------------------
 *
 * nodeSetOp.h
 *
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/executor/nodeSetOp.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef NODESETOP_H
#define NODESETOP_H

#include "nodes/execnodes.h"

extern PGDLLIMPORT SetOpState *ExecInitSetOp(SetOp *node, EState *estate, int eflags);
extern PGDLLIMPORT void ExecEndSetOp(SetOpState *node);
extern PGDLLIMPORT void ExecReScanSetOp(SetOpState *node);

#endif							/* NODESETOP_H */
