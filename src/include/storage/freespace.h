/*-------------------------------------------------------------------------
 *
 * freespace.h
 *	  POSTGRES free space map for quickly finding free space in relations
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/storage/freespace.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef FREESPACE_H_
#define FREESPACE_H_

#include "storage/block.h"
#include "storage/relfilenode.h"
#include "utils/relcache.h"

/* prototypes for public functions in freespace.c */
extern PGDLLIMPORT Size GetRecordedFreeSpace(Relation rel, BlockNumber heapBlk);
extern PGDLLIMPORT BlockNumber GetPageWithFreeSpace(Relation rel, Size spaceNeeded);
extern PGDLLIMPORT BlockNumber RecordAndGetPageWithFreeSpace(Relation rel,
												 BlockNumber oldPage,
												 Size oldSpaceAvail,
												 Size spaceNeeded);
extern PGDLLIMPORT void RecordPageWithFreeSpace(Relation rel, BlockNumber heapBlk,
									Size spaceAvail);
extern PGDLLIMPORT void XLogRecordPageWithFreeSpace(RelFileNode rnode, BlockNumber heapBlk,
										Size spaceAvail);

extern PGDLLIMPORT BlockNumber FreeSpaceMapPrepareTruncateRel(Relation rel,
												  BlockNumber nblocks);
extern PGDLLIMPORT void FreeSpaceMapVacuum(Relation rel);
extern PGDLLIMPORT void FreeSpaceMapVacuumRange(Relation rel, BlockNumber start,
									BlockNumber end);

#endif							/* FREESPACE_H_ */
