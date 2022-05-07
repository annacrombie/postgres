/*-------------------------------------------------------------------------
 *
 * parse_coerce.h
 *	Routines for type coercion.
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/parser/parse_coerce.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PARSE_COERCE_H
#define PARSE_COERCE_H

#include "parser/parse_node.h"


/* Type categories (see TYPCATEGORY_xxx symbols in catalog/pg_type.h) */
typedef char TYPCATEGORY;

/* Result codes for find_coercion_pathway */
typedef enum CoercionPathType
{
	COERCION_PATH_NONE,			/* failed to find any coercion pathway */
	COERCION_PATH_FUNC,			/* apply the specified coercion function */
	COERCION_PATH_RELABELTYPE,	/* binary-compatible cast, no function */
	COERCION_PATH_ARRAYCOERCE,	/* need an ArrayCoerceExpr node */
	COERCION_PATH_COERCEVIAIO	/* need a CoerceViaIO node */
} CoercionPathType;


extern PGDLLIMPORT bool IsBinaryCoercible(Oid srctype, Oid targettype);
extern PGDLLIMPORT bool IsPreferredType(TYPCATEGORY category, Oid type);
extern PGDLLIMPORT TYPCATEGORY TypeCategory(Oid type);

extern PGDLLIMPORT Node *coerce_to_target_type(ParseState *pstate,
								   Node *expr, Oid exprtype,
								   Oid targettype, int32 targettypmod,
								   CoercionContext ccontext,
								   CoercionForm cformat,
								   int location);
extern PGDLLIMPORT bool can_coerce_type(int nargs, const Oid *input_typeids, const Oid *target_typeids,
							CoercionContext ccontext);
extern PGDLLIMPORT Node *coerce_type(ParseState *pstate, Node *node,
						 Oid inputTypeId, Oid targetTypeId, int32 targetTypeMod,
						 CoercionContext ccontext, CoercionForm cformat, int location);
extern PGDLLIMPORT Node *coerce_to_domain(Node *arg, Oid baseTypeId, int32 baseTypeMod,
							  Oid typeId,
							  CoercionContext ccontext, CoercionForm cformat, int location,
							  bool hideInputCoercion);

extern PGDLLIMPORT Node *coerce_to_boolean(ParseState *pstate, Node *node,
							   const char *constructName);
extern PGDLLIMPORT Node *coerce_to_specific_type(ParseState *pstate, Node *node,
									 Oid targetTypeId,
									 const char *constructName);

extern PGDLLIMPORT Node *coerce_to_specific_type_typmod(ParseState *pstate, Node *node,
											Oid targetTypeId, int32 targetTypmod,
											const char *constructName);

extern PGDLLIMPORT int	parser_coercion_errposition(ParseState *pstate,
										int coerce_location,
										Node *input_expr);

extern PGDLLIMPORT Oid	select_common_type(ParseState *pstate, List *exprs,
							   const char *context, Node **which_expr);
extern PGDLLIMPORT Node *coerce_to_common_type(ParseState *pstate, Node *node,
								   Oid targetTypeId,
								   const char *context);
extern PGDLLIMPORT bool verify_common_type(Oid common_type, List *exprs);

extern PGDLLIMPORT int32 select_common_typmod(ParseState *pstate, List *exprs, Oid common_type);

extern PGDLLIMPORT bool check_generic_type_consistency(const Oid *actual_arg_types,
										   const Oid *declared_arg_types,
										   int nargs);
extern PGDLLIMPORT Oid	enforce_generic_type_consistency(const Oid *actual_arg_types,
											 Oid *declared_arg_types,
											 int nargs,
											 Oid rettype,
											 bool allow_poly);

extern PGDLLIMPORT char *check_valid_polymorphic_signature(Oid ret_type,
											   const Oid *declared_arg_types,
											   int nargs);
extern PGDLLIMPORT char *check_valid_internal_signature(Oid ret_type,
											const Oid *declared_arg_types,
											int nargs);

extern PGDLLIMPORT CoercionPathType find_coercion_pathway(Oid targetTypeId,
											  Oid sourceTypeId,
											  CoercionContext ccontext,
											  Oid *funcid);
extern PGDLLIMPORT CoercionPathType find_typmod_coercion_function(Oid typeId,
													  Oid *funcid);

#endif							/* PARSE_COERCE_H */
