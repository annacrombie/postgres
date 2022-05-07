/*-------------------------------------------------------------------------
 *
 * bgwriter.h
 *	  Exports from postmaster/bgwriter.c and postmaster/checkpointer.c.
 *
 * The bgwriter process used to handle checkpointing duties too.  Now
 * there is a separate process, but we did not bother to split this header.
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 *
 * src/include/postmaster/bgwriter.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef _BGWRITER_H
#define _BGWRITER_H

#include "storage/block.h"
#include "storage/relfilenode.h"
#include "storage/smgr.h"
#include "storage/sync.h"


/* GUC options */
extern PGDLLIMPORT int BgWriterDelay;
extern PGDLLIMPORT int CheckPointTimeout;
extern PGDLLIMPORT int CheckPointWarning;
extern PGDLLIMPORT double CheckPointCompletionTarget;

extern PGDLLIMPORT void BackgroundWriterMain(void) pg_attribute_noreturn();
extern PGDLLIMPORT void CheckpointerMain(void) pg_attribute_noreturn();

extern PGDLLIMPORT void RequestCheckpoint(int flags);
extern PGDLLIMPORT void CheckpointWriteDelay(int flags, double progress);

extern PGDLLIMPORT bool ForwardSyncRequest(const FileTag *ftag, SyncRequestType type);

extern PGDLLIMPORT void AbsorbSyncRequests(void);

extern PGDLLIMPORT Size CheckpointerShmemSize(void);
extern PGDLLIMPORT void CheckpointerShmemInit(void);

extern PGDLLIMPORT bool FirstCallSinceLastCheckpoint(void);

#endif							/* _BGWRITER_H */
