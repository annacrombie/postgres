/*-------------------------------------------------------------------------
 *
 * logicallauncher.h
 *	  Exports for logical replication launcher.
 *
 * Portions Copyright (c) 2016-2022, PostgreSQL Global Development Group
 *
 * src/include/replication/logicallauncher.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef LOGICALLAUNCHER_H
#define LOGICALLAUNCHER_H

extern PGDLLIMPORT int max_logical_replication_workers;
extern PGDLLIMPORT int max_sync_workers_per_subscription;

extern PGDLLIMPORT void ApplyLauncherRegister(void);
extern PGDLLIMPORT void ApplyLauncherMain(Datum main_arg);

extern PGDLLIMPORT Size ApplyLauncherShmemSize(void);
extern PGDLLIMPORT void ApplyLauncherShmemInit(void);

extern PGDLLIMPORT void ApplyLauncherWakeupAtCommit(void);
extern PGDLLIMPORT void AtEOXact_ApplyLauncher(bool isCommit);

extern PGDLLIMPORT bool IsLogicalLauncher(void);

#endif							/* LOGICALLAUNCHER_H */
