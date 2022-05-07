/*-------------------------------------------------------------------------
 *
 * relmapper.h
 *	  Catalog-to-filenode mapping
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/utils/relmapper.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef RELMAPPER_H
#define RELMAPPER_H

#include "access/xlogreader.h"
#include "lib/stringinfo.h"

/* ----------------
 *		relmap-related XLOG entries
 * ----------------
 */

#define XLOG_RELMAP_UPDATE		0x00

typedef struct xl_relmap_update
{
	Oid			dbid;			/* database ID, or 0 for shared map */
	Oid			tsid;			/* database's tablespace, or pg_global */
	int32		nbytes;			/* size of relmap data */
	char		data[FLEXIBLE_ARRAY_MEMBER];
} xl_relmap_update;

#define MinSizeOfRelmapUpdate offsetof(xl_relmap_update, data)


extern PGDLLIMPORT Oid	RelationMapOidToFilenode(Oid relationId, bool shared);

extern PGDLLIMPORT Oid	RelationMapFilenodeToOid(Oid relationId, bool shared);
extern PGDLLIMPORT Oid RelationMapOidToFilenodeForDatabase(char *dbpath, Oid relationId);
extern PGDLLIMPORT void RelationMapCopy(Oid dbid, Oid tsid, char *srcdbpath,
							char *dstdbpath);
extern PGDLLIMPORT void RelationMapUpdateMap(Oid relationId, Oid fileNode, bool shared,
								 bool immediate);

extern PGDLLIMPORT void RelationMapRemoveMapping(Oid relationId);

extern PGDLLIMPORT void RelationMapInvalidate(bool shared);
extern PGDLLIMPORT void RelationMapInvalidateAll(void);

extern PGDLLIMPORT void AtCCI_RelationMap(void);
extern PGDLLIMPORT void AtEOXact_RelationMap(bool isCommit, bool isParallelWorker);
extern PGDLLIMPORT void AtPrepare_RelationMap(void);

extern PGDLLIMPORT void CheckPointRelationMap(void);

extern PGDLLIMPORT void RelationMapFinishBootstrap(void);

extern PGDLLIMPORT void RelationMapInitialize(void);
extern PGDLLIMPORT void RelationMapInitializePhase2(void);
extern PGDLLIMPORT void RelationMapInitializePhase3(void);

extern PGDLLIMPORT Size EstimateRelationMapSpace(void);
extern PGDLLIMPORT void SerializeRelationMap(Size maxSize, char *startAddress);
extern PGDLLIMPORT void RestoreRelationMap(char *startAddress);

extern PGDLLIMPORT void relmap_redo(XLogReaderState *record);
extern PGDLLIMPORT void relmap_desc(StringInfo buf, XLogReaderState *record);
extern PGDLLIMPORT const char *relmap_identify(uint8 info);

#endif							/* RELMAPPER_H */
