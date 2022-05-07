/*-------------------------------------------------------------------------
 *
 * objectaddress.h
 *	  functions for working with object addresses
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/objectaddress.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef OBJECTADDRESS_H
#define OBJECTADDRESS_H

#include "access/htup.h"
#include "nodes/parsenodes.h"
#include "storage/lockdefs.h"
#include "utils/relcache.h"

/*
 * An ObjectAddress represents a database object of any type.
 */
typedef struct ObjectAddress
{
	Oid			classId;		/* Class Id from pg_class */
	Oid			objectId;		/* OID of the object */
	int32		objectSubId;	/* Subitem within object (eg column), or 0 */
} ObjectAddress;

extern PGDLLIMPORT const ObjectAddress InvalidObjectAddress;

#define ObjectAddressSubSet(addr, class_id, object_id, object_sub_id) \
	do { \
		(addr).classId = (class_id); \
		(addr).objectId = (object_id); \
		(addr).objectSubId = (object_sub_id); \
	} while (0)

#define ObjectAddressSet(addr, class_id, object_id) \
	ObjectAddressSubSet(addr, class_id, object_id, 0)

extern PGDLLIMPORT ObjectAddress get_object_address(ObjectType objtype, Node *object,
										Relation *relp,
										LOCKMODE lockmode, bool missing_ok);

extern PGDLLIMPORT ObjectAddress get_object_address_rv(ObjectType objtype, RangeVar *rel,
										   List *object, Relation *relp,
										   LOCKMODE lockmode, bool missing_ok);

extern PGDLLIMPORT void check_object_ownership(Oid roleid,
								   ObjectType objtype, ObjectAddress address,
								   Node *object, Relation relation);

extern PGDLLIMPORT Oid	get_object_namespace(const ObjectAddress *address);

extern PGDLLIMPORT bool is_objectclass_supported(Oid class_id);
extern PGDLLIMPORT const char *get_object_class_descr(Oid class_id);
extern PGDLLIMPORT Oid	get_object_oid_index(Oid class_id);
extern PGDLLIMPORT int	get_object_catcache_oid(Oid class_id);
extern PGDLLIMPORT int	get_object_catcache_name(Oid class_id);
extern PGDLLIMPORT AttrNumber get_object_attnum_oid(Oid class_id);
extern PGDLLIMPORT AttrNumber get_object_attnum_name(Oid class_id);
extern PGDLLIMPORT AttrNumber get_object_attnum_namespace(Oid class_id);
extern PGDLLIMPORT AttrNumber get_object_attnum_owner(Oid class_id);
extern PGDLLIMPORT AttrNumber get_object_attnum_acl(Oid class_id);
extern PGDLLIMPORT ObjectType get_object_type(Oid class_id, Oid object_id);
extern PGDLLIMPORT bool get_object_namensp_unique(Oid class_id);

extern PGDLLIMPORT HeapTuple get_catalog_object_by_oid(Relation catalog,
										   AttrNumber oidcol, Oid objectId);

extern PGDLLIMPORT char *getObjectDescription(const ObjectAddress *object,
								  bool missing_ok);
extern PGDLLIMPORT char *getObjectDescriptionOids(Oid classid, Oid objid);

extern PGDLLIMPORT int	read_objtype_from_string(const char *objtype);
extern PGDLLIMPORT char *getObjectTypeDescription(const ObjectAddress *object,
									  bool missing_ok);
extern PGDLLIMPORT char *getObjectIdentity(const ObjectAddress *address,
							   bool missing_ok);
extern PGDLLIMPORT char *getObjectIdentityParts(const ObjectAddress *address,
									List **objname, List **objargs,
									bool missing_ok);
extern PGDLLIMPORT struct ArrayType *strlist_to_textarray(List *list);

extern PGDLLIMPORT ObjectType get_relkind_objtype(char relkind);

#endif							/* OBJECTADDRESS_H */
