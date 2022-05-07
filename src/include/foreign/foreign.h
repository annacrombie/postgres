/*-------------------------------------------------------------------------
 *
 * foreign.h
 *	  support for foreign-data wrappers, servers and user mappings.
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 *
 * src/include/foreign/foreign.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef FOREIGN_H
#define FOREIGN_H

#include "nodes/parsenodes.h"


/* Helper for obtaining username for user mapping */
#define MappingUserName(userid) \
	(OidIsValid(userid) ? GetUserNameFromId(userid, false) : "public")


typedef struct ForeignDataWrapper
{
	Oid			fdwid;			/* FDW Oid */
	Oid			owner;			/* FDW owner user Oid */
	char	   *fdwname;		/* Name of the FDW */
	Oid			fdwhandler;		/* Oid of handler function, or 0 */
	Oid			fdwvalidator;	/* Oid of validator function, or 0 */
	List	   *options;		/* fdwoptions as DefElem list */
} ForeignDataWrapper;

typedef struct ForeignServer
{
	Oid			serverid;		/* server Oid */
	Oid			fdwid;			/* foreign-data wrapper */
	Oid			owner;			/* server owner user Oid */
	char	   *servername;		/* name of the server */
	char	   *servertype;		/* server type, optional */
	char	   *serverversion;	/* server version, optional */
	List	   *options;		/* srvoptions as DefElem list */
} ForeignServer;

typedef struct UserMapping
{
	Oid			umid;			/* Oid of user mapping */
	Oid			userid;			/* local user Oid */
	Oid			serverid;		/* server Oid */
	List	   *options;		/* useoptions as DefElem list */
} UserMapping;

typedef struct ForeignTable
{
	Oid			relid;			/* relation Oid */
	Oid			serverid;		/* server Oid */
	List	   *options;		/* ftoptions as DefElem list */
} ForeignTable;

/* Flags for GetForeignServerExtended */
#define FSV_MISSING_OK	0x01

/* Flags for GetForeignDataWrapperExtended */
#define FDW_MISSING_OK	0x01


extern PGDLLIMPORT ForeignServer *GetForeignServer(Oid serverid);
extern PGDLLIMPORT ForeignServer *GetForeignServerExtended(Oid serverid,
											   bits16 flags);
extern PGDLLIMPORT ForeignServer *GetForeignServerByName(const char *name, bool missing_ok);
extern PGDLLIMPORT UserMapping *GetUserMapping(Oid userid, Oid serverid);
extern PGDLLIMPORT ForeignDataWrapper *GetForeignDataWrapper(Oid fdwid);
extern PGDLLIMPORT ForeignDataWrapper *GetForeignDataWrapperExtended(Oid fdwid,
														 bits16 flags);
extern PGDLLIMPORT ForeignDataWrapper *GetForeignDataWrapperByName(const char *name,
													   bool missing_ok);
extern PGDLLIMPORT ForeignTable *GetForeignTable(Oid relid);

extern PGDLLIMPORT List *GetForeignColumnOptions(Oid relid, AttrNumber attnum);

extern PGDLLIMPORT Oid	get_foreign_data_wrapper_oid(const char *fdwname, bool missing_ok);
extern PGDLLIMPORT Oid	get_foreign_server_oid(const char *servername, bool missing_ok);

#endif							/* FOREIGN_H */
