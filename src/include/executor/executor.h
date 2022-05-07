/*-------------------------------------------------------------------------
 *
 * executor.h
 *	  support for the POSTGRES executor module
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/executor/executor.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "executor/execdesc.h"
#include "fmgr.h"
#include "nodes/lockoptions.h"
#include "nodes/parsenodes.h"
#include "utils/memutils.h"


/*
 * The "eflags" argument to ExecutorStart and the various ExecInitNode
 * routines is a bitwise OR of the following flag bits, which tell the
 * called plan node what to expect.  Note that the flags will get modified
 * as they are passed down the plan tree, since an upper node may require
 * functionality in its subnode not demanded of the plan as a whole
 * (example: MergeJoin requires mark/restore capability in its inner input),
 * or an upper node may shield its input from some functionality requirement
 * (example: Materialize shields its input from needing to do backward scan).
 *
 * EXPLAIN_ONLY indicates that the plan tree is being initialized just so
 * EXPLAIN can print it out; it will not be run.  Hence, no side-effects
 * of startup should occur.  However, error checks (such as permission checks)
 * should be performed.
 *
 * REWIND indicates that the plan node should try to efficiently support
 * rescans without parameter changes.  (Nodes must support ExecReScan calls
 * in any case, but if this flag was not given, they are at liberty to do it
 * through complete recalculation.  Note that a parameter change forces a
 * full recalculation in any case.)
 *
 * BACKWARD indicates that the plan node must respect the es_direction flag.
 * When this is not passed, the plan node will only be run forwards.
 *
 * MARK indicates that the plan node must support Mark/Restore calls.
 * When this is not passed, no Mark/Restore will occur.
 *
 * SKIP_TRIGGERS tells ExecutorStart/ExecutorFinish to skip calling
 * AfterTriggerBeginQuery/AfterTriggerEndQuery.  This does not necessarily
 * mean that the plan can't queue any AFTER triggers; just that the caller
 * is responsible for there being a trigger context for them to be queued in.
 */
#define EXEC_FLAG_EXPLAIN_ONLY	0x0001	/* EXPLAIN, no ANALYZE */
#define EXEC_FLAG_REWIND		0x0002	/* need efficient rescan */
#define EXEC_FLAG_BACKWARD		0x0004	/* need backward scan */
#define EXEC_FLAG_MARK			0x0008	/* need mark/restore */
#define EXEC_FLAG_SKIP_TRIGGERS 0x0010	/* skip AfterTrigger calls */
#define EXEC_FLAG_WITH_NO_DATA	0x0020	/* rel scannability doesn't matter */


/* Hook for plugins to get control in ExecutorStart() */
typedef void (*ExecutorStart_hook_type) (QueryDesc *queryDesc, int eflags);
extern PGDLLIMPORT ExecutorStart_hook_type ExecutorStart_hook;

/* Hook for plugins to get control in ExecutorRun() */
typedef void (*ExecutorRun_hook_type) (QueryDesc *queryDesc,
									   ScanDirection direction,
									   uint64 count,
									   bool execute_once);
extern PGDLLIMPORT ExecutorRun_hook_type ExecutorRun_hook;

/* Hook for plugins to get control in ExecutorFinish() */
typedef void (*ExecutorFinish_hook_type) (QueryDesc *queryDesc);
extern PGDLLIMPORT ExecutorFinish_hook_type ExecutorFinish_hook;

/* Hook for plugins to get control in ExecutorEnd() */
typedef void (*ExecutorEnd_hook_type) (QueryDesc *queryDesc);
extern PGDLLIMPORT ExecutorEnd_hook_type ExecutorEnd_hook;

/* Hook for plugins to get control in ExecCheckRTPerms() */
typedef bool (*ExecutorCheckPerms_hook_type) (List *, bool);
extern PGDLLIMPORT ExecutorCheckPerms_hook_type ExecutorCheckPerms_hook;


/*
 * prototypes from functions in execAmi.c
 */
struct Path;					/* avoid including pathnodes.h here */

