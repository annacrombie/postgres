/*-------------------------------------------------------------------------
 *
 * syncscan.h
 *    POSTGRES synchronous scan support functions.
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/access/syncscan.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef SYNCSCAN_H
#define SYNCSCAN_H

#include "storage/block.h"
#include "utils/relcache.h"

extern PGDLLIMPORT void ss_report_location(Relation rel, BlockNumber location);
extern PGDLLIMPORT BlockNumber ss_get_location(Relation rel, BlockNumber relnblocks);
extern PGDLLIMPORT void SyncScanShmemInit(void);
extern PGDLLIMPORT Size SyncScanShmemSize(void);

#endif
