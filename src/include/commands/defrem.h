/*-------------------------------------------------------------------------
 *
 * defrem.h
 *	  POSTGRES define and remove utility definitions.
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/commands/defrem.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef DEFREM_H
#define DEFREM_H

#include "catalog/objectaddress.h"
#include "nodes/params.h"
#include "parser/parse_node.h"
#include "tcop/dest.h"
#include "utils/array.h"

/* commands/dropcmds.c */
extern PGDLLIMPORT void RemoveObjects(DropStmt *stmt);

/* commands/indexcmds.c */
extern PGDLLIMPORT ObjectAddress DefineIndex(Oid relationId,
								 IndexStmt *stmt,
								 Oid indexRelationId,
								 Oid parentIndexId,
								 Oid parentConstraintId,
								 bool is_alter_table,
								 bool check_rights,
								 bool check_not_in_use,
								 bool skip_build,
								 bool quiet);
extern PGDLLIMPORT void ExecReindex(ParseState *pstate, ReindexStmt *stmt, bool isTopLevel);
extern PGDLLIMPORT char *makeObjectName(const char *name1, const char *name2,
							const char *label);
extern PGDLLIMPORT char *ChooseRelationName(const char *name1, const char *name2,
								const char *label, Oid namespaceid,
								bool isconstraint);
extern PGDLLIMPORT bool CheckIndexCompatible(Oid oldId,
								 const char *accessMethodName,
								 List *attributeList,
								 List *exclusionOpNames);
extern PGDLLIMPORT Oid	GetDefaultOpClass(Oid type_id, Oid am_id);
extern PGDLLIMPORT Oid	ResolveOpClass(List *opclass, Oid attrType,
						   const char *accessMethodName, Oid accessMethodId);

/* commands/functioncmds.c */
extern PGDLLIMPORT ObjectAddress CreateFunction(ParseState *pstate, CreateFunctionStmt *stmt);
extern PGDLLIMPORT void RemoveFunctionById(Oid funcOid);
extern PGDLLIMPORT ObjectAddress AlterFunction(ParseState *pstate, AlterFunctionStmt *stmt);
extern PGDLLIMPORT ObjectAddress CreateCast(CreateCastStmt *stmt);
extern PGDLLIMPORT ObjectAddress CreateTransform(CreateTransformStmt *stmt);
extern PGDLLIMPORT void IsThereFunctionInNamespace(const char *proname, int pronargs,
									   oidvector *proargtypes, Oid nspOid);
extern PGDLLIMPORT void ExecuteDoStmt(ParseState *pstate, DoStmt *stmt, bool atomic);
extern PGDLLIMPORT void ExecuteCallStmt(CallStmt *stmt, ParamListInfo params, bool atomic, DestReceiver *dest);
extern PGDLLIMPORT TupleDesc CallStmtResultDesc(CallStmt *stmt);
extern PGDLLIMPORT Oid	get_transform_oid(Oid type_id, Oid lang_id, bool missing_ok);
extern PGDLLIMPORT void interpret_function_parameter_list(ParseState *pstate,
											  List *parameters,
											  Oid languageOid,
											  ObjectType objtype,
											  oidvector **parameterTypes,
											  List **parameterTypes_list,
											  ArrayType **allParameterTypes,
											  ArrayType **parameterModes,
											  ArrayType **parameterNames,
											  List **inParameterNames_list,
											  List **parameterDefaults,
											  Oid *variadicArgType,
											  Oid *requiredResultType);

/* commands/operatorcmds.c */
extern PGDLLIMPORT ObjectAddress DefineOperator(List *names, List *parameters);
extern PGDLLIMPORT void RemoveOperatorById(Oid operOid);
extern PGDLLIMPORT ObjectAddress AlterOperator(AlterOperatorStmt *stmt);

/* commands/statscmds.c */
extern PGDLLIMPORT ObjectAddress CreateStatistics(CreateStatsStmt *stmt);
extern PGDLLIMPORT ObjectAddress AlterStatistics(AlterStatsStmt *stmt);
extern PGDLLIMPORT void RemoveStatisticsById(Oid statsOid);
extern PGDLLIMPORT void RemoveStatisticsDataById(Oid statsOid, bool inh);
extern PGDLLIMPORT Oid	StatisticsGetRelation(Oid statId, bool missing_ok);

/* commands/aggregatecmds.c */
extern PGDLLIMPORT ObjectAddress DefineAggregate(ParseState *pstate, List *name, List *args, bool oldstyle,
									 List *parameters, bool replace);