extern PGDLLIMPORT void ExecReScan(PlanState *node);
extern PGDLLIMPORT void ExecMarkPos(PlanState *node);
extern PGDLLIMPORT void ExecRestrPos(PlanState *node);
extern PGDLLIMPORT bool ExecSupportsMarkRestore(struct Path *pathnode);
extern PGDLLIMPORT bool ExecSupportsBackwardScan(Plan *node);
extern PGDLLIMPORT bool ExecMaterializesOutput(NodeTag plantype);

/*
 * prototypes from functions in execCurrent.c
 */
extern PGDLLIMPORT bool execCurrentOf(CurrentOfExpr *cexpr,
						  ExprContext *econtext,
						  Oid table_oid,
						  ItemPointer current_tid);

/*
 * prototypes from functions in execGrouping.c
 */
extern PGDLLIMPORT ExprState *execTuplesMatchPrepare(TupleDesc desc,
										 int numCols,
										 const AttrNumber *keyColIdx,
										 const Oid *eqOperators,
										 const Oid *collations,
										 PlanState *parent);
extern PGDLLIMPORT void execTuplesHashPrepare(int numCols,
								  const Oid *eqOperators,
								  Oid **eqFuncOids,
								  FmgrInfo **hashFunctions);
extern PGDLLIMPORT TupleHashTable BuildTupleHashTable(PlanState *parent,
										  TupleDesc inputDesc,
										  int numCols, AttrNumber *keyColIdx,
										  const Oid *eqfuncoids,
										  FmgrInfo *hashfunctions,
										  Oid *collations,
										  long nbuckets, Size additionalsize,
										  MemoryContext tablecxt,
										  MemoryContext tempcxt, bool use_variable_hash_iv);
extern PGDLLIMPORT TupleHashTable BuildTupleHashTableExt(PlanState *parent,
											 TupleDesc inputDesc,
											 int numCols, AttrNumber *keyColIdx,
											 const Oid *eqfuncoids,
											 FmgrInfo *hashfunctions,
											 Oid *collations,
											 long nbuckets, Size additionalsize,
											 MemoryContext metacxt,
											 MemoryContext tablecxt,
											 MemoryContext tempcxt, bool use_variable_hash_iv);
extern PGDLLIMPORT TupleHashEntry LookupTupleHashEntry(TupleHashTable hashtable,
										   TupleTableSlot *slot,
										   bool *isnew, uint32 *hash);
extern PGDLLIMPORT uint32 TupleHashTableHash(TupleHashTable hashtable,
								 TupleTableSlot *slot);
extern PGDLLIMPORT TupleHashEntry LookupTupleHashEntryHash(TupleHashTable hashtable,
											   TupleTableSlot *slot,
											   bool *isnew, uint32 hash);
extern PGDLLIMPORT TupleHashEntry FindTupleHashEntry(TupleHashTable hashtable,
										 TupleTableSlot *slot,
										 ExprState *eqcomp,
										 FmgrInfo *hashfunctions);
extern PGDLLIMPORT void ResetTupleHashTable(TupleHashTable hashtable);

/*
 * prototypes from functions in execJunk.c
 */
extern PGDLLIMPORT JunkFilter *ExecInitJunkFilter(List *targetList,
									  TupleTableSlot *slot);
extern PGDLLIMPORT JunkFilter *ExecInitJunkFilterConversion(List *targetList,
												TupleDesc cleanTupType,
												TupleTableSlot *slot);
extern PGDLLIMPORT AttrNumber ExecFindJunkAttribute(JunkFilter *junkfilter,
										const char *attrName);
extern PGDLLIMPORT AttrNumber ExecFindJunkAttributeInTlist(List *targetlist,
											   const char *attrName);
extern PGDLLIMPORT TupleTableSlot *ExecFilterJunk(JunkFilter *junkfilter,
									  TupleTableSlot *slot);

/*
 * ExecGetJunkAttribute
 *
 * Given a junk filter's input tuple (slot) and a junk attribute's number
 * previously found by ExecFindJunkAttribute, extract & return the value and
 * isNull flag of the attribute.
 */
