/*-------------------------------------------------------------------------
 *
 * nodeUnique.h
 *
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/executor/nodeUnique.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef NODEUNIQUE_H
#define NODEUNIQUE_H

#include "nodes/execnodes.h"

extern PGDLLIMPORT UniqueState *ExecInitUnique(Unique *node, EState *estate, int eflags);
extern PGDLLIMPORT void ExecEndUnique(UniqueState *node);
extern PGDLLIMPORT void ExecReScanUnique(UniqueState *node);

#endif							/* NODEUNIQUE_H */
