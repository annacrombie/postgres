/*-------------------------------------------------------------------------
 *
 * walsender.h
 *	  Exports from replication/walsender.c.
 *
 * Portions Copyright (c) 2010-2022, PostgreSQL Global Development Group
 *
 * src/include/replication/walsender.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef _WALSENDER_H
#define _WALSENDER_H

#include <signal.h>

/*
 * What to do with a snapshot in create replication slot command.
 */
typedef enum
{
	CRS_EXPORT_SNAPSHOT,
	CRS_NOEXPORT_SNAPSHOT,
	CRS_USE_SNAPSHOT
} CRSSnapshotAction;

/* global state */
extern PGDLLIMPORT bool am_walsender;
extern PGDLLIMPORT bool am_cascading_walsender;
extern PGDLLIMPORT bool am_db_walsender;
extern PGDLLIMPORT bool wake_wal_senders;

/* user-settable parameters */
extern PGDLLIMPORT int max_wal_senders;
extern PGDLLIMPORT int wal_sender_timeout;
extern PGDLLIMPORT bool log_replication_commands;

extern PGDLLIMPORT void InitWalSender(void);
extern PGDLLIMPORT bool exec_replication_command(const char *query_string);
extern PGDLLIMPORT void WalSndErrorCleanup(void);
extern PGDLLIMPORT void WalSndResourceCleanup(bool isCommit);
extern PGDLLIMPORT void WalSndSignals(void);
extern PGDLLIMPORT Size WalSndShmemSize(void);
extern PGDLLIMPORT void WalSndShmemInit(void);
extern PGDLLIMPORT void WalSndWakeup(void);
extern PGDLLIMPORT void WalSndInitStopping(void);
extern PGDLLIMPORT void WalSndWaitStopping(void);
extern PGDLLIMPORT void HandleWalSndInitStopping(void);
extern PGDLLIMPORT void WalSndRqstFileReload(void);

/*
 * Remember that we want to wakeup walsenders later
 *
 * This is separated from doing the actual wakeup because the writeout is done
 * while holding contended locks.
 */
#define WalSndWakeupRequest() \
	do { wake_wal_senders = true; } while (0)

/*
 * wakeup walsenders if there is work to be done
 */
#define WalSndWakeupProcessRequests()		\
	do										\
	{										\
		if (wake_wal_senders)				\
		{									\
			wake_wal_senders = false;		\
			if (max_wal_senders > 0)		\
				WalSndWakeup();				\
		}									\
	} while (0)

#endif							/* _WALSENDER_H */
