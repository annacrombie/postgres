/*-------------------------------------------------------------------------
 *
 * logicalworker.h
 *	  Exports for logical replication workers.
 *
 * Portions Copyright (c) 2016-2022, PostgreSQL Global Development Group
 *
 * src/include/replication/logicalworker.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef LOGICALWORKER_H
#define LOGICALWORKER_H

extern PGDLLIMPORT void ApplyWorkerMain(Datum main_arg);

extern PGDLLIMPORT bool IsLogicalWorker(void);

#endif							/* LOGICALWORKER_H */
