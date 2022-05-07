/*-------------------------------------------------------------------------
 *
 * namespace.h
 *	  prototypes for functions in backend/catalog/namespace.c
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/namespace.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef NAMESPACE_H
#define NAMESPACE_H

#include "nodes/primnodes.h"
#include "storage/lock.h"


/*
 *	This structure holds a list of possible functions or operators
 *	found by namespace lookup.  Each function/operator is identified
 *	by OID and by argument types; the list must be pruned by type
 *	resolution rules that are embodied in the parser, not here.
 *	See FuncnameGetCandidates's comments for more info.
 */
typedef struct _FuncCandidateList
{
	struct _FuncCandidateList *next;
	int			pathpos;		/* for internal use of namespace lookup */
	Oid			oid;			/* the function or operator's OID */
	int			nominalnargs;	/* either pronargs or length(proallargtypes) */
	int			nargs;			/* number of arg types returned */
	int			nvargs;			/* number of args to become variadic array */
	int			ndargs;			/* number of defaulted args */
	int		   *argnumbers;		/* args' positional indexes, if named call */
	Oid			args[FLEXIBLE_ARRAY_MEMBER];	/* arg types */
}		   *FuncCandidateList;

/*
 * Result of checkTempNamespaceStatus
 */
typedef enum TempNamespaceStatus
{
	TEMP_NAMESPACE_NOT_TEMP,	/* nonexistent, or non-temp namespace */
	TEMP_NAMESPACE_IDLE,		/* exists, belongs to no active session */
	TEMP_NAMESPACE_IN_USE		/* belongs to some active session */
} TempNamespaceStatus;

/*
 *	Structure for xxxOverrideSearchPath functions
 *
 * The generation counter is private to namespace.c and shouldn't be touched
 * by other code.  It can be initialized to zero if necessary (that means
 * "not known equal to the current active path").
 */
typedef struct OverrideSearchPath
{
	List	   *schemas;		/* OIDs of explicitly named schemas */
	bool		addCatalog;		/* implicitly prepend pg_catalog? */
	bool		addTemp;		/* implicitly prepend temp schema? */
	uint64		generation;		/* for quick detection of equality to active */
} OverrideSearchPath;

/*
 * Option flag bits for RangeVarGetRelidExtended().
 */
typedef enum RVROption
{
	RVR_MISSING_OK = 1 << 0,	/* don't error if relation doesn't exist */
	RVR_NOWAIT = 1 << 1,		/* error if relation cannot be locked */
	RVR_SKIP_LOCKED = 1 << 2	/* skip if relation cannot be locked */
} RVROption;

typedef void (*RangeVarGetRelidCallback) (const RangeVar *relation, Oid relId,
										  Oid oldRelId, void *callback_arg);

#define RangeVarGetRelid(relation, lockmode, missing_ok) \
	RangeVarGetRelidExtended(relation, lockmode, \
							 (missing_ok) ? RVR_MISSING_OK : 0, NULL, NULL)

extern PGDLLIMPORT Oid	RangeVarGetRelidExtended(const RangeVar *relation,
									 LOCKMODE lockmode, uint32 flags,
									 RangeVarGetRelidCallback callback,
									 void *callback_arg);
extern PGDLLIMPORT Oid	RangeVarGetCreationNamespace(const RangeVar *newRelation);
extern PGDLLIMPORT Oid	RangeVarGetAndCheckCreationNamespace(RangeVar *newRelation,
												 LOCKMODE lockmode,
												 Oid *existing_relation_id);
extern PGDLLIMPORT void RangeVarAdjustRelationPersistence(RangeVar *newRelation, Oid nspid);
extern PGDLLIMPORT Oid	RelnameGetRelid(const char *relname);
extern PGDLLIMPORT bool RelationIsVisible(Oid relid);

extern PGDLLIMPORT Oid	TypenameGetTypid(const char *typname);
extern PGDLLIMPORT Oid	TypenameGetTypidExtended(const char *typname, bool temp_ok);
extern PGDLLIMPORT bool TypeIsVisible(Oid typid);

extern PGDLLIMPORT FuncCandidateList FuncnameGetCandidates(List *names,
											   int nargs, List *argnames,
											   bool expand_variadic,
											   bool expand_defaults,
											   bool include_out_arguments,
											   bool missing_ok);
extern PGDLLIMPORT bool FunctionIsVisible(Oid funcid);

extern PGDLLIMPORT Oid	OpernameGetOprid(List *names, Oid oprleft, Oid oprright);
extern PGDLLIMPORT FuncCandidateList OpernameGetCandidates(List *names, char oprkind,
											   bool missing_schema_ok);
extern PGDLLIMPORT bool OperatorIsVisible(Oid oprid);

extern PGDLLIMPORT Oid	OpclassnameGetOpcid(Oid amid, const char *opcname);
extern PGDLLIMPORT bool OpclassIsVisible(Oid opcid);

