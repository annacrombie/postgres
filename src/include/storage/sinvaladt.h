/*-------------------------------------------------------------------------
 *
 * sinvaladt.h
 *	  POSTGRES shared cache invalidation data manager.
 *
 * The shared cache invalidation manager is responsible for transmitting
 * invalidation messages between backends.  Any message sent by any backend
 * must be delivered to all already-running backends before it can be
 * forgotten.  (If we run out of space, we instead deliver a "RESET"
 * message to backends that have fallen too far behind.)
 *
 * The struct type SharedInvalidationMessage, defining the contents of
 * a single message, is defined in sinval.h.
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/storage/sinvaladt.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef SINVALADT_H
#define SINVALADT_H

#include "storage/lock.h"
#include "storage/sinval.h"

/*
 * prototypes for functions in sinvaladt.c
 */
extern PGDLLIMPORT Size SInvalShmemSize(void);
extern PGDLLIMPORT void CreateSharedInvalidationState(void);
extern PGDLLIMPORT void SharedInvalBackendInit(bool sendOnly);
extern PGDLLIMPORT PGPROC *BackendIdGetProc(int backendID);
extern PGDLLIMPORT void BackendIdGetTransactionIds(int backendID, TransactionId *xid, TransactionId *xmin);

extern PGDLLIMPORT void SIInsertDataEntries(const SharedInvalidationMessage *data, int n);
extern PGDLLIMPORT int	SIGetDataEntries(SharedInvalidationMessage *data, int datasize);
extern PGDLLIMPORT void SICleanupQueue(bool callerHasWriteLock, int minFree);

extern PGDLLIMPORT LocalTransactionId GetNextLocalTransactionId(void);

#endif							/* SINVALADT_H */
