/*-------------------------------------------------------------------------
 *
 * explain.h
 *	  prototypes for explain.c
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994-5, Regents of the University of California
 *
 * src/include/commands/explain.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef EXPLAIN_H
#define EXPLAIN_H

#include "executor/executor.h"
#include "lib/stringinfo.h"
#include "parser/parse_node.h"

typedef enum ExplainFormat
{
	EXPLAIN_FORMAT_TEXT,
	EXPLAIN_FORMAT_XML,
	EXPLAIN_FORMAT_JSON,
	EXPLAIN_FORMAT_YAML
} ExplainFormat;

typedef struct ExplainWorkersState
{
	int			num_workers;	/* # of worker processes the plan used */
	bool	   *worker_inited;	/* per-worker state-initialized flags */
	StringInfoData *worker_str; /* per-worker transient output buffers */
	int		   *worker_state_save;	/* per-worker grouping state save areas */
	StringInfo	prev_str;		/* saved output buffer while redirecting */
} ExplainWorkersState;

typedef struct ExplainState
{
	StringInfo	str;			/* output buffer */
	/* options */
	bool		verbose;		/* be verbose */
	bool		analyze;		/* print actual times */
	bool		costs;			/* print estimated costs */
	bool		buffers;		/* print buffer usage */
	bool		wal;			/* print WAL usage */
	bool		timing;			/* print detailed node timing */
	bool		summary;		/* print total planning and execution timing */
	bool		settings;		/* print modified settings */
	ExplainFormat format;		/* output format */
	/* state for output formatting --- not reset for each new plan tree */
	int			indent;			/* current indentation level */
	List	   *grouping_stack; /* format-specific grouping state */
	/* state related to the current plan tree (filled by ExplainPrintPlan) */
	PlannedStmt *pstmt;			/* top of plan */
	List	   *rtable;			/* range table */
	List	   *rtable_names;	/* alias names for RTEs */
	List	   *deparse_cxt;	/* context list for deparsing expressions */
	Bitmapset  *printed_subplans;	/* ids of SubPlans we've printed */
	bool		hide_workers;	/* set if we find an invisible Gather */
	/* state related to the current plan node */
	ExplainWorkersState *workers_state; /* needed if parallel plan */
} ExplainState;

/* Hook for plugins to get control in ExplainOneQuery() */
typedef void (*ExplainOneQuery_hook_type) (Query *query,
										   int cursorOptions,
										   IntoClause *into,
										   ExplainState *es,
										   const char *queryString,
										   ParamListInfo params,
										   QueryEnvironment *queryEnv);
extern PGDLLIMPORT ExplainOneQuery_hook_type ExplainOneQuery_hook;

/* Hook for plugins to get control in explain_get_index_name() */
typedef const char *(*explain_get_index_name_hook_type) (Oid indexId);
extern PGDLLIMPORT explain_get_index_name_hook_type explain_get_index_name_hook;


extern PGDLLIMPORT void ExplainQuery(ParseState *pstate, ExplainStmt *stmt,
						 ParamListInfo params, DestReceiver *dest);

extern PGDLLIMPORT ExplainState *NewExplainState(void);

extern PGDLLIMPORT TupleDesc ExplainResultDesc(ExplainStmt *stmt);

extern PGDLLIMPORT void ExplainOneUtility(Node *utilityStmt, IntoClause *into,
							  ExplainState *es, const char *queryString,
							  ParamListInfo params, QueryEnvironment *queryEnv);

extern PGDLLIMPORT void ExplainOnePlan(PlannedStmt *plannedstmt, IntoClause *into,
						   ExplainState *es, const char *queryString,
						   ParamListInfo params, QueryEnvironment *queryEnv,
						   const instr_time *planduration,
						   const BufferUsage *bufusage);

extern PGDLLIMPORT void ExplainPrintPlan(ExplainState *es, QueryDesc *queryDesc);
extern PGDLLIMPORT void ExplainPrintTriggers(ExplainState *es, QueryDesc *queryDesc);

extern PGDLLIMPORT void ExplainPrintJITSummary(ExplainState *es, QueryDesc *queryDesc);

extern PGDLLIMPORT void ExplainQueryText(ExplainState *es, QueryDesc *queryDesc);

extern PGDLLIMPORT void ExplainBeginOutput(ExplainState *es);
extern PGDLLIMPORT void ExplainEndOutput(ExplainState *es);
extern PGDLLIMPORT void ExplainSeparatePlans(ExplainState *es);

extern PGDLLIMPORT void ExplainPropertyList(const char *qlabel, List *data,
								ExplainState *es);
extern PGDLLIMPORT void ExplainPropertyListNested(const char *qlabel, List *data,
									  ExplainState *es);
extern PGDLLIMPORT void ExplainPropertyText(const char *qlabel, const char *value,
								ExplainState *es);
extern PGDLLIMPORT void ExplainPropertyInteger(const char *qlabel, const char *unit,
								   int64 value, ExplainState *es);
extern PGDLLIMPORT void ExplainPropertyUInteger(const char *qlabel, const char *unit,
									uint64 value, ExplainState *es);
extern PGDLLIMPORT void ExplainPropertyFloat(const char *qlabel, const char *unit,
								 double value, int ndigits, ExplainState *es);
extern PGDLLIMPORT void ExplainPropertyBool(const char *qlabel, bool value,
								ExplainState *es);

extern PGDLLIMPORT void ExplainOpenGroup(const char *objtype, const char *labelname,
							 bool labeled, ExplainState *es);
extern PGDLLIMPORT void ExplainCloseGroup(const char *objtype, const char *labelname,
							  bool labeled, ExplainState *es);

#endif							/* EXPLAIN_H */
