/*-------------------------------------------------------------------------
 *
 * dbcommands.h
 *		Database management commands (create/drop database).
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/commands/dbcommands.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef DBCOMMANDS_H
#define DBCOMMANDS_H

#include "access/xlogreader.h"
#include "catalog/objectaddress.h"
#include "lib/stringinfo.h"
#include "parser/parse_node.h"

extern PGDLLIMPORT Oid	createdb(ParseState *pstate, const CreatedbStmt *stmt);
extern PGDLLIMPORT void dropdb(const char *dbname, bool missing_ok, bool force);
extern PGDLLIMPORT void DropDatabase(ParseState *pstate, DropdbStmt *stmt);
extern PGDLLIMPORT ObjectAddress RenameDatabase(const char *oldname, const char *newname);
extern PGDLLIMPORT Oid	AlterDatabase(ParseState *pstate, AlterDatabaseStmt *stmt, bool isTopLevel);
extern PGDLLIMPORT ObjectAddress AlterDatabaseRefreshColl(AlterDatabaseRefreshCollStmt *stmt);
extern PGDLLIMPORT Oid	AlterDatabaseSet(AlterDatabaseSetStmt *stmt);
extern PGDLLIMPORT ObjectAddress AlterDatabaseOwner(const char *dbname, Oid newOwnerId);

extern PGDLLIMPORT Oid	get_database_oid(const char *dbname, bool missing_ok);
extern PGDLLIMPORT char *get_database_name(Oid dbid);

extern PGDLLIMPORT void check_encoding_locale_matches(int encoding, const char *collate, const char *ctype);

#endif							/* DBCOMMANDS_H */
