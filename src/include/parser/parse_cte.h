/*-------------------------------------------------------------------------
 *
 * parse_cte.h
 *	  handle CTEs (common table expressions) in parser
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/parser/parse_cte.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PARSE_CTE_H
#define PARSE_CTE_H

#include "parser/parse_node.h"

extern PGDLLIMPORT List *transformWithClause(ParseState *pstate, WithClause *withClause);

extern PGDLLIMPORT void analyzeCTETargetList(ParseState *pstate, CommonTableExpr *cte,
								 List *tlist);

#endif							/* PARSE_CTE_H */