/* commands/opclasscmds.c */
extern PGDLLIMPORT ObjectAddress DefineOpClass(CreateOpClassStmt *stmt);
extern PGDLLIMPORT ObjectAddress DefineOpFamily(CreateOpFamilyStmt *stmt);
extern PGDLLIMPORT Oid	AlterOpFamily(AlterOpFamilyStmt *stmt);
extern PGDLLIMPORT void IsThereOpClassInNamespace(const char *opcname, Oid opcmethod,
									  Oid opcnamespace);
extern PGDLLIMPORT void IsThereOpFamilyInNamespace(const char *opfname, Oid opfmethod,
									   Oid opfnamespace);
extern PGDLLIMPORT Oid	get_opclass_oid(Oid amID, List *opclassname, bool missing_ok);
extern PGDLLIMPORT Oid	get_opfamily_oid(Oid amID, List *opfamilyname, bool missing_ok);

/* commands/tsearchcmds.c */
extern PGDLLIMPORT ObjectAddress DefineTSParser(List *names, List *parameters);

extern PGDLLIMPORT ObjectAddress DefineTSDictionary(List *names, List *parameters);
extern PGDLLIMPORT ObjectAddress AlterTSDictionary(AlterTSDictionaryStmt *stmt);

extern PGDLLIMPORT ObjectAddress DefineTSTemplate(List *names, List *parameters);

extern PGDLLIMPORT ObjectAddress DefineTSConfiguration(List *names, List *parameters,
										   ObjectAddress *copied);
extern PGDLLIMPORT void RemoveTSConfigurationById(Oid cfgId);
extern PGDLLIMPORT ObjectAddress AlterTSConfiguration(AlterTSConfigurationStmt *stmt);

extern PGDLLIMPORT text *serialize_deflist(List *deflist);
extern PGDLLIMPORT List *deserialize_deflist(Datum txt);

/* commands/foreigncmds.c */
extern PGDLLIMPORT ObjectAddress AlterForeignServerOwner(const char *name, Oid newOwnerId);
extern PGDLLIMPORT void AlterForeignServerOwner_oid(Oid, Oid newOwnerId);
extern PGDLLIMPORT ObjectAddress AlterForeignDataWrapperOwner(const char *name, Oid newOwnerId);
extern PGDLLIMPORT void AlterForeignDataWrapperOwner_oid(Oid fwdId, Oid newOwnerId);
extern PGDLLIMPORT ObjectAddress CreateForeignDataWrapper(ParseState *pstate, CreateFdwStmt *stmt);
extern PGDLLIMPORT ObjectAddress AlterForeignDataWrapper(ParseState *pstate, AlterFdwStmt *stmt);
extern PGDLLIMPORT ObjectAddress CreateForeignServer(CreateForeignServerStmt *stmt);
extern PGDLLIMPORT ObjectAddress AlterForeignServer(AlterForeignServerStmt *stmt);
extern PGDLLIMPORT ObjectAddress CreateUserMapping(CreateUserMappingStmt *stmt);
extern PGDLLIMPORT ObjectAddress AlterUserMapping(AlterUserMappingStmt *stmt);
extern PGDLLIMPORT Oid	RemoveUserMapping(DropUserMappingStmt *stmt);
extern PGDLLIMPORT void CreateForeignTable(CreateForeignTableStmt *stmt, Oid relid);
extern PGDLLIMPORT void ImportForeignSchema(ImportForeignSchemaStmt *stmt);
extern PGDLLIMPORT Datum transformGenericOptions(Oid catalogId,
									 Datum oldOptions,
									 List *options,
									 Oid fdwvalidator);

/* commands/amcmds.c */
extern PGDLLIMPORT ObjectAddress CreateAccessMethod(CreateAmStmt *stmt);
extern PGDLLIMPORT Oid	get_index_am_oid(const char *amname, bool missing_ok);
extern PGDLLIMPORT Oid	get_table_am_oid(const char *amname, bool missing_ok);
extern PGDLLIMPORT Oid	get_am_oid(const char *amname, bool missing_ok);
extern PGDLLIMPORT char *get_am_name(Oid amOid);

/* support routines in commands/define.c */

extern PGDLLIMPORT char *defGetString(DefElem *def);
extern PGDLLIMPORT double defGetNumeric(DefElem *def);
extern PGDLLIMPORT bool defGetBoolean(DefElem *def);
extern PGDLLIMPORT int32 defGetInt32(DefElem *def);
extern PGDLLIMPORT int64 defGetInt64(DefElem *def);
extern PGDLLIMPORT List *defGetQualifiedName(DefElem *def);
extern PGDLLIMPORT TypeName *defGetTypeName(DefElem *def);
extern PGDLLIMPORT int	defGetTypeLength(DefElem *def);
extern PGDLLIMPORT List *defGetStringList(DefElem *def);
extern PGDLLIMPORT void errorConflictingDefElem(DefElem *defel, ParseState *pstate) pg_attribute_noreturn();

#endif							/* DEFREM_H */
