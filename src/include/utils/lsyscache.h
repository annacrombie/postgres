/*-------------------------------------------------------------------------
 *
 * lsyscache.h
 *	  Convenience routines for common queries in the system catalog cache.
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/utils/lsyscache.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef LSYSCACHE_H
#define LSYSCACHE_H

#include "access/attnum.h"
#include "access/htup.h"
#include "nodes/pg_list.h"

/* avoid including subscripting.h here */
struct SubscriptRoutines;

/* Result list element for get_op_btree_interpretation */
typedef struct OpBtreeInterpretation
{
	Oid			opfamily_id;	/* btree opfamily containing operator */
	int			strategy;		/* its strategy number */
	Oid			oplefttype;		/* declared left input datatype */
	Oid			oprighttype;	/* declared right input datatype */
} OpBtreeInterpretation;

/* I/O function selector for get_type_io_data */
typedef enum IOFuncSelector
{
	IOFunc_input,
	IOFunc_output,
	IOFunc_receive,
	IOFunc_send
} IOFuncSelector;

/* Flag bits for get_attstatsslot */
#define ATTSTATSSLOT_VALUES		0x01
#define ATTSTATSSLOT_NUMBERS	0x02

/* Result struct for get_attstatsslot */
typedef struct AttStatsSlot
{
	/* Always filled: */
	Oid			staop;			/* Actual staop for the found slot */
	Oid			stacoll;		/* Actual collation for the found slot */
	/* Filled if ATTSTATSSLOT_VALUES is specified: */
	Oid			valuetype;		/* Actual datatype of the values */
	Datum	   *values;			/* slot's "values" array, or NULL if none */
	int			nvalues;		/* length of values[], or 0 */
	/* Filled if ATTSTATSSLOT_NUMBERS is specified: */
	float4	   *numbers;		/* slot's "numbers" array, or NULL if none */
	int			nnumbers;		/* length of numbers[], or 0 */

	/* Remaining fields are private to get_attstatsslot/free_attstatsslot */
	void	   *values_arr;		/* palloc'd values array, if any */
	void	   *numbers_arr;	/* palloc'd numbers array, if any */
} AttStatsSlot;

/* Hook for plugins to get control in get_attavgwidth() */
typedef int32 (*get_attavgwidth_hook_type) (Oid relid, AttrNumber attnum);
extern PGDLLIMPORT get_attavgwidth_hook_type get_attavgwidth_hook;

extern PGDLLIMPORT bool op_in_opfamily(Oid opno, Oid opfamily);
extern PGDLLIMPORT int	get_op_opfamily_strategy(Oid opno, Oid opfamily);
extern PGDLLIMPORT Oid	get_op_opfamily_sortfamily(Oid opno, Oid opfamily);
extern PGDLLIMPORT void get_op_opfamily_properties(Oid opno, Oid opfamily, bool ordering_op,
									   int *strategy,
									   Oid *lefttype,
									   Oid *righttype);
extern PGDLLIMPORT Oid	get_opfamily_member(Oid opfamily, Oid lefttype, Oid righttype,
								int16 strategy);
extern PGDLLIMPORT bool get_ordering_op_properties(Oid opno,
									   Oid *opfamily, Oid *opcintype, int16 *strategy);
extern PGDLLIMPORT Oid	get_equality_op_for_ordering_op(Oid opno, bool *reverse);
extern PGDLLIMPORT Oid	get_ordering_op_for_equality_op(Oid opno, bool use_lhs_type);
extern PGDLLIMPORT List *get_mergejoin_opfamilies(Oid opno);
extern PGDLLIMPORT bool get_compatible_hash_operators(Oid opno,
										  Oid *lhs_opno, Oid *rhs_opno);
extern PGDLLIMPORT bool get_op_hash_functions(Oid opno,
								  RegProcedure *lhs_procno, RegProcedure *rhs_procno);
extern PGDLLIMPORT List *get_op_btree_interpretation(Oid opno);
extern PGDLLIMPORT bool equality_ops_are_compatible(Oid opno1, Oid opno2);
extern PGDLLIMPORT bool comparison_ops_are_compatible(Oid opno1, Oid opno2);
extern PGDLLIMPORT Oid	get_opfamily_proc(Oid opfamily, Oid lefttype, Oid righttype,
							  int16 procnum);
