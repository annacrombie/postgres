/*
 * variable.h
 *		Routines for handling specialized SET variables.
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/commands/variable.h
 */
#ifndef VARIABLE_H
#define VARIABLE_H

#include "utils/guc.h"


extern PGDLLIMPORT bool check_datestyle(char **newval, void **extra, GucSource source);
extern PGDLLIMPORT void assign_datestyle(const char *newval, void *extra);
extern PGDLLIMPORT bool check_timezone(char **newval, void **extra, GucSource source);
extern PGDLLIMPORT void assign_timezone(const char *newval, void *extra);
extern PGDLLIMPORT const char *show_timezone(void);
extern PGDLLIMPORT bool check_log_timezone(char **newval, void **extra, GucSource source);
extern PGDLLIMPORT void assign_log_timezone(const char *newval, void *extra);
extern PGDLLIMPORT const char *show_log_timezone(void);
extern PGDLLIMPORT bool check_transaction_read_only(bool *newval, void **extra, GucSource source);
extern PGDLLIMPORT bool check_XactIsoLevel(int *newval, void **extra, GucSource source);
extern PGDLLIMPORT bool check_transaction_deferrable(bool *newval, void **extra, GucSource source);
extern PGDLLIMPORT bool check_random_seed(double *newval, void **extra, GucSource source);
extern PGDLLIMPORT void assign_random_seed(double newval, void *extra);
extern PGDLLIMPORT const char *show_random_seed(void);
extern PGDLLIMPORT bool check_client_encoding(char **newval, void **extra, GucSource source);
extern PGDLLIMPORT void assign_client_encoding(const char *newval, void *extra);
extern PGDLLIMPORT bool check_session_authorization(char **newval, void **extra, GucSource source);
extern PGDLLIMPORT void assign_session_authorization(const char *newval, void *extra);
extern PGDLLIMPORT bool check_role(char **newval, void **extra, GucSource source);
extern PGDLLIMPORT void assign_role(const char *newval, void *extra);
extern PGDLLIMPORT const char *show_role(void);

#endif							/* VARIABLE_H */