extern PGDLLIMPORT Oid	OpfamilynameGetOpfid(Oid amid, const char *opfname);
extern PGDLLIMPORT bool OpfamilyIsVisible(Oid opfid);

extern PGDLLIMPORT Oid	CollationGetCollid(const char *collname);
extern PGDLLIMPORT bool CollationIsVisible(Oid collid);

extern PGDLLIMPORT Oid	ConversionGetConid(const char *conname);
extern PGDLLIMPORT bool ConversionIsVisible(Oid conid);

extern PGDLLIMPORT Oid	get_statistics_object_oid(List *names, bool missing_ok);
extern PGDLLIMPORT bool StatisticsObjIsVisible(Oid relid);

extern PGDLLIMPORT Oid	get_ts_parser_oid(List *names, bool missing_ok);
extern PGDLLIMPORT bool TSParserIsVisible(Oid prsId);

extern PGDLLIMPORT Oid	get_ts_dict_oid(List *names, bool missing_ok);
extern PGDLLIMPORT bool TSDictionaryIsVisible(Oid dictId);

extern PGDLLIMPORT Oid	get_ts_template_oid(List *names, bool missing_ok);
extern PGDLLIMPORT bool TSTemplateIsVisible(Oid tmplId);

extern PGDLLIMPORT Oid	get_ts_config_oid(List *names, bool missing_ok);
extern PGDLLIMPORT bool TSConfigIsVisible(Oid cfgid);

extern PGDLLIMPORT void DeconstructQualifiedName(List *names,
									 char **nspname_p,
									 char **objname_p);
extern PGDLLIMPORT Oid	LookupNamespaceNoError(const char *nspname);
extern PGDLLIMPORT Oid	LookupExplicitNamespace(const char *nspname, bool missing_ok);
extern PGDLLIMPORT Oid	get_namespace_oid(const char *nspname, bool missing_ok);

extern PGDLLIMPORT Oid	LookupCreationNamespace(const char *nspname);
extern PGDLLIMPORT void CheckSetNamespace(Oid oldNspOid, Oid nspOid);
extern PGDLLIMPORT Oid	QualifiedNameGetCreationNamespace(List *names, char **objname_p);
extern PGDLLIMPORT RangeVar *makeRangeVarFromNameList(List *names);
extern PGDLLIMPORT char *NameListToString(List *names);
extern PGDLLIMPORT char *NameListToQuotedString(List *names);

extern PGDLLIMPORT bool isTempNamespace(Oid namespaceId);
extern PGDLLIMPORT bool isTempToastNamespace(Oid namespaceId);
extern PGDLLIMPORT bool isTempOrTempToastNamespace(Oid namespaceId);
extern PGDLLIMPORT bool isAnyTempNamespace(Oid namespaceId);
extern PGDLLIMPORT bool isOtherTempNamespace(Oid namespaceId);
extern PGDLLIMPORT TempNamespaceStatus checkTempNamespaceStatus(Oid namespaceId);
extern PGDLLIMPORT int	GetTempNamespaceBackendId(Oid namespaceId);
extern PGDLLIMPORT Oid	GetTempToastNamespace(void);
extern PGDLLIMPORT void GetTempNamespaceState(Oid *tempNamespaceId,
								  Oid *tempToastNamespaceId);
extern PGDLLIMPORT void SetTempNamespaceState(Oid tempNamespaceId,
								  Oid tempToastNamespaceId);
extern PGDLLIMPORT void ResetTempTableNamespace(void);

extern PGDLLIMPORT OverrideSearchPath *GetOverrideSearchPath(MemoryContext context);
extern PGDLLIMPORT OverrideSearchPath *CopyOverrideSearchPath(OverrideSearchPath *path);
extern PGDLLIMPORT bool OverrideSearchPathMatchesCurrent(OverrideSearchPath *path);
extern PGDLLIMPORT void PushOverrideSearchPath(OverrideSearchPath *newpath);
extern PGDLLIMPORT void PopOverrideSearchPath(void);

extern PGDLLIMPORT Oid	get_collation_oid(List *collname, bool missing_ok);
extern PGDLLIMPORT Oid	get_conversion_oid(List *conname, bool missing_ok);
extern PGDLLIMPORT Oid	FindDefaultConversionProc(int32 for_encoding, int32 to_encoding);


/* initialization & transaction cleanup code */
extern PGDLLIMPORT void InitializeSearchPath(void);
extern PGDLLIMPORT void AtEOXact_Namespace(bool isCommit, bool parallel);
extern PGDLLIMPORT void AtEOSubXact_Namespace(bool isCommit, SubTransactionId mySubid,
								  SubTransactionId parentSubid);

/* stuff for search_path GUC variable */
extern PGDLLIMPORT char *namespace_search_path;

extern PGDLLIMPORT List *fetch_search_path(bool includeImplicit);
extern PGDLLIMPORT int	fetch_search_path_array(Oid *sarray, int sarray_len);

#endif							/* NAMESPACE_H */
