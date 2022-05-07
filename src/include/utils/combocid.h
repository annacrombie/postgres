/*-------------------------------------------------------------------------
 *
 * combocid.h
 *	  Combo command ID support routines
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/utils/combocid.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef COMBOCID_H
#define COMBOCID_H

/*
 * HeapTupleHeaderGetCmin and HeapTupleHeaderGetCmax function prototypes
 * are in access/htup.h, because that's where the macro definitions that
 * those functions replaced used to be.
 */

extern PGDLLIMPORT void AtEOXact_ComboCid(void);
extern PGDLLIMPORT void RestoreComboCIDState(char *comboCIDstate);
extern PGDLLIMPORT void SerializeComboCIDState(Size maxsize, char *start_address);
extern PGDLLIMPORT Size EstimateComboCIDStateSpace(void);

#endif							/* COMBOCID_H */
