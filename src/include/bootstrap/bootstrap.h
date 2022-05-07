/*-------------------------------------------------------------------------
 *
 * bootstrap.h
 *	  include file for the bootstrapping code
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/bootstrap/bootstrap.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef BOOTSTRAP_H
#define BOOTSTRAP_H

#include "nodes/execnodes.h"


/*
 * MAXATTR is the maximum number of attributes in a relation supported
 * at bootstrap time (i.e., the max possible in a system table).
 */
#define MAXATTR 40

#define BOOTCOL_NULL_AUTO			1
#define BOOTCOL_NULL_FORCE_NULL		2
#define BOOTCOL_NULL_FORCE_NOT_NULL 3

extern PGDLLIMPORT Relation boot_reldesc;
extern PGDLLIMPORT Form_pg_attribute attrtypes[MAXATTR];
extern PGDLLIMPORT int numattr;


extern PGDLLIMPORT void BootstrapModeMain(int argc, char *argv[], bool check_only) pg_attribute_noreturn();

extern PGDLLIMPORT void closerel(char *name);
extern PGDLLIMPORT void boot_openrel(char *name);

extern PGDLLIMPORT void DefineAttr(char *name, char *type, int attnum, int nullness);
extern PGDLLIMPORT void InsertOneTuple(void);
extern PGDLLIMPORT void InsertOneValue(char *value, int i);
extern PGDLLIMPORT void InsertOneNull(int i);

extern PGDLLIMPORT void index_register(Oid heap, Oid ind, IndexInfo *indexInfo);
extern PGDLLIMPORT void build_indices(void);

extern PGDLLIMPORT void boot_get_type_io_data(Oid typid,
								  int16 *typlen,
								  bool *typbyval,
								  char *typalign,
								  char *typdelim,
								  Oid *typioparam,
								  Oid *typinput,
								  Oid *typoutput);

extern PGDLLIMPORT int	boot_yyparse(void);

extern PGDLLIMPORT int	boot_yylex(void);
extern PGDLLIMPORT void boot_yyerror(const char *str) pg_attribute_noreturn();

#endif							/* BOOTSTRAP_H */
