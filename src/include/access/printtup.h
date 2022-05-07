/*-------------------------------------------------------------------------
 *
 * printtup.h
 *
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/access/printtup.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PRINTTUP_H
#define PRINTTUP_H

#include "utils/portal.h"

extern PGDLLIMPORT DestReceiver *printtup_create_DR(CommandDest dest);

extern PGDLLIMPORT void SetRemoteDestReceiverParams(DestReceiver *self, Portal portal);

extern PGDLLIMPORT void SendRowDescriptionMessage(StringInfo buf,
									  TupleDesc typeinfo, List *targetlist, int16 *formats);

extern PGDLLIMPORT void debugStartup(DestReceiver *self, int operation,
						 TupleDesc typeinfo);
extern PGDLLIMPORT bool debugtup(TupleTableSlot *slot, DestReceiver *self);

/* XXX these are really in executor/spi.c */
extern PGDLLIMPORT void spi_dest_startup(DestReceiver *self, int operation,
							 TupleDesc typeinfo);
extern PGDLLIMPORT bool spi_printtup(TupleTableSlot *slot, DestReceiver *self);

#endif							/* PRINTTUP_H */
