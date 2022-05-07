/*-------------------------------------------------------------------------
 *
 * collationcmds.h
 *	  prototypes for collationcmds.c.
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/commands/collationcmds.h
 *
 *-------------------------------------------------------------------------
 */

#ifndef COLLATIONCMDS_H
#define COLLATIONCMDS_H

#include "catalog/objectaddress.h"
#include "parser/parse_node.h"

extern PGDLLIMPORT ObjectAddress DefineCollation(ParseState *pstate, List *names, List *parameters, bool if_not_exists);
extern PGDLLIMPORT void IsThereCollationInNamespace(const char *collname, Oid nspOid);
extern PGDLLIMPORT ObjectAddress AlterCollation(AlterCollationStmt *stmt);

#endif							/* COLLATIONCMDS_H */