#ifndef FRONTEND
static inline Datum
ExecGetJunkAttribute(TupleTableSlot *slot, AttrNumber attno, bool *isNull)
{
	Assert(attno > 0);
	return slot_getattr(slot, attno, isNull);
}
#endif

/*
 * prototypes from functions in execMain.c
 */
extern PGDLLIMPORT void ExecutorStart(QueryDesc *queryDesc, int eflags);
extern PGDLLIMPORT void standard_ExecutorStart(QueryDesc *queryDesc, int eflags);
extern PGDLLIMPORT void ExecutorRun(QueryDesc *queryDesc,
						ScanDirection direction, uint64 count, bool execute_once);
extern PGDLLIMPORT void standard_ExecutorRun(QueryDesc *queryDesc,
								 ScanDirection direction, uint64 count, bool execute_once);
extern PGDLLIMPORT void ExecutorFinish(QueryDesc *queryDesc);
extern PGDLLIMPORT void standard_ExecutorFinish(QueryDesc *queryDesc);
extern PGDLLIMPORT void ExecutorEnd(QueryDesc *queryDesc);
extern PGDLLIMPORT void standard_ExecutorEnd(QueryDesc *queryDesc);
extern PGDLLIMPORT void ExecutorRewind(QueryDesc *queryDesc);
extern PGDLLIMPORT bool ExecCheckRTPerms(List *rangeTable, bool ereport_on_violation);
extern PGDLLIMPORT void CheckValidResultRel(ResultRelInfo *resultRelInfo, CmdType operation);
extern PGDLLIMPORT void InitResultRelInfo(ResultRelInfo *resultRelInfo,
							  Relation resultRelationDesc,
							  Index resultRelationIndex,
							  ResultRelInfo *partition_root_rri,
							  int instrument_options);
extern PGDLLIMPORT ResultRelInfo *ExecGetTriggerResultRel(EState *estate, Oid relid,
											  ResultRelInfo *rootRelInfo);
extern PGDLLIMPORT List *ExecGetAncestorResultRels(EState *estate, ResultRelInfo *resultRelInfo);
extern PGDLLIMPORT void ExecConstraints(ResultRelInfo *resultRelInfo,
							TupleTableSlot *slot, EState *estate);
extern PGDLLIMPORT bool ExecPartitionCheck(ResultRelInfo *resultRelInfo,
							   TupleTableSlot *slot, EState *estate, bool emitError);
extern PGDLLIMPORT void ExecPartitionCheckEmitError(ResultRelInfo *resultRelInfo,
										TupleTableSlot *slot, EState *estate);
extern PGDLLIMPORT void ExecWithCheckOptions(WCOKind kind, ResultRelInfo *resultRelInfo,
								 TupleTableSlot *slot, EState *estate);
extern PGDLLIMPORT LockTupleMode ExecUpdateLockMode(EState *estate, ResultRelInfo *relinfo);
extern PGDLLIMPORT ExecRowMark *ExecFindRowMark(EState *estate, Index rti, bool missing_ok);
extern PGDLLIMPORT ExecAuxRowMark *ExecBuildAuxRowMark(ExecRowMark *erm, List *targetlist);
extern PGDLLIMPORT TupleTableSlot *EvalPlanQual(EPQState *epqstate, Relation relation,
									Index rti, TupleTableSlot *testslot);
extern PGDLLIMPORT void EvalPlanQualInit(EPQState *epqstate, EState *parentestate,
							 Plan *subplan, List *auxrowmarks, int epqParam);
extern PGDLLIMPORT void EvalPlanQualSetPlan(EPQState *epqstate,
								Plan *subplan, List *auxrowmarks);
extern PGDLLIMPORT TupleTableSlot *EvalPlanQualSlot(EPQState *epqstate,
										Relation relation, Index rti);

#define EvalPlanQualSetSlot(epqstate, slot)  ((epqstate)->origslot = (slot))
extern PGDLLIMPORT bool EvalPlanQualFetchRowMark(EPQState *epqstate, Index rti, TupleTableSlot *slot);
extern PGDLLIMPORT TupleTableSlot *EvalPlanQualNext(EPQState *epqstate);
extern PGDLLIMPORT void EvalPlanQualBegin(EPQState *epqstate);
extern PGDLLIMPORT void EvalPlanQualEnd(EPQState *epqstate);

