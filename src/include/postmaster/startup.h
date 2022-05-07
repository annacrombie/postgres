/*-------------------------------------------------------------------------
 *
 * startup.h
 *	  Exports from postmaster/startup.c.
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 *
 * src/include/postmaster/startup.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef _STARTUP_H
#define _STARTUP_H

/*
 * Log the startup progress message if a timer has expired.
 */
#define ereport_startup_progress(msg, ...) \
	do { \
		long    secs; \
		int     usecs; \
		if (has_startup_progress_timeout_expired(&secs, &usecs)) \
			ereport(LOG, errmsg(msg, secs, (usecs / 10000),  __VA_ARGS__ )); \
	} while(0)

extern PGDLLIMPORT int log_startup_progress_interval;

extern PGDLLIMPORT void HandleStartupProcInterrupts(void);
extern PGDLLIMPORT void StartupProcessMain(void) pg_attribute_noreturn();
extern PGDLLIMPORT void PreRestoreCommand(void);
extern PGDLLIMPORT void PostRestoreCommand(void);
extern PGDLLIMPORT bool IsPromoteSignaled(void);
extern PGDLLIMPORT void ResetPromoteSignaled(void);

extern PGDLLIMPORT void begin_startup_progress_phase(void);
extern PGDLLIMPORT void startup_progress_timeout_handler(void);
extern PGDLLIMPORT bool has_startup_progress_timeout_expired(long *secs, int *usecs);

#endif							/* _STARTUP_H */
