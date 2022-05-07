/*-------------------------------------------------------------------------
 *
 * xlogprefetcher.h
 *		Declarations for the recovery prefetching module.
 *
 * Portions Copyright (c) 2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * IDENTIFICATION
 *		src/include/access/xlogprefetcher.h
 *-------------------------------------------------------------------------
 */
#ifndef XLOGPREFETCHER_H
#define XLOGPREFETCHER_H

#include "access/xlogdefs.h"
#include "access/xlogreader.h"
#include "access/xlogrecord.h"

/* GUCs */
extern PGDLLIMPORT int recovery_prefetch;

/* Possible values for recovery_prefetch */
typedef enum
{
	RECOVERY_PREFETCH_OFF,
	RECOVERY_PREFETCH_ON,
	RECOVERY_PREFETCH_TRY
}			RecoveryPrefetchValue;

struct XLogPrefetcher;
typedef struct XLogPrefetcher XLogPrefetcher;


extern PGDLLIMPORT void XLogPrefetchReconfigure(void);

extern PGDLLIMPORT size_t XLogPrefetchShmemSize(void);
extern PGDLLIMPORT void XLogPrefetchShmemInit(void);

extern PGDLLIMPORT void XLogPrefetchResetStats(void);

extern PGDLLIMPORT XLogPrefetcher *XLogPrefetcherAllocate(XLogReaderState *reader);
extern PGDLLIMPORT void XLogPrefetcherFree(XLogPrefetcher *prefetcher);

extern PGDLLIMPORT XLogReaderState *XLogPrefetcherGetReader(XLogPrefetcher *prefetcher);

extern PGDLLIMPORT void XLogPrefetcherBeginRead(XLogPrefetcher *prefetcher,
									XLogRecPtr recPtr);

extern PGDLLIMPORT XLogRecord *XLogPrefetcherReadRecord(XLogPrefetcher *prefetcher,
											char **errmsg);

extern PGDLLIMPORT void XLogPrefetcherComputeStats(XLogPrefetcher *prefetcher);

#endif
