/*-------------------------------------------------------------------------
 *
 * toasting.h
 *	  This file provides some definitions to support creation of toast tables
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/toasting.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef TOASTING_H
#define TOASTING_H

#include "storage/lock.h"

/*
 * toasting.c prototypes
 */
extern PGDLLIMPORT void NewRelationCreateToastTable(Oid relOid, Datum reloptions);
extern PGDLLIMPORT void NewHeapCreateToastTable(Oid relOid, Datum reloptions,
									LOCKMODE lockmode, Oid OIDOldToast);
extern PGDLLIMPORT void AlterTableCreateToastTable(Oid relOid, Datum reloptions,
									   LOCKMODE lockmode);
extern PGDLLIMPORT void BootstrapToastTable(char *relName,
								Oid toastOid, Oid toastIndexOid);

#endif							/* TOASTING_H */
