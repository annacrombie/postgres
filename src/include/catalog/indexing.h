/*-------------------------------------------------------------------------
 *
 * indexing.h
 *	  This file provides some definitions to support indexing
 *	  on system catalogs
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/indexing.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef INDEXING_H
#define INDEXING_H

#include "access/htup.h"
#include "nodes/execnodes.h"
#include "utils/relcache.h"

/*
 * The state object used by CatalogOpenIndexes and friends is actually the
 * same as the executor's ResultRelInfo, but we give it another type name
 * to decouple callers from that fact.
 */
typedef struct ResultRelInfo *CatalogIndexState;

/*
 * Cap the maximum amount of bytes allocated for multi-inserts with system
 * catalogs, limiting the number of slots used.
 */
#define MAX_CATALOG_MULTI_INSERT_BYTES 65535

/*
 * indexing.c prototypes
 */
extern PGDLLIMPORT CatalogIndexState CatalogOpenIndexes(Relation heapRel);
extern PGDLLIMPORT void CatalogCloseIndexes(CatalogIndexState indstate);
extern PGDLLIMPORT void CatalogTupleInsert(Relation heapRel, HeapTuple tup);
extern PGDLLIMPORT void CatalogTupleInsertWithInfo(Relation heapRel, HeapTuple tup,
									   CatalogIndexState indstate);
extern PGDLLIMPORT void CatalogTuplesMultiInsertWithInfo(Relation heapRel,
											 TupleTableSlot **slot,
											 int ntuples,
											 CatalogIndexState indstate);
extern PGDLLIMPORT void CatalogTupleUpdate(Relation heapRel, ItemPointer otid,
							   HeapTuple tup);
extern PGDLLIMPORT void CatalogTupleUpdateWithInfo(Relation heapRel,
									   ItemPointer otid, HeapTuple tup,
									   CatalogIndexState indstate);
extern PGDLLIMPORT void CatalogTupleDelete(Relation heapRel, ItemPointer tid);

#endif							/* INDEXING_H */
