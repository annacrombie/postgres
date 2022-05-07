/*-------------------------------------------------------------------------
 * decode.h
 *	   PostgreSQL WAL to logical transformation
 *
 * Portions Copyright (c) 2012-2022, PostgreSQL Global Development Group
 *
 *-------------------------------------------------------------------------
 */
#ifndef DECODE_H
#define DECODE_H

#include "access/xlogreader.h"
#include "access/xlogrecord.h"
#include "replication/logical.h"
#include "replication/reorderbuffer.h"

typedef struct XLogRecordBuffer
{
	XLogRecPtr	origptr;
	XLogRecPtr	endptr;
	XLogReaderState *record;
} XLogRecordBuffer;

extern PGDLLIMPORT void xlog_decode(LogicalDecodingContext *ctx, XLogRecordBuffer *buf);
extern PGDLLIMPORT void heap_decode(LogicalDecodingContext *ctx, XLogRecordBuffer *buf);
extern PGDLLIMPORT void heap2_decode(LogicalDecodingContext *ctx, XLogRecordBuffer *buf);
extern PGDLLIMPORT void xact_decode(LogicalDecodingContext *ctx, XLogRecordBuffer *buf);
extern PGDLLIMPORT void standby_decode(LogicalDecodingContext *ctx, XLogRecordBuffer *buf);
extern PGDLLIMPORT void logicalmsg_decode(LogicalDecodingContext *ctx, XLogRecordBuffer *buf);

extern PGDLLIMPORT void	LogicalDecodingProcessRecord(LogicalDecodingContext *ctx,
										 XLogReaderState *record);

#endif
