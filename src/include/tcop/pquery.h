/*-------------------------------------------------------------------------
 *
 * pquery.h
 *	  prototypes for pquery.c.
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/tcop/pquery.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PQUERY_H
#define PQUERY_H

#include "nodes/parsenodes.h"
#include "utils/portal.h"

struct PlannedStmt;				/* avoid including plannodes.h here */


extern PGDLLIMPORT Portal ActivePortal;


extern PGDLLIMPORT PortalStrategy ChoosePortalStrategy(List *stmts);

extern PGDLLIMPORT List *FetchPortalTargetList(Portal portal);

extern PGDLLIMPORT List *FetchStatementTargetList(Node *stmt);

extern PGDLLIMPORT void PortalStart(Portal portal, ParamListInfo params,
						int eflags, Snapshot snapshot);

extern PGDLLIMPORT void PortalSetResultFormat(Portal portal, int nFormats,
								  int16 *formats);

extern PGDLLIMPORT bool PortalRun(Portal portal, long count, bool isTopLevel,
					  bool run_once, DestReceiver *dest, DestReceiver *altdest,
					  QueryCompletion *qc);

extern PGDLLIMPORT uint64 PortalRunFetch(Portal portal,
							 FetchDirection fdirection,
							 long count,
							 DestReceiver *dest);

extern PGDLLIMPORT bool PlannedStmtRequiresSnapshot(struct PlannedStmt *pstmt);

extern PGDLLIMPORT void EnsurePortalSnapshotExists(void);

#endif							/* PQUERY_H */
