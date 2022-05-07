/*-------------------------------------------------------------------------
 *
 * nodeBitmapAnd.h
 *
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/executor/nodeBitmapAnd.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef NODEBITMAPAND_H
#define NODEBITMAPAND_H

#include "nodes/execnodes.h"

extern PGDLLIMPORT BitmapAndState *ExecInitBitmapAnd(BitmapAnd *node, EState *estate, int eflags);
extern PGDLLIMPORT Node *MultiExecBitmapAnd(BitmapAndState *node);
extern PGDLLIMPORT void ExecEndBitmapAnd(BitmapAndState *node);
extern PGDLLIMPORT void ExecReScanBitmapAnd(BitmapAndState *node);

#endif							/* NODEBITMAPAND_H */
