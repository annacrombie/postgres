/*-------------------------------------------------------------------------
 *
 * subscriptioncmds.h
 *	  prototypes for subscriptioncmds.c.
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/commands/subscriptioncmds.h
 *
 *-------------------------------------------------------------------------
 */

#ifndef SUBSCRIPTIONCMDS_H
#define SUBSCRIPTIONCMDS_H

#include "catalog/objectaddress.h"
#include "parser/parse_node.h"

extern PGDLLIMPORT ObjectAddress CreateSubscription(ParseState *pstate, CreateSubscriptionStmt *stmt,
										bool isTopLevel);
extern PGDLLIMPORT ObjectAddress AlterSubscription(ParseState *pstate, AlterSubscriptionStmt *stmt, bool isTopLevel);
extern PGDLLIMPORT void DropSubscription(DropSubscriptionStmt *stmt, bool isTopLevel);

extern PGDLLIMPORT ObjectAddress AlterSubscriptionOwner(const char *name, Oid newOwnerId);
extern PGDLLIMPORT void AlterSubscriptionOwner_oid(Oid subid, Oid newOwnerId);

#endif							/* SUBSCRIPTIONCMDS_H */
