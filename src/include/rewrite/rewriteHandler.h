/*-------------------------------------------------------------------------
 *
 * rewriteHandler.h
 *		External interface to query rewriter.
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/rewrite/rewriteHandler.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef REWRITEHANDLER_H
#define REWRITEHANDLER_H

#include "nodes/parsenodes.h"
#include "utils/relcache.h"

extern PGDLLIMPORT List *QueryRewrite(Query *parsetree);
extern PGDLLIMPORT void AcquireRewriteLocks(Query *parsetree,
								bool forExecute,
								bool forUpdatePushedDown);

extern PGDLLIMPORT Node *build_column_default(Relation rel, int attrno);

extern PGDLLIMPORT void fill_extraUpdatedCols(RangeTblEntry *target_rte,
								  Relation target_relation);

extern PGDLLIMPORT Query *get_view_query(Relation view);
extern PGDLLIMPORT const char *view_query_is_auto_updatable(Query *viewquery,
												bool check_cols);
extern PGDLLIMPORT int	relation_is_updatable(Oid reloid,
								  List *outer_reloids,
								  bool include_triggers,
								  Bitmapset *include_cols);

#endif							/* REWRITEHANDLER_H */
