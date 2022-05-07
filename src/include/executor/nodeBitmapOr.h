/*-------------------------------------------------------------------------
 *
 * nodeBitmapOr.h
 *
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/executor/nodeBitmapOr.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef NODEBITMAPOR_H
#define NODEBITMAPOR_H

#include "nodes/execnodes.h"

extern PGDLLIMPORT BitmapOrState *ExecInitBitmapOr(BitmapOr *node, EState *estate, int eflags);
extern PGDLLIMPORT Node *MultiExecBitmapOr(BitmapOrState *node);
extern PGDLLIMPORT void ExecEndBitmapOr(BitmapOrState *node);
extern PGDLLIMPORT void ExecReScanBitmapOr(BitmapOrState *node);

#endif							/* NODEBITMAPOR_H */
