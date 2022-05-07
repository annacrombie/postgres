/*------------------------------------------------------------------------
 *
 * xlogarchive.h
 *		Prototypes for WAL archives in the backend
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * IDENTIFICATION
 *		src/include/access/xlogarchive.h
 *
 *------------------------------------------------------------------------
 */

#ifndef XLOG_ARCHIVE_H
#define XLOG_ARCHIVE_H

#include "access/xlogdefs.h"

extern PGDLLIMPORT bool RestoreArchivedFile(char *path, const char *xlogfname,
								const char *recovername, off_t expectedSize,
								bool cleanupEnabled);
extern PGDLLIMPORT void ExecuteRecoveryCommand(const char *command, const char *commandName,
								   bool failOnSignal, uint32 wait_event_info);
extern PGDLLIMPORT void KeepFileRestoredFromArchive(const char *path, const char *xlogfname);
extern PGDLLIMPORT void XLogArchiveNotify(const char *xlog);
extern PGDLLIMPORT void XLogArchiveNotifySeg(XLogSegNo segno, TimeLineID tli);
extern PGDLLIMPORT void XLogArchiveForceDone(const char *xlog);
extern PGDLLIMPORT bool XLogArchiveCheckDone(const char *xlog);
extern PGDLLIMPORT bool XLogArchiveIsBusy(const char *xlog);
extern PGDLLIMPORT bool XLogArchiveIsReady(const char *xlog);
extern PGDLLIMPORT bool XLogArchiveIsReadyOrDone(const char *xlog);
extern PGDLLIMPORT void XLogArchiveCleanup(const char *xlog);

#endif							/* XLOG_ARCHIVE_H */
