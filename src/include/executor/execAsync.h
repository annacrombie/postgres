/*-------------------------------------------------------------------------
 * execAsync.h
 *		Support functions for asynchronous execution
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * IDENTIFICATION
 *		src/include/executor/execAsync.h
 *-------------------------------------------------------------------------
 */

#ifndef EXECASYNC_H
#define EXECASYNC_H

#include "nodes/execnodes.h"

extern PGDLLIMPORT void ExecAsyncRequest(AsyncRequest *areq);
extern PGDLLIMPORT void ExecAsyncConfigureWait(AsyncRequest *areq);
extern PGDLLIMPORT void ExecAsyncNotify(AsyncRequest *areq);
extern PGDLLIMPORT void ExecAsyncResponse(AsyncRequest *areq);
extern PGDLLIMPORT void ExecAsyncRequestDone(AsyncRequest *areq, TupleTableSlot *result);
extern PGDLLIMPORT void ExecAsyncRequestPending(AsyncRequest *areq);

#endif							/* EXECASYNC_H */
