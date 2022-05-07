/*-------------------------------------------------------------------------
 *
 * event_trigger.h
 *	  Declarations for command trigger handling.
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/commands/event_trigger.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef EVENT_TRIGGER_H
#define EVENT_TRIGGER_H

#include "catalog/dependency.h"
#include "catalog/objectaddress.h"
#include "catalog/pg_event_trigger.h"
#include "nodes/parsenodes.h"
#include "tcop/cmdtag.h"
#include "tcop/deparse_utility.h"
#include "utils/aclchk_internal.h"

typedef struct EventTriggerData
{
	NodeTag		type;
	const char *event;			/* event name */
	Node	   *parsetree;		/* parse tree */
	CommandTag	tag;
} EventTriggerData;

#define AT_REWRITE_ALTER_PERSISTENCE	0x01
#define AT_REWRITE_DEFAULT_VAL			0x02
#define AT_REWRITE_COLUMN_REWRITE		0x04
#define AT_REWRITE_ACCESS_METHOD		0x08

/*
 * EventTriggerData is the node type that is passed as fmgr "context" info
 * when a function is called by the event trigger manager.
 */
#define CALLED_AS_EVENT_TRIGGER(fcinfo) \
	((fcinfo)->context != NULL && IsA((fcinfo)->context, EventTriggerData))

extern PGDLLIMPORT Oid	CreateEventTrigger(CreateEventTrigStmt *stmt);
extern PGDLLIMPORT Oid	get_event_trigger_oid(const char *trigname, bool missing_ok);

extern PGDLLIMPORT Oid	AlterEventTrigger(AlterEventTrigStmt *stmt);
extern PGDLLIMPORT ObjectAddress AlterEventTriggerOwner(const char *name, Oid newOwnerId);
extern PGDLLIMPORT void AlterEventTriggerOwner_oid(Oid, Oid newOwnerId);

extern PGDLLIMPORT bool EventTriggerSupportsObjectType(ObjectType obtype);
extern PGDLLIMPORT bool EventTriggerSupportsObjectClass(ObjectClass objclass);
extern PGDLLIMPORT void EventTriggerDDLCommandStart(Node *parsetree);
extern PGDLLIMPORT void EventTriggerDDLCommandEnd(Node *parsetree);
extern PGDLLIMPORT void EventTriggerSQLDrop(Node *parsetree);
extern PGDLLIMPORT void EventTriggerTableRewrite(Node *parsetree, Oid tableOid, int reason);

extern PGDLLIMPORT bool EventTriggerBeginCompleteQuery(void);
extern PGDLLIMPORT void EventTriggerEndCompleteQuery(void);
extern PGDLLIMPORT bool trackDroppedObjectsNeeded(void);
extern PGDLLIMPORT void EventTriggerSQLDropAddObject(const ObjectAddress *object,
										 bool original, bool normal);

extern PGDLLIMPORT void EventTriggerInhibitCommandCollection(void);
extern PGDLLIMPORT void EventTriggerUndoInhibitCommandCollection(void);

extern PGDLLIMPORT void EventTriggerCollectSimpleCommand(ObjectAddress address,
											 ObjectAddress secondaryObject,
											 Node *parsetree);

extern PGDLLIMPORT void EventTriggerAlterTableStart(Node *parsetree);
extern PGDLLIMPORT void EventTriggerAlterTableRelid(Oid objectId);
extern PGDLLIMPORT void EventTriggerCollectAlterTableSubcmd(Node *subcmd,
												ObjectAddress address);
extern PGDLLIMPORT void EventTriggerAlterTableEnd(void);

extern PGDLLIMPORT void EventTriggerCollectGrant(InternalGrant *istmt);
extern PGDLLIMPORT void EventTriggerCollectAlterOpFam(AlterOpFamilyStmt *stmt,
										  Oid opfamoid, List *operators,
										  List *procedures);
extern PGDLLIMPORT void EventTriggerCollectCreateOpClass(CreateOpClassStmt *stmt,
											 Oid opcoid, List *operators,
											 List *procedures);
extern PGDLLIMPORT void EventTriggerCollectAlterTSConfig(AlterTSConfigurationStmt *stmt,
											 Oid cfgId, Oid *dictIds, int ndicts);
extern PGDLLIMPORT void EventTriggerCollectAlterDefPrivs(AlterDefaultPrivilegesStmt *stmt);

#endif							/* EVENT_TRIGGER_H */
