/*
 * subtrans.h
 *
 * PostgreSQL subtransaction-log manager
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/access/subtrans.h
 */
#ifndef SUBTRANS_H
#define SUBTRANS_H

/* Number of SLRU buffers to use for subtrans */
#define NUM_SUBTRANS_BUFFERS	32

extern PGDLLIMPORT void SubTransSetParent(TransactionId xid, TransactionId parent);
extern PGDLLIMPORT TransactionId SubTransGetParent(TransactionId xid);
extern PGDLLIMPORT TransactionId SubTransGetTopmostTransaction(TransactionId xid);

extern PGDLLIMPORT Size SUBTRANSShmemSize(void);
extern PGDLLIMPORT void SUBTRANSShmemInit(void);
extern PGDLLIMPORT void BootStrapSUBTRANS(void);
extern PGDLLIMPORT void StartupSUBTRANS(TransactionId oldestActiveXID);
extern PGDLLIMPORT void CheckPointSUBTRANS(void);
extern PGDLLIMPORT void ExtendSUBTRANS(TransactionId newestXact);
extern PGDLLIMPORT void TruncateSUBTRANS(TransactionId oldestXact);

#endif							/* SUBTRANS_H */
