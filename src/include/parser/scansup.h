/*-------------------------------------------------------------------------
 *
 * scansup.h
 *	  scanner support routines used by the core lexer
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/parser/scansup.h
 *
 *-------------------------------------------------------------------------
 */

#ifndef SCANSUP_H
#define SCANSUP_H

extern PGDLLIMPORT char *downcase_truncate_identifier(const char *ident, int len,
										  bool warn);

extern PGDLLIMPORT char *downcase_identifier(const char *ident, int len,
								 bool warn, bool truncate);

extern PGDLLIMPORT void truncate_identifier(char *ident, int len, bool warn);

extern PGDLLIMPORT bool scanner_isspace(char ch);

#endif							/* SCANSUP_H */
