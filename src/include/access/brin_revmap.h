/*
 * brin_revmap.h
 *		Prototypes for BRIN reverse range maps
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * IDENTIFICATION
 *		src/include/access/brin_revmap.h
 */

#ifndef BRIN_REVMAP_H
#define BRIN_REVMAP_H

#include "access/brin_tuple.h"
#include "storage/block.h"
#include "storage/buf.h"
#include "storage/itemptr.h"
#include "storage/off.h"
#include "utils/relcache.h"
#include "utils/snapshot.h"

/* struct definition lives in brin_revmap.c */
typedef struct BrinRevmap BrinRevmap;

extern PGDLLIMPORT BrinRevmap *brinRevmapInitialize(Relation idxrel,
										BlockNumber *pagesPerRange, Snapshot snapshot);
extern PGDLLIMPORT void brinRevmapTerminate(BrinRevmap *revmap);

extern PGDLLIMPORT void brinRevmapExtend(BrinRevmap *revmap,
							 BlockNumber heapBlk);
extern PGDLLIMPORT Buffer brinLockRevmapPageForUpdate(BrinRevmap *revmap,
										  BlockNumber heapBlk);
extern PGDLLIMPORT void brinSetHeapBlockItemptr(Buffer buf, BlockNumber pagesPerRange,
									BlockNumber heapBlk, ItemPointerData tid);
extern PGDLLIMPORT BrinTuple *brinGetTupleForHeapBlock(BrinRevmap *revmap,
										   BlockNumber heapBlk, Buffer *buf, OffsetNumber *off,
										   Size *size, int mode, Snapshot snapshot);
extern PGDLLIMPORT bool brinRevmapDesummarizeRange(Relation idxrel, BlockNumber heapBlk);

#endif							/* BRIN_REVMAP_H */