extern PGDLLIMPORT char *get_attname(Oid relid, AttrNumber attnum, bool missing_ok);
extern PGDLLIMPORT AttrNumber get_attnum(Oid relid, const char *attname);
extern PGDLLIMPORT int	get_attstattarget(Oid relid, AttrNumber attnum);
extern PGDLLIMPORT char get_attgenerated(Oid relid, AttrNumber attnum);
extern PGDLLIMPORT Oid	get_atttype(Oid relid, AttrNumber attnum);
extern PGDLLIMPORT void get_atttypetypmodcoll(Oid relid, AttrNumber attnum,
								  Oid *typid, int32 *typmod, Oid *collid);
extern PGDLLIMPORT Datum get_attoptions(Oid relid, int16 attnum);
extern PGDLLIMPORT Oid	get_cast_oid(Oid sourcetypeid, Oid targettypeid, bool missing_ok);
extern PGDLLIMPORT char *get_collation_name(Oid colloid);
extern PGDLLIMPORT bool get_collation_isdeterministic(Oid colloid);
extern PGDLLIMPORT char *get_constraint_name(Oid conoid);
extern PGDLLIMPORT Oid	get_constraint_index(Oid conoid);
extern PGDLLIMPORT char *get_language_name(Oid langoid, bool missing_ok);
extern PGDLLIMPORT Oid	get_opclass_family(Oid opclass);
extern PGDLLIMPORT Oid	get_opclass_input_type(Oid opclass);
extern PGDLLIMPORT bool get_opclass_opfamily_and_input_type(Oid opclass,
												Oid *opfamily, Oid *opcintype);
extern PGDLLIMPORT RegProcedure get_opcode(Oid opno);
extern PGDLLIMPORT char *get_opname(Oid opno);
extern PGDLLIMPORT Oid	get_op_rettype(Oid opno);
extern PGDLLIMPORT void op_input_types(Oid opno, Oid *lefttype, Oid *righttype);
extern PGDLLIMPORT bool op_mergejoinable(Oid opno, Oid inputtype);
extern PGDLLIMPORT bool op_hashjoinable(Oid opno, Oid inputtype);
extern PGDLLIMPORT bool op_strict(Oid opno);
extern PGDLLIMPORT char op_volatile(Oid opno);
extern PGDLLIMPORT Oid	get_commutator(Oid opno);
extern PGDLLIMPORT Oid	get_negator(Oid opno);
extern PGDLLIMPORT RegProcedure get_oprrest(Oid opno);
extern PGDLLIMPORT RegProcedure get_oprjoin(Oid opno);
extern PGDLLIMPORT char *get_func_name(Oid funcid);
extern PGDLLIMPORT Oid	get_func_namespace(Oid funcid);
extern PGDLLIMPORT Oid	get_func_rettype(Oid funcid);
extern PGDLLIMPORT int	get_func_nargs(Oid funcid);
extern PGDLLIMPORT Oid	get_func_signature(Oid funcid, Oid **argtypes, int *nargs);
extern PGDLLIMPORT Oid	get_func_variadictype(Oid funcid);
extern PGDLLIMPORT bool get_func_retset(Oid funcid);
extern PGDLLIMPORT bool func_strict(Oid funcid);
extern PGDLLIMPORT char func_volatile(Oid funcid);
extern PGDLLIMPORT char func_parallel(Oid funcid);
extern PGDLLIMPORT char get_func_prokind(Oid funcid);
extern PGDLLIMPORT bool get_func_leakproof(Oid funcid);
extern PGDLLIMPORT RegProcedure get_func_support(Oid funcid);
extern PGDLLIMPORT Oid	get_relname_relid(const char *relname, Oid relnamespace);
extern PGDLLIMPORT char *get_rel_name(Oid relid);
extern PGDLLIMPORT Oid	get_rel_namespace(Oid relid);
extern PGDLLIMPORT Oid	get_rel_type_id(Oid relid);
extern PGDLLIMPORT char get_rel_relkind(Oid relid);
extern PGDLLIMPORT bool get_rel_relispartition(Oid relid);
extern PGDLLIMPORT Oid	get_rel_tablespace(Oid relid);
extern PGDLLIMPORT char get_rel_persistence(Oid relid);
extern PGDLLIMPORT Oid	get_transform_fromsql(Oid typid, Oid langid, List *trftypes);
extern PGDLLIMPORT Oid	get_transform_tosql(Oid typid, Oid langid, List *trftypes);
extern PGDLLIMPORT bool get_typisdefined(Oid typid);
extern PGDLLIMPORT int16 get_typlen(Oid typid);
extern PGDLLIMPORT bool get_typbyval(Oid typid);
extern PGDLLIMPORT void get_typlenbyval(Oid typid, int16 *typlen, bool *typbyval);
extern PGDLLIMPORT void get_typlenbyvalalign(Oid typid, int16 *typlen, bool *typbyval,
								 char *typalign);