/*
 * functions in execProcnode.c
 */
extern PGDLLIMPORT PlanState *ExecInitNode(Plan *node, EState *estate, int eflags);
extern PGDLLIMPORT void ExecSetExecProcNode(PlanState *node, ExecProcNodeMtd function);
extern PGDLLIMPORT Node *MultiExecProcNode(PlanState *node);
extern PGDLLIMPORT void ExecEndNode(PlanState *node);
extern PGDLLIMPORT bool ExecShutdownNode(PlanState *node);
extern PGDLLIMPORT void ExecSetTupleBound(int64 tuples_needed, PlanState *child_node);


/* ----------------------------------------------------------------
 *		ExecProcNode
 *
 *		Execute the given node to return a(nother) tuple.
 * ----------------------------------------------------------------
 */
#ifndef FRONTEND
static inline TupleTableSlot *
ExecProcNode(PlanState *node)
{
	if (node->chgParam != NULL) /* something changed? */
		ExecReScan(node);		/* let ReScan handle this */

	return node->ExecProcNode(node);
}
#endif

/*
 * prototypes from functions in execExpr.c
 */
extern PGDLLIMPORT ExprState *ExecInitExpr(Expr *node, PlanState *parent);
extern PGDLLIMPORT ExprState *ExecInitExprWithParams(Expr *node, ParamListInfo ext_params);
extern PGDLLIMPORT ExprState *ExecInitExprWithCaseValue(Expr *node, PlanState *parent,
						  Datum *caseval, bool *casenull);
extern PGDLLIMPORT ExprState *ExecInitQual(List *qual, PlanState *parent);
extern PGDLLIMPORT ExprState *ExecInitCheck(List *qual, PlanState *parent);
extern PGDLLIMPORT List *ExecInitExprList(List *nodes, PlanState *parent);
extern PGDLLIMPORT ExprState *ExecBuildAggTrans(AggState *aggstate, struct AggStatePerPhaseData *phase,
									bool doSort, bool doHash, bool nullcheck);
extern PGDLLIMPORT ExprState *ExecBuildGroupingEqual(TupleDesc ldesc, TupleDesc rdesc,
										 const TupleTableSlotOps *lops, const TupleTableSlotOps *rops,
										 int numCols,
										 const AttrNumber *keyColIdx,
										 const Oid *eqfunctions,
										 const Oid *collations,
										 PlanState *parent);
extern PGDLLIMPORT ExprState *ExecBuildParamSetEqual(TupleDesc desc,
										 const TupleTableSlotOps *lops,
										 const TupleTableSlotOps *rops,
										 const Oid *eqfunctions,
										 const Oid *collations,
										 const List *param_exprs,
										 PlanState *parent);
extern PGDLLIMPORT ProjectionInfo *ExecBuildProjectionInfo(List *targetList,
											   ExprContext *econtext,
											   TupleTableSlot *slot,
											   PlanState *parent,
											   TupleDesc inputDesc);
extern PGDLLIMPORT ProjectionInfo *ExecBuildUpdateProjection(List *targetList,
												 bool evalTargetList,
												 List *targetColnos,
												 TupleDesc relDesc,
												 ExprContext *econtext,
												 TupleTableSlot *slot,
												 PlanState *parent);
extern PGDLLIMPORT ExprState *ExecPrepareExpr(Expr *node, EState *estate);
extern PGDLLIMPORT ExprState *ExecPrepareQual(List *qual, EState *estate);
extern PGDLLIMPORT ExprState *ExecPrepareCheck(List *qual, EState *estate);
extern PGDLLIMPORT List *ExecPrepareExprList(List *nodes, EState *estate);

/*
 * ExecEvalExpr
 *
 * Evaluate expression identified by "state" in the execution context
 * given by "econtext".  *isNull is set to the is-null flag for the result,
 * and the Datum value is the function result.
 *
 * The caller should already have switched into the temporary memory
 * context econtext->ecxt_per_tuple_memory.  The convenience entry point
 * ExecEvalExprSwitchContext() is provided for callers who don't prefer to
 * do the switch in an outer loop.
 */
