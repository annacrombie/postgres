/*-------------------------------------------------------------------------
 *
 * table.h
 *	  Generic routines for table related code.
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/access/table.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef TABLE_H
#define TABLE_H

#include "nodes/primnodes.h"
#include "storage/lockdefs.h"
#include "utils/relcache.h"

extern PGDLLIMPORT Relation table_open(Oid relationId, LOCKMODE lockmode);
extern PGDLLIMPORT Relation table_openrv(const RangeVar *relation, LOCKMODE lockmode);
extern PGDLLIMPORT Relation table_openrv_extended(const RangeVar *relation,
									  LOCKMODE lockmode, bool missing_ok);
extern PGDLLIMPORT Relation try_table_open(Oid relationId, LOCKMODE lockmode);
extern PGDLLIMPORT void table_close(Relation relation, LOCKMODE lockmode);

#endif							/* TABLE_H */
