/*-------------------------------------------------------------------------
 *
 * copydir.h
 *	  Copy a directory.
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/storage/copydir.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef COPYDIR_H
#define COPYDIR_H

extern PGDLLIMPORT void copydir(char *fromdir, char *todir, bool recurse);
extern PGDLLIMPORT void copy_file(char *fromfile, char *tofile);

#endif							/* COPYDIR_H */
