/*
 * clog.h
 *
 * PostgreSQL transaction-commit-log manager
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/access/clog.h
 */
#ifndef CLOG_H
#define CLOG_H

#include "access/xlogreader.h"
#include "storage/sync.h"
#include "lib/stringinfo.h"

/*
 * Possible transaction statuses --- note that all-zeroes is the initial
 * state.
 *
 * A "subcommitted" transaction is a committed subtransaction whose parent
 * hasn't committed or aborted yet.
 */
typedef int XidStatus;

#define TRANSACTION_STATUS_IN_PROGRESS		0x00
#define TRANSACTION_STATUS_COMMITTED		0x01
#define TRANSACTION_STATUS_ABORTED			0x02
#define TRANSACTION_STATUS_SUB_COMMITTED	0x03

typedef struct xl_clog_truncate
{
	int			pageno;
	TransactionId oldestXact;
	Oid			oldestXactDb;
} xl_clog_truncate;

extern PGDLLIMPORT void TransactionIdSetTreeStatus(TransactionId xid, int nsubxids,
									   TransactionId *subxids, XidStatus status, XLogRecPtr lsn);
extern PGDLLIMPORT XidStatus TransactionIdGetStatus(TransactionId xid, XLogRecPtr *lsn);

extern PGDLLIMPORT Size CLOGShmemBuffers(void);
extern PGDLLIMPORT Size CLOGShmemSize(void);
extern PGDLLIMPORT void CLOGShmemInit(void);
extern PGDLLIMPORT void BootStrapCLOG(void);
extern PGDLLIMPORT void StartupCLOG(void);
extern PGDLLIMPORT void TrimCLOG(void);
extern PGDLLIMPORT void CheckPointCLOG(void);
extern PGDLLIMPORT void ExtendCLOG(TransactionId newestXact);
extern PGDLLIMPORT void TruncateCLOG(TransactionId oldestXact, Oid oldestxid_datoid);

extern PGDLLIMPORT int	clogsyncfiletag(const FileTag *ftag, char *path);

/* XLOG stuff */
#define CLOG_ZEROPAGE		0x00
#define CLOG_TRUNCATE		0x10

extern PGDLLIMPORT void clog_redo(XLogReaderState *record);
extern PGDLLIMPORT void clog_desc(StringInfo buf, XLogReaderState *record);
extern PGDLLIMPORT const char *clog_identify(uint8 info);

#endif							/* CLOG_H */
