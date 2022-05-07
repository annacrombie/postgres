/*-------------------------------------------------------------------------
 *
 * indexfsm.h
 *	  POSTGRES free space map for quickly finding an unused page in index
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/storage/indexfsm.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef INDEXFSM_H_
#define INDEXFSM_H_

#include "storage/block.h"
#include "utils/relcache.h"

extern PGDLLIMPORT BlockNumber GetFreeIndexPage(Relation rel);
extern PGDLLIMPORT void RecordFreeIndexPage(Relation rel, BlockNumber page);
extern PGDLLIMPORT void RecordUsedIndexPage(Relation rel, BlockNumber page);

extern PGDLLIMPORT void IndexFreeSpaceMapVacuum(Relation rel);

#endif							/* INDEXFSM_H_ */