#ifndef FRONTEND
static inline Datum
ExecEvalExpr(ExprState *state,
			 ExprContext *econtext,
			 bool *isNull)
{
	return state->evalfunc(state, econtext, isNull);
}
#endif

/*
 * ExecEvalExprSwitchContext
 *
 * Same as ExecEvalExpr, but get into the right allocation context explicitly.
 */
#ifndef FRONTEND
static inline Datum
ExecEvalExprSwitchContext(ExprState *state,
						  ExprContext *econtext,
						  bool *isNull)
{
	Datum		retDatum;
	MemoryContext oldContext;

	oldContext = MemoryContextSwitchTo(econtext->ecxt_per_tuple_memory);
	retDatum = state->evalfunc(state, econtext, isNull);
	MemoryContextSwitchTo(oldContext);
	return retDatum;
}
#endif

/*
 * ExecProject
 *
 * Projects a tuple based on projection info and stores it in the slot passed
 * to ExecBuildProjectionInfo().
 *
 * Note: the result is always a virtual tuple; therefore it may reference
 * the contents of the exprContext's scan tuples and/or temporary results
 * constructed in the exprContext.  If the caller wishes the result to be
 * valid longer than that data will be valid, he must call ExecMaterializeSlot
 * on the result slot.
 */
#ifndef FRONTEND
static inline TupleTableSlot *
ExecProject(ProjectionInfo *projInfo)
{
	ExprContext *econtext = projInfo->pi_exprContext;
	ExprState  *state = &projInfo->pi_state;
	TupleTableSlot *slot = state->resultslot;
	bool		isnull;

	/*
	 * Clear any former contents of the result slot.  This makes it safe for
	 * us to use the slot's Datum/isnull arrays as workspace.
	 */
	ExecClearTuple(slot);

	/* Run the expression, discarding scalar result from the last column. */
	(void) ExecEvalExprSwitchContext(state, econtext, &isnull);

	/*
	 * Successfully formed a result row.  Mark the result slot as containing a
	 * valid virtual tuple (inlined version of ExecStoreVirtualTuple()).
	 */
	slot->tts_flags &= ~TTS_FLAG_EMPTY;
	slot->tts_nvalid = slot->tts_tupleDescriptor->natts;

	return slot;
}
#endif

/*
 * ExecQual - evaluate a qual prepared with ExecInitQual (possibly via
 * ExecPrepareQual).  Returns true if qual is satisfied, else false.
 *
 * Note: ExecQual used to have a third argument "resultForNull".  The
 * behavior of this function now corresponds to resultForNull == false.
 * If you want the resultForNull == true behavior, see ExecCheck.
 */
#ifndef FRONTEND
static inline bool
ExecQual(ExprState *state, ExprContext *econtext)
{
	Datum		ret;
	bool		isnull;

	/* short-circuit (here and in ExecInitQual) for empty restriction list */
	if (state == NULL)
		return true;

	/* verify that expression was compiled using ExecInitQual */
	Assert(state->flags & EEO_FLAG_IS_QUAL);

	ret = ExecEvalExprSwitchContext(state, econtext, &isnull);

	/* EEOP_QUAL should never return NULL */
	Assert(!isnull);

	return DatumGetBool(ret);
}
#endif

/*
 * ExecQualAndReset() - evaluate qual with ExecQual() and reset expression
 * context.
 */
#ifndef FRONTEND
static inline bool
ExecQualAndReset(ExprState *state, ExprContext *econtext)
{
	bool		ret = ExecQual(state, econtext);

	/* inline ResetExprContext, to avoid ordering issue in this file */
	MemoryContextReset(econtext->ecxt_per_tuple_memory);
	return ret;
}
#endif

extern PGDLLIMPORT bool ExecCheck(ExprState *state, ExprContext *context);

/*
 * prototypes from functions in execSRF.c
 */
extern PGDLLIMPORT SetExprState *ExecInitTableFunctionResult(Expr *expr,
												 ExprContext *econtext, PlanState *parent);
