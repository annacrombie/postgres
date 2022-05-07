/*-------------------------------------------------------------------------
 *
 * async.h
 *	  Asynchronous notification: NOTIFY, LISTEN, UNLISTEN
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/commands/async.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef ASYNC_H
#define ASYNC_H

#include <signal.h>

/*
 * The number of SLRU page buffers we use for the notification queue.
 */
#define NUM_NOTIFY_BUFFERS	8

extern PGDLLIMPORT bool Trace_notify;
extern PGDLLIMPORT volatile sig_atomic_t notifyInterruptPending;

extern PGDLLIMPORT Size AsyncShmemSize(void);
extern PGDLLIMPORT void AsyncShmemInit(void);

extern PGDLLIMPORT void NotifyMyFrontEnd(const char *channel,
							 const char *payload,
							 int32 srcPid);

/* notify-related SQL statements */
extern PGDLLIMPORT void Async_Notify(const char *channel, const char *payload);
extern PGDLLIMPORT void Async_Listen(const char *channel);
extern PGDLLIMPORT void Async_Unlisten(const char *channel);
extern PGDLLIMPORT void Async_UnlistenAll(void);

/* perform (or cancel) outbound notify processing at transaction commit */
extern PGDLLIMPORT void PreCommit_Notify(void);
extern PGDLLIMPORT void AtCommit_Notify(void);
extern PGDLLIMPORT void AtAbort_Notify(void);
extern PGDLLIMPORT void AtSubCommit_Notify(void);
extern PGDLLIMPORT void AtSubAbort_Notify(void);
extern PGDLLIMPORT void AtPrepare_Notify(void);

/* signal handler for inbound notifies (PROCSIG_NOTIFY_INTERRUPT) */
extern PGDLLIMPORT void HandleNotifyInterrupt(void);

/* process interrupts */
extern PGDLLIMPORT void ProcessNotifyInterrupt(bool flush);

#endif							/* ASYNC_H */
