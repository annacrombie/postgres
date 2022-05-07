/*-------------------------------------------------------------------------
 *
 * nodeHash.h
 *	  prototypes for nodeHash.c
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/executor/nodeHash.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef NODEHASH_H
#define NODEHASH_H

#include "access/parallel.h"
#include "nodes/execnodes.h"

struct SharedHashJoinBatch;

extern PGDLLIMPORT HashState *ExecInitHash(Hash *node, EState *estate, int eflags);
extern PGDLLIMPORT Node *MultiExecHash(HashState *node);
extern PGDLLIMPORT void ExecEndHash(HashState *node);
extern PGDLLIMPORT void ExecReScanHash(HashState *node);

extern PGDLLIMPORT HashJoinTable ExecHashTableCreate(HashState *state, List *hashOperators, List *hashCollations,
										 bool keepNulls);
extern PGDLLIMPORT void ExecParallelHashTableAlloc(HashJoinTable hashtable,
									   int batchno);
extern PGDLLIMPORT void ExecHashTableDestroy(HashJoinTable hashtable);
extern PGDLLIMPORT void ExecHashTableDetach(HashJoinTable hashtable);
extern PGDLLIMPORT void ExecHashTableDetachBatch(HashJoinTable hashtable);
extern PGDLLIMPORT void ExecParallelHashTableSetCurrentBatch(HashJoinTable hashtable,
												 int batchno);

extern PGDLLIMPORT void ExecHashTableInsert(HashJoinTable hashtable,
								TupleTableSlot *slot,
								uint32 hashvalue);
extern PGDLLIMPORT void ExecParallelHashTableInsert(HashJoinTable hashtable,
										TupleTableSlot *slot,
										uint32 hashvalue);
extern PGDLLIMPORT void ExecParallelHashTableInsertCurrentBatch(HashJoinTable hashtable,
													TupleTableSlot *slot,
													uint32 hashvalue);
extern PGDLLIMPORT bool ExecHashGetHashValue(HashJoinTable hashtable,
								 ExprContext *econtext,
								 List *hashkeys,
								 bool outer_tuple,
								 bool keep_nulls,
								 uint32 *hashvalue);
extern PGDLLIMPORT void ExecHashGetBucketAndBatch(HashJoinTable hashtable,
									  uint32 hashvalue,
									  int *bucketno,
									  int *batchno);
extern PGDLLIMPORT bool ExecScanHashBucket(HashJoinState *hjstate, ExprContext *econtext);
extern PGDLLIMPORT bool ExecParallelScanHashBucket(HashJoinState *hjstate, ExprContext *econtext);
extern PGDLLIMPORT void ExecPrepHashTableForUnmatched(HashJoinState *hjstate);
extern PGDLLIMPORT bool ExecScanHashTableForUnmatched(HashJoinState *hjstate,
										  ExprContext *econtext);
extern PGDLLIMPORT void ExecHashTableReset(HashJoinTable hashtable);
extern PGDLLIMPORT void ExecHashTableResetMatchFlags(HashJoinTable hashtable);
extern PGDLLIMPORT void ExecChooseHashTableSize(double ntuples, int tupwidth, bool useskew,
									bool try_combined_hash_mem,
									int parallel_workers,
									size_t *space_allowed,
									int *numbuckets,
									int *numbatches,
									int *num_skew_mcvs);
extern PGDLLIMPORT int	ExecHashGetSkewBucket(HashJoinTable hashtable, uint32 hashvalue);
extern PGDLLIMPORT void ExecHashEstimate(HashState *node, ParallelContext *pcxt);
extern PGDLLIMPORT void ExecHashInitializeDSM(HashState *node, ParallelContext *pcxt);
extern PGDLLIMPORT void ExecHashInitializeWorker(HashState *node, ParallelWorkerContext *pwcxt);
extern PGDLLIMPORT void ExecHashRetrieveInstrumentation(HashState *node);
extern PGDLLIMPORT void ExecShutdownHash(HashState *node);
extern PGDLLIMPORT void ExecHashAccumInstrumentation(HashInstrumentation *instrument,
										 HashJoinTable hashtable);

#endif							/* NODEHASH_H */
