/*-------------------------------------------------------------------------
 * origin.h
 *	   Exports from replication/logical/origin.c
 *
 * Copyright (c) 2013-2022, PostgreSQL Global Development Group
 *
 * src/include/replication/origin.h
 *-------------------------------------------------------------------------
 */
#ifndef PG_ORIGIN_H
#define PG_ORIGIN_H

#include "access/xlog.h"
#include "access/xlogdefs.h"
#include "access/xlogreader.h"
#include "catalog/pg_replication_origin.h"

typedef struct xl_replorigin_set
{
	XLogRecPtr	remote_lsn;
	RepOriginId node_id;
	bool		force;
} xl_replorigin_set;

typedef struct xl_replorigin_drop
{
	RepOriginId node_id;
} xl_replorigin_drop;

#define XLOG_REPLORIGIN_SET		0x00
#define XLOG_REPLORIGIN_DROP		0x10

#define InvalidRepOriginId 0
#define DoNotReplicateId PG_UINT16_MAX

extern PGDLLIMPORT RepOriginId replorigin_session_origin;
extern PGDLLIMPORT XLogRecPtr replorigin_session_origin_lsn;
extern PGDLLIMPORT TimestampTz replorigin_session_origin_timestamp;

/* API for querying & manipulating replication origins */
extern PGDLLIMPORT RepOriginId replorigin_by_name(const char *name, bool missing_ok);
extern PGDLLIMPORT RepOriginId replorigin_create(const char *name);
extern PGDLLIMPORT void replorigin_drop_by_name(const char *name, bool missing_ok, bool nowait);
extern PGDLLIMPORT bool replorigin_by_oid(RepOriginId roident, bool missing_ok,
							  char **roname);

/* API for querying & manipulating replication progress tracking */
extern PGDLLIMPORT void replorigin_advance(RepOriginId node,
							   XLogRecPtr remote_commit,
							   XLogRecPtr local_commit,
							   bool go_backward, bool wal_log);
extern PGDLLIMPORT XLogRecPtr replorigin_get_progress(RepOriginId node, bool flush);

extern PGDLLIMPORT void replorigin_session_advance(XLogRecPtr remote_commit,
									   XLogRecPtr local_commit);
extern PGDLLIMPORT void replorigin_session_setup(RepOriginId node);
extern PGDLLIMPORT void replorigin_session_reset(void);
extern PGDLLIMPORT XLogRecPtr replorigin_session_get_progress(bool flush);

/* Checkpoint/Startup integration */
extern PGDLLIMPORT void CheckPointReplicationOrigin(void);
extern PGDLLIMPORT void StartupReplicationOrigin(void);

/* WAL logging */
extern PGDLLIMPORT void replorigin_redo(XLogReaderState *record);
extern PGDLLIMPORT void replorigin_desc(StringInfo buf, XLogReaderState *record);
extern PGDLLIMPORT const char *replorigin_identify(uint8 info);

/* shared memory allocation */
extern PGDLLIMPORT Size ReplicationOriginShmemSize(void);
extern PGDLLIMPORT void ReplicationOriginShmemInit(void);

#endif							/* PG_ORIGIN_H */
