/*-------------------------------------------------------------------------
 *
 * analyze.h
 *		parse analysis for optimizable statements
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/parser/analyze.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef ANALYZE_H
#define ANALYZE_H

#include "nodes/params.h"
#include "parser/parse_node.h"
#include "utils/queryjumble.h"

/* Hook for plugins to get control at end of parse analysis */
typedef void (*post_parse_analyze_hook_type) (ParseState *pstate,
											  Query *query,
											  JumbleState *jstate);
extern PGDLLIMPORT post_parse_analyze_hook_type post_parse_analyze_hook;


extern PGDLLIMPORT Query *parse_analyze_fixedparams(RawStmt *parseTree, const char *sourceText,
							const Oid *paramTypes, int numParams, QueryEnvironment *queryEnv);
extern PGDLLIMPORT Query *parse_analyze_varparams(RawStmt *parseTree, const char *sourceText,
									  Oid **paramTypes, int *numParams, QueryEnvironment *queryEnv);
extern PGDLLIMPORT Query *parse_analyze_withcb(RawStmt *parseTree, const char *sourceText,
								   ParserSetupHook parserSetup,
								   void *parserSetupArg,
								   QueryEnvironment *queryEnv);

extern PGDLLIMPORT Query *parse_sub_analyze(Node *parseTree, ParseState *parentParseState,
								CommonTableExpr *parentCTE,
								bool locked_from_parent,
								bool resolve_unknowns);

extern PGDLLIMPORT List *transformInsertRow(ParseState *pstate, List *exprlist,
								List *stmtcols, List *icolumns, List *attrnos,
								bool strip_indirection);
extern PGDLLIMPORT List *transformUpdateTargetList(ParseState *pstate,
									   List *targetList);
extern PGDLLIMPORT Query *transformTopLevelStmt(ParseState *pstate, RawStmt *parseTree);
extern PGDLLIMPORT Query *transformStmt(ParseState *pstate, Node *parseTree);

extern PGDLLIMPORT bool analyze_requires_snapshot(RawStmt *parseTree);

extern PGDLLIMPORT const char *LCS_asString(LockClauseStrength strength);
extern PGDLLIMPORT void CheckSelectLocking(Query *qry, LockClauseStrength strength);
extern PGDLLIMPORT void applyLockingClause(Query *qry, Index rtindex,
							   LockClauseStrength strength,
							   LockWaitPolicy waitPolicy, bool pushedDown);

extern PGDLLIMPORT List *BuildOnConflictExcludedTargetlist(Relation targetrel,
											   Index exclRelIndex);

extern PGDLLIMPORT SortGroupClause *makeSortGroupClauseForSetOp(Oid rescoltype, bool require_hash);

#endif							/* ANALYZE_H */
