/*-------------------------------------------------------------------------
 *
 * print.h
 *	  definitions for nodes/print.c
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/nodes/print.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PRINT_H
#define PRINT_H

#include "executor/tuptable.h"


#define nodeDisplay(x)		pprint(x)

extern PGDLLIMPORT void print(const void *obj);
extern PGDLLIMPORT void pprint(const void *obj);
extern PGDLLIMPORT void elog_node_display(int lev, const char *title,
							  const void *obj, bool pretty);
extern PGDLLIMPORT char *format_node_dump(const char *dump);
extern PGDLLIMPORT char *pretty_format_node_dump(const char *dump);
extern PGDLLIMPORT void print_rt(const List *rtable);
extern PGDLLIMPORT void print_expr(const Node *expr, const List *rtable);
extern PGDLLIMPORT void print_pathkeys(const List *pathkeys, const List *rtable);
extern PGDLLIMPORT void print_tl(const List *tlist, const List *rtable);
extern PGDLLIMPORT void print_slot(TupleTableSlot *slot);

#endif							/* PRINT_H */
