/*
 * commit_ts.h
 *
 * PostgreSQL commit timestamp manager
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/access/commit_ts.h
 */
#ifndef COMMIT_TS_H
#define COMMIT_TS_H

#include "access/xlog.h"
#include "datatype/timestamp.h"
#include "replication/origin.h"
#include "storage/sync.h"


extern PGDLLIMPORT bool track_commit_timestamp;

extern PGDLLIMPORT void TransactionTreeSetCommitTsData(TransactionId xid, int nsubxids,
										   TransactionId *subxids, TimestampTz timestamp,
										   RepOriginId nodeid);
extern PGDLLIMPORT bool TransactionIdGetCommitTsData(TransactionId xid,
										 TimestampTz *ts, RepOriginId *nodeid);
extern PGDLLIMPORT TransactionId GetLatestCommitTsData(TimestampTz *ts,
										   RepOriginId *nodeid);

extern PGDLLIMPORT Size CommitTsShmemBuffers(void);
extern PGDLLIMPORT Size CommitTsShmemSize(void);
extern PGDLLIMPORT void CommitTsShmemInit(void);
extern PGDLLIMPORT void BootStrapCommitTs(void);
extern PGDLLIMPORT void StartupCommitTs(void);
extern PGDLLIMPORT void CommitTsParameterChange(bool newvalue, bool oldvalue);
extern PGDLLIMPORT void CompleteCommitTsInitialization(void);
extern PGDLLIMPORT void CheckPointCommitTs(void);
extern PGDLLIMPORT void ExtendCommitTs(TransactionId newestXact);
extern PGDLLIMPORT void TruncateCommitTs(TransactionId oldestXact);
extern PGDLLIMPORT void SetCommitTsLimit(TransactionId oldestXact,
							 TransactionId newestXact);
extern PGDLLIMPORT void AdvanceOldestCommitTsXid(TransactionId oldestXact);

extern PGDLLIMPORT int	committssyncfiletag(const FileTag *ftag, char *path);

/* XLOG stuff */
#define COMMIT_TS_ZEROPAGE		0x00
#define COMMIT_TS_TRUNCATE		0x10

typedef struct xl_commit_ts_set
{
	TimestampTz timestamp;
	RepOriginId nodeid;
	TransactionId mainxid;
	/* subxact Xids follow */
}			xl_commit_ts_set;

#define SizeOfCommitTsSet	(offsetof(xl_commit_ts_set, mainxid) + \
							 sizeof(TransactionId))

typedef struct xl_commit_ts_truncate
{
	int			pageno;
	TransactionId oldestXid;
} xl_commit_ts_truncate;

#define SizeOfCommitTsTruncate	(offsetof(xl_commit_ts_truncate, oldestXid) + \
								 sizeof(TransactionId))

extern PGDLLIMPORT void commit_ts_redo(XLogReaderState *record);
extern PGDLLIMPORT void commit_ts_desc(StringInfo buf, XLogReaderState *record);
extern PGDLLIMPORT const char *commit_ts_identify(uint8 info);

#endif							/* COMMIT_TS_H */