extern PGDLLIMPORT Oid	getTypeIOParam(HeapTuple typeTuple);
extern PGDLLIMPORT void get_type_io_data(Oid typid,
							 IOFuncSelector which_func,
							 int16 *typlen,
							 bool *typbyval,
							 char *typalign,
							 char *typdelim,
							 Oid *typioparam,
							 Oid *func);
extern PGDLLIMPORT char get_typstorage(Oid typid);
extern PGDLLIMPORT Node *get_typdefault(Oid typid);
extern PGDLLIMPORT char get_typtype(Oid typid);
extern PGDLLIMPORT bool type_is_rowtype(Oid typid);
extern PGDLLIMPORT bool type_is_enum(Oid typid);
extern PGDLLIMPORT bool type_is_range(Oid typid);
extern PGDLLIMPORT bool type_is_multirange(Oid typid);
extern PGDLLIMPORT void get_type_category_preferred(Oid typid,
										char *typcategory,
										bool *typispreferred);
extern PGDLLIMPORT Oid	get_typ_typrelid(Oid typid);
extern PGDLLIMPORT Oid	get_element_type(Oid typid);
extern PGDLLIMPORT Oid	get_array_type(Oid typid);
extern PGDLLIMPORT Oid	get_promoted_array_type(Oid typid);
extern PGDLLIMPORT Oid	get_base_element_type(Oid typid);
extern PGDLLIMPORT void getTypeInputInfo(Oid type, Oid *typInput, Oid *typIOParam);
extern PGDLLIMPORT void getTypeOutputInfo(Oid type, Oid *typOutput, bool *typIsVarlena);
extern PGDLLIMPORT void getTypeBinaryInputInfo(Oid type, Oid *typReceive, Oid *typIOParam);
extern PGDLLIMPORT void getTypeBinaryOutputInfo(Oid type, Oid *typSend, bool *typIsVarlena);
extern PGDLLIMPORT Oid	get_typmodin(Oid typid);
extern PGDLLIMPORT Oid	get_typcollation(Oid typid);
extern PGDLLIMPORT bool type_is_collatable(Oid typid);
extern PGDLLIMPORT RegProcedure get_typsubscript(Oid typid, Oid *typelemp);
extern PGDLLIMPORT const struct SubscriptRoutines *getSubscriptingRoutines(Oid typid,
															   Oid *typelemp);
extern PGDLLIMPORT Oid	getBaseType(Oid typid);
extern PGDLLIMPORT Oid	getBaseTypeAndTypmod(Oid typid, int32 *typmod);
extern PGDLLIMPORT int32 get_typavgwidth(Oid typid, int32 typmod);
extern PGDLLIMPORT int32 get_attavgwidth(Oid relid, AttrNumber attnum);
extern PGDLLIMPORT bool get_attstatsslot(AttStatsSlot *sslot, HeapTuple statstuple,
							 int reqkind, Oid reqop, int flags);
extern PGDLLIMPORT void free_attstatsslot(AttStatsSlot *sslot);
extern PGDLLIMPORT char *get_namespace_name(Oid nspid);
extern PGDLLIMPORT char *get_namespace_name_or_temp(Oid nspid);
extern PGDLLIMPORT Oid	get_range_subtype(Oid rangeOid);
extern PGDLLIMPORT Oid	get_range_collation(Oid rangeOid);
extern PGDLLIMPORT Oid	get_range_multirange(Oid rangeOid);
extern PGDLLIMPORT Oid	get_multirange_range(Oid multirangeOid);
extern PGDLLIMPORT Oid	get_index_column_opclass(Oid index_oid, int attno);
extern PGDLLIMPORT bool get_index_isreplident(Oid index_oid);
extern PGDLLIMPORT bool get_index_isvalid(Oid index_oid);
extern PGDLLIMPORT bool get_index_isclustered(Oid index_oid);

#define type_is_array(typid)  (get_element_type(typid) != InvalidOid)
/* type_is_array_domain accepts both plain arrays and domains over arrays */
#define type_is_array_domain(typid)  (get_base_element_type(typid) != InvalidOid)

#define TypeIsToastable(typid)	(get_typstorage(typid) != TYPSTORAGE_PLAIN)

#endif							/* LSYSCACHE_H */