extern PGDLLIMPORT Tuplestorestate *ExecMakeTableFunctionResult(SetExprState *setexpr,
													ExprContext *econtext,
													MemoryContext argContext,
													TupleDesc expectedDesc,
													bool randomAccess);
extern PGDLLIMPORT SetExprState *ExecInitFunctionResultSet(Expr *expr,
											   ExprContext *econtext, PlanState *parent);
extern PGDLLIMPORT Datum ExecMakeFunctionResultSet(SetExprState *fcache,
									   ExprContext *econtext,
									   MemoryContext argContext,
									   bool *isNull,
									   ExprDoneCond *isDone);

/*
 * prototypes from functions in execScan.c
 */
typedef TupleTableSlot *(*ExecScanAccessMtd) (ScanState *node);
typedef bool (*ExecScanRecheckMtd) (ScanState *node, TupleTableSlot *slot);

extern PGDLLIMPORT TupleTableSlot *ExecScan(ScanState *node, ExecScanAccessMtd accessMtd,
								ExecScanRecheckMtd recheckMtd);
extern PGDLLIMPORT void ExecAssignScanProjectionInfo(ScanState *node);
extern PGDLLIMPORT void ExecAssignScanProjectionInfoWithVarno(ScanState *node, int varno);
extern PGDLLIMPORT void ExecScanReScan(ScanState *node);

/*
 * prototypes from functions in execTuples.c
 */
extern PGDLLIMPORT void ExecInitResultTypeTL(PlanState *planstate);
extern PGDLLIMPORT void ExecInitResultSlot(PlanState *planstate,
							   const TupleTableSlotOps *tts_ops);
extern PGDLLIMPORT void ExecInitResultTupleSlotTL(PlanState *planstate,
									  const TupleTableSlotOps *tts_ops);
extern PGDLLIMPORT void ExecInitScanTupleSlot(EState *estate, ScanState *scanstate,
								  TupleDesc tupleDesc,
								  const TupleTableSlotOps *tts_ops);
extern PGDLLIMPORT TupleTableSlot *ExecInitExtraTupleSlot(EState *estate,
											  TupleDesc tupledesc,
											  const TupleTableSlotOps *tts_ops);
extern PGDLLIMPORT TupleTableSlot *ExecInitNullTupleSlot(EState *estate, TupleDesc tupType,
											 const TupleTableSlotOps *tts_ops);
extern PGDLLIMPORT TupleDesc ExecTypeFromTL(List *targetList);
extern PGDLLIMPORT TupleDesc ExecCleanTypeFromTL(List *targetList);
extern PGDLLIMPORT TupleDesc ExecTypeFromExprList(List *exprList);
extern PGDLLIMPORT void ExecTypeSetColNames(TupleDesc typeInfo, List *namesList);
extern PGDLLIMPORT void UpdateChangedParamSet(PlanState *node, Bitmapset *newchg);

typedef struct TupOutputState
{
	TupleTableSlot *slot;
	DestReceiver *dest;
} TupOutputState;

extern PGDLLIMPORT TupOutputState *begin_tup_output_tupdesc(DestReceiver *dest,
												TupleDesc tupdesc,
												const TupleTableSlotOps *tts_ops);
extern PGDLLIMPORT void do_tup_output(TupOutputState *tstate, Datum *values, bool *isnull);
extern PGDLLIMPORT void do_text_output_multiline(TupOutputState *tstate, const char *txt);
extern PGDLLIMPORT void end_tup_output(TupOutputState *tstate);

/*
 * Write a single line of text given as a C string.
 *
 * Should only be used with a single-TEXT-attribute tupdesc.
 */
#define do_text_output_oneline(tstate, str_to_emit) \
	do { \
		Datum	values_[1]; \
		bool	isnull_[1]; \
		values_[0] = PointerGetDatum(cstring_to_text(str_to_emit)); \
		isnull_[0] = false; \
		do_tup_output(tstate, values_, isnull_); \
		pfree(DatumGetPointer(values_[0])); \
	} while (0)


/*
 * prototypes from functions in execUtils.c
 */
