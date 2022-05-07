/*-------------------------------------------------------------------------
 *
 * prepare.h
 *	  PREPARE, EXECUTE and DEALLOCATE commands, and prepared-stmt storage
 *
 *
 * Copyright (c) 2002-2022, PostgreSQL Global Development Group
 *
 * src/include/commands/prepare.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PREPARE_H
#define PREPARE_H

#include "commands/explain.h"
#include "datatype/timestamp.h"
#include "utils/plancache.h"

/*
 * The data structure representing a prepared statement.  This is now just
 * a thin veneer over a plancache entry --- the main addition is that of
 * a name.
 *
 * Note: all subsidiary storage lives in the referenced plancache entry.
 */
typedef struct
{
	/* dynahash.c requires key to be first field */
	char		stmt_name[NAMEDATALEN];
	CachedPlanSource *plansource;	/* the actual cached plan */
	bool		from_sql;		/* prepared via SQL, not FE/BE protocol? */
	TimestampTz prepare_time;	/* the time when the stmt was prepared */
} PreparedStatement;


/* Utility statements PREPARE, EXECUTE, DEALLOCATE, EXPLAIN EXECUTE */
extern PGDLLIMPORT void PrepareQuery(ParseState *pstate, PrepareStmt *stmt,
						 int stmt_location, int stmt_len);
extern PGDLLIMPORT void ExecuteQuery(ParseState *pstate,
						 ExecuteStmt *stmt, IntoClause *intoClause,
						 ParamListInfo params,
						 DestReceiver *dest, QueryCompletion *qc);
extern PGDLLIMPORT void DeallocateQuery(DeallocateStmt *stmt);
extern PGDLLIMPORT void ExplainExecuteQuery(ExecuteStmt *execstmt, IntoClause *into,
								ExplainState *es, const char *queryString,
								ParamListInfo params, QueryEnvironment *queryEnv);

/* Low-level access to stored prepared statements */
extern PGDLLIMPORT void StorePreparedStatement(const char *stmt_name,
								   CachedPlanSource *plansource,
								   bool from_sql);
extern PGDLLIMPORT PreparedStatement *FetchPreparedStatement(const char *stmt_name,
												 bool throwError);
extern PGDLLIMPORT void DropPreparedStatement(const char *stmt_name, bool showError);
extern PGDLLIMPORT TupleDesc FetchPreparedStatementResultDesc(PreparedStatement *stmt);
extern PGDLLIMPORT List *FetchPreparedStatementTargetList(PreparedStatement *stmt);

extern PGDLLIMPORT void DropAllPreparedStatements(void);

#endif							/* PREPARE_H */
