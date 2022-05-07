/*-------------------------------------------------------------------------
 *
 * nodeHashjoin.h
 *	  prototypes for nodeHashjoin.c
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/executor/nodeHashjoin.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef NODEHASHJOIN_H
#define NODEHASHJOIN_H

#include "access/parallel.h"
#include "nodes/execnodes.h"
#include "storage/buffile.h"

extern PGDLLIMPORT HashJoinState *ExecInitHashJoin(HashJoin *node, EState *estate, int eflags);
extern PGDLLIMPORT void ExecEndHashJoin(HashJoinState *node);
extern PGDLLIMPORT void ExecReScanHashJoin(HashJoinState *node);
extern PGDLLIMPORT void ExecShutdownHashJoin(HashJoinState *node);
extern PGDLLIMPORT void ExecHashJoinEstimate(HashJoinState *state, ParallelContext *pcxt);
extern PGDLLIMPORT void ExecHashJoinInitializeDSM(HashJoinState *state, ParallelContext *pcxt);
extern PGDLLIMPORT void ExecHashJoinReInitializeDSM(HashJoinState *state, ParallelContext *pcxt);
extern PGDLLIMPORT void ExecHashJoinInitializeWorker(HashJoinState *state,
										 ParallelWorkerContext *pwcxt);

extern PGDLLIMPORT void ExecHashJoinSaveTuple(MinimalTuple tuple, uint32 hashvalue,
								  BufFile **fileptr);

#endif							/* NODEHASHJOIN_H */