extern PGDLLIMPORT EState *CreateExecutorState(void);
extern PGDLLIMPORT void FreeExecutorState(EState *estate);
extern PGDLLIMPORT ExprContext *CreateExprContext(EState *estate);
extern PGDLLIMPORT ExprContext *CreateWorkExprContext(EState *estate);
extern PGDLLIMPORT ExprContext *CreateStandaloneExprContext(void);
extern PGDLLIMPORT void FreeExprContext(ExprContext *econtext, bool isCommit);
extern PGDLLIMPORT void ReScanExprContext(ExprContext *econtext);

#define ResetExprContext(econtext) \
	MemoryContextReset((econtext)->ecxt_per_tuple_memory)

extern PGDLLIMPORT ExprContext *MakePerTupleExprContext(EState *estate);

/* Get an EState's per-output-tuple exprcontext, making it if first use */
#define GetPerTupleExprContext(estate) \
	((estate)->es_per_tuple_exprcontext ? \
	 (estate)->es_per_tuple_exprcontext : \
	 MakePerTupleExprContext(estate))

#define GetPerTupleMemoryContext(estate) \
	(GetPerTupleExprContext(estate)->ecxt_per_tuple_memory)

/* Reset an EState's per-output-tuple exprcontext, if one's been created */
#define ResetPerTupleExprContext(estate) \
	do { \
		if ((estate)->es_per_tuple_exprcontext) \
			ResetExprContext((estate)->es_per_tuple_exprcontext); \
	} while (0)

extern PGDLLIMPORT void ExecAssignExprContext(EState *estate, PlanState *planstate);
extern PGDLLIMPORT TupleDesc ExecGetResultType(PlanState *planstate);
extern PGDLLIMPORT const TupleTableSlotOps *ExecGetResultSlotOps(PlanState *planstate,
													 bool *isfixed);
extern PGDLLIMPORT void ExecAssignProjectionInfo(PlanState *planstate,
									 TupleDesc inputDesc);
extern PGDLLIMPORT void ExecConditionalAssignProjectionInfo(PlanState *planstate,
												TupleDesc inputDesc, int varno);
extern PGDLLIMPORT void ExecFreeExprContext(PlanState *planstate);
extern PGDLLIMPORT void ExecAssignScanType(ScanState *scanstate, TupleDesc tupDesc);
extern PGDLLIMPORT void ExecCreateScanSlotFromOuterPlan(EState *estate,
											ScanState *scanstate,
											const TupleTableSlotOps *tts_ops);

extern PGDLLIMPORT bool ExecRelationIsTargetRelation(EState *estate, Index scanrelid);

extern PGDLLIMPORT Relation ExecOpenScanRelation(EState *estate, Index scanrelid, int eflags);

extern PGDLLIMPORT void ExecInitRangeTable(EState *estate, List *rangeTable);
extern PGDLLIMPORT void ExecCloseRangeTableRelations(EState *estate);
extern PGDLLIMPORT void ExecCloseResultRelations(EState *estate);

static inline RangeTblEntry *
exec_rt_fetch(Index rti, EState *estate)
{
	return (RangeTblEntry *) list_nth(estate->es_range_table, rti - 1);
}

extern PGDLLIMPORT Relation ExecGetRangeTableRelation(EState *estate, Index rti);
extern PGDLLIMPORT void ExecInitResultRelation(EState *estate, ResultRelInfo *resultRelInfo,
								   Index rti);

extern PGDLLIMPORT int	executor_errposition(EState *estate, int location);

extern PGDLLIMPORT void RegisterExprContextCallback(ExprContext *econtext,
										ExprContextCallbackFunction function,
										Datum arg);
extern PGDLLIMPORT void UnregisterExprContextCallback(ExprContext *econtext,
										  ExprContextCallbackFunction function,
										  Datum arg);

extern PGDLLIMPORT Datum GetAttributeByName(HeapTupleHeader tuple, const char *attname,
								bool *isNull);
extern PGDLLIMPORT Datum GetAttributeByNum(HeapTupleHeader tuple, AttrNumber attrno,
							   bool *isNull);

