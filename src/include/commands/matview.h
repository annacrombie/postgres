/*-------------------------------------------------------------------------
 *
 * matview.h
 *	  prototypes for matview.c.
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/commands/matview.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef MATVIEW_H
#define MATVIEW_H

#include "catalog/objectaddress.h"
#include "nodes/params.h"
#include "nodes/parsenodes.h"
#include "tcop/dest.h"
#include "utils/relcache.h"


extern PGDLLIMPORT void SetMatViewPopulatedState(Relation relation, bool newstate);

extern PGDLLIMPORT ObjectAddress ExecRefreshMatView(RefreshMatViewStmt *stmt, const char *queryString,
										ParamListInfo params, QueryCompletion *qc);

extern PGDLLIMPORT DestReceiver *CreateTransientRelDestReceiver(Oid oid);

extern PGDLLIMPORT bool MatViewIncrementalMaintenanceIsEnabled(void);

#endif							/* MATVIEW_H */
