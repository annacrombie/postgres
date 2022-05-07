/*-------------------------------------------------------------------------
 *
 * parse_type.h
 *		handle type operations for parser
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/parser/parse_type.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PARSE_TYPE_H
#define PARSE_TYPE_H

#include "access/htup.h"
#include "parser/parse_node.h"


typedef HeapTuple Type;

extern PGDLLIMPORT Type LookupTypeName(ParseState *pstate, const TypeName *typeName,
						   int32 *typmod_p, bool missing_ok);
extern PGDLLIMPORT Type LookupTypeNameExtended(ParseState *pstate,
								   const TypeName *typeName, int32 *typmod_p,
								   bool temp_ok, bool missing_ok);
extern PGDLLIMPORT Oid	LookupTypeNameOid(ParseState *pstate, const TypeName *typeName,
							  bool missing_ok);
extern PGDLLIMPORT Type typenameType(ParseState *pstate, const TypeName *typeName,
						 int32 *typmod_p);
extern PGDLLIMPORT Oid	typenameTypeId(ParseState *pstate, const TypeName *typeName);
extern PGDLLIMPORT void typenameTypeIdAndMod(ParseState *pstate, const TypeName *typeName,
								 Oid *typeid_p, int32 *typmod_p);

extern PGDLLIMPORT char *TypeNameToString(const TypeName *typeName);
extern PGDLLIMPORT char *TypeNameListToString(List *typenames);

extern PGDLLIMPORT Oid	LookupCollation(ParseState *pstate, List *collnames, int location);
extern PGDLLIMPORT Oid	GetColumnDefCollation(ParseState *pstate, ColumnDef *coldef, Oid typeOid);

extern PGDLLIMPORT Type typeidType(Oid id);

extern PGDLLIMPORT Oid	typeTypeId(Type tp);
extern PGDLLIMPORT int16 typeLen(Type t);
extern PGDLLIMPORT bool typeByVal(Type t);
extern PGDLLIMPORT char *typeTypeName(Type t);
extern PGDLLIMPORT Oid	typeTypeRelid(Type typ);
extern PGDLLIMPORT Oid	typeTypeCollation(Type typ);
extern PGDLLIMPORT Datum stringTypeDatum(Type tp, char *string, int32 atttypmod);

extern PGDLLIMPORT Oid	typeidTypeRelid(Oid type_id);
extern PGDLLIMPORT Oid	typeOrDomainTypeRelid(Oid type_id);

extern PGDLLIMPORT TypeName *typeStringToTypeName(const char *str);
extern PGDLLIMPORT void parseTypeString(const char *str, Oid *typeid_p, int32 *typmod_p, bool missing_ok);

/* true if typeid is composite, or domain over composite, but not RECORD */
#define ISCOMPLEX(typeid) (typeOrDomainTypeRelid(typeid) != InvalidOid)

#endif							/* PARSE_TYPE_H */