extern PGDLLIMPORT int	ExecTargetListLength(List *targetlist);
extern PGDLLIMPORT int	ExecCleanTargetListLength(List *targetlist);

extern PGDLLIMPORT TupleTableSlot *ExecGetTriggerOldSlot(EState *estate, ResultRelInfo *relInfo);
extern PGDLLIMPORT TupleTableSlot *ExecGetTriggerNewSlot(EState *estate, ResultRelInfo *relInfo);
extern PGDLLIMPORT TupleTableSlot *ExecGetReturningSlot(EState *estate, ResultRelInfo *relInfo);
extern PGDLLIMPORT TupleConversionMap *ExecGetChildToRootMap(ResultRelInfo *resultRelInfo);

extern PGDLLIMPORT Bitmapset *ExecGetInsertedCols(ResultRelInfo *relinfo, EState *estate);
extern PGDLLIMPORT Bitmapset *ExecGetUpdatedCols(ResultRelInfo *relinfo, EState *estate);
extern PGDLLIMPORT Bitmapset *ExecGetExtraUpdatedCols(ResultRelInfo *relinfo, EState *estate);
extern PGDLLIMPORT Bitmapset *ExecGetAllUpdatedCols(ResultRelInfo *relinfo, EState *estate);

/*
 * prototypes from functions in execIndexing.c
 */
extern PGDLLIMPORT void ExecOpenIndices(ResultRelInfo *resultRelInfo, bool speculative);
extern PGDLLIMPORT void ExecCloseIndices(ResultRelInfo *resultRelInfo);
extern PGDLLIMPORT List *ExecInsertIndexTuples(ResultRelInfo *resultRelInfo,
								   TupleTableSlot *slot, EState *estate,
								   bool update,
								   bool noDupErr,
								   bool *specConflict, List *arbiterIndexes);
extern PGDLLIMPORT bool ExecCheckIndexConstraints(ResultRelInfo *resultRelInfo,
									  TupleTableSlot *slot,
									  EState *estate, ItemPointer conflictTid,
									  List *arbiterIndexes);
extern PGDLLIMPORT void check_exclusion_constraint(Relation heap, Relation index,
									   IndexInfo *indexInfo,
									   ItemPointer tupleid,
									   Datum *values, bool *isnull,
									   EState *estate, bool newIndex);

/*
 * prototypes from functions in execReplication.c
 */
extern PGDLLIMPORT bool RelationFindReplTupleByIndex(Relation rel, Oid idxoid,
										 LockTupleMode lockmode,
										 TupleTableSlot *searchslot,
										 TupleTableSlot *outslot);
extern PGDLLIMPORT bool RelationFindReplTupleSeq(Relation rel, LockTupleMode lockmode,
									 TupleTableSlot *searchslot, TupleTableSlot *outslot);

extern PGDLLIMPORT void ExecSimpleRelationInsert(ResultRelInfo *resultRelInfo,
									 EState *estate, TupleTableSlot *slot);
extern PGDLLIMPORT void ExecSimpleRelationUpdate(ResultRelInfo *resultRelInfo,
									 EState *estate, EPQState *epqstate,
									 TupleTableSlot *searchslot, TupleTableSlot *slot);
extern PGDLLIMPORT void ExecSimpleRelationDelete(ResultRelInfo *resultRelInfo,
									 EState *estate, EPQState *epqstate,
									 TupleTableSlot *searchslot);
extern PGDLLIMPORT void CheckCmdReplicaIdentity(Relation rel, CmdType cmd);

extern PGDLLIMPORT void CheckSubscriptionRelkind(char relkind, const char *nspname,
									 const char *relname);

/*
 * prototypes from functions in nodeModifyTable.c
 */
extern PGDLLIMPORT TupleTableSlot *ExecGetUpdateNewTuple(ResultRelInfo *relinfo,
											 TupleTableSlot *planSlot,
											 TupleTableSlot *oldSlot);
extern PGDLLIMPORT ResultRelInfo *ExecLookupResultRelByOid(ModifyTableState *node,
											   Oid resultoid,
											   bool missing_ok,
											   bool update_cache);

#endif							/* EXECUTOR_H  */
