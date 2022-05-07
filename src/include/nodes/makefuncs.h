/*-------------------------------------------------------------------------
 *
 * makefuncs.h
 *	  prototypes for the creator functions of various nodes
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/nodes/makefuncs.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef MAKEFUNC_H
#define MAKEFUNC_H

#include "nodes/execnodes.h"
#include "nodes/parsenodes.h"


extern PGDLLIMPORT A_Expr *makeA_Expr(A_Expr_Kind kind, List *name,
						  Node *lexpr, Node *rexpr, int location);

extern PGDLLIMPORT A_Expr *makeSimpleA_Expr(A_Expr_Kind kind, char *name,
								Node *lexpr, Node *rexpr, int location);

extern PGDLLIMPORT Var *makeVar(int varno,
					AttrNumber varattno,
					Oid vartype,
					int32 vartypmod,
					Oid varcollid,
					Index varlevelsup);

extern PGDLLIMPORT Var *makeVarFromTargetEntry(int varno,
								   TargetEntry *tle);

extern PGDLLIMPORT Var *makeWholeRowVar(RangeTblEntry *rte,
							int varno,
							Index varlevelsup,
							bool allowScalar);

extern PGDLLIMPORT TargetEntry *makeTargetEntry(Expr *expr,
									AttrNumber resno,
									char *resname,
									bool resjunk);

extern PGDLLIMPORT TargetEntry *flatCopyTargetEntry(TargetEntry *src_tle);

extern PGDLLIMPORT FromExpr *makeFromExpr(List *fromlist, Node *quals);

extern PGDLLIMPORT Const *makeConst(Oid consttype,
						int32 consttypmod,
						Oid constcollid,
						int constlen,
						Datum constvalue,
						bool constisnull,
						bool constbyval);

extern PGDLLIMPORT Const *makeNullConst(Oid consttype, int32 consttypmod, Oid constcollid);

extern PGDLLIMPORT Node *makeBoolConst(bool value, bool isnull);

extern PGDLLIMPORT Expr *makeBoolExpr(BoolExprType boolop, List *args, int location);

extern PGDLLIMPORT Alias *makeAlias(const char *aliasname, List *colnames);

extern PGDLLIMPORT RelabelType *makeRelabelType(Expr *arg, Oid rtype, int32 rtypmod,
									Oid rcollid, CoercionForm rformat);

extern PGDLLIMPORT RangeVar *makeRangeVar(char *schemaname, char *relname, int location);

extern PGDLLIMPORT TypeName *makeTypeName(char *typnam);
extern PGDLLIMPORT TypeName *makeTypeNameFromNameList(List *names);
extern PGDLLIMPORT TypeName *makeTypeNameFromOid(Oid typeOid, int32 typmod);

extern PGDLLIMPORT ColumnDef *makeColumnDef(const char *colname,
								Oid typeOid, int32 typmod, Oid collOid);

extern PGDLLIMPORT FuncExpr *makeFuncExpr(Oid funcid, Oid rettype, List *args,
							  Oid funccollid, Oid inputcollid, CoercionForm fformat);

extern PGDLLIMPORT FuncCall *makeFuncCall(List *name, List *args,
							  CoercionForm funcformat, int location);

extern PGDLLIMPORT Expr *make_opclause(Oid opno, Oid opresulttype, bool opretset,
						   Expr *leftop, Expr *rightop,
						   Oid opcollid, Oid inputcollid);

extern PGDLLIMPORT Expr *make_andclause(List *andclauses);
extern PGDLLIMPORT Expr *make_orclause(List *orclauses);
extern PGDLLIMPORT Expr *make_notclause(Expr *notclause);

extern PGDLLIMPORT Node *make_and_qual(Node *qual1, Node *qual2);
extern PGDLLIMPORT Expr *make_ands_explicit(List *andclauses);
extern PGDLLIMPORT List *make_ands_implicit(Expr *clause);

extern PGDLLIMPORT IndexInfo *makeIndexInfo(int numattrs, int numkeyattrs, Oid amoid,
								List *expressions, List *predicates,
								bool unique, bool nulls_not_distinct, bool isready, bool concurrent);

extern PGDLLIMPORT DefElem *makeDefElem(char *name, Node *arg, int location);
extern PGDLLIMPORT DefElem *makeDefElemExtended(char *nameSpace, char *name, Node *arg,
									DefElemAction defaction, int location);

extern PGDLLIMPORT GroupingSet *makeGroupingSet(GroupingSetKind kind, List *content, int location);

extern PGDLLIMPORT VacuumRelation *makeVacuumRelation(RangeVar *relation, Oid oid, List *va_cols);

extern PGDLLIMPORT JsonFormat *makeJsonFormat(JsonFormatType type, JsonEncoding encoding,
								  int location);
extern PGDLLIMPORT JsonValueExpr *makeJsonValueExpr(Expr *expr, JsonFormat *format);
extern PGDLLIMPORT JsonBehavior *makeJsonBehavior(JsonBehaviorType type, Node *expr);
extern PGDLLIMPORT Node *makeJsonTableJoinedPlan(JsonTablePlanJoinType type,
									 Node *plan1, Node *plan2, int location);
extern PGDLLIMPORT Node *makeJsonKeyValue(Node *key, Node *value);
extern PGDLLIMPORT Node *makeJsonIsPredicate(Node *expr, JsonFormat *format,
								 JsonValueType vtype, bool unique_keys,
								 int location);
extern PGDLLIMPORT JsonEncoding makeJsonEncoding(char *name);

#endif							/* MAKEFUNC_H */
