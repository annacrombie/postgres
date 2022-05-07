/*-------------------------------------------------------------------------
 *
 * rewriteSupport.h
 *
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/rewrite/rewriteSupport.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef REWRITESUPPORT_H
#define REWRITESUPPORT_H

/* The ON SELECT rule of a view is always named this: */
#define ViewSelectRuleName	"_RETURN"

extern PGDLLIMPORT bool IsDefinedRewriteRule(Oid owningRel, const char *ruleName);

extern PGDLLIMPORT void SetRelationRuleStatus(Oid relationId, bool relHasRules);

extern PGDLLIMPORT Oid	get_rewrite_oid(Oid relid, const char *rulename, bool missing_ok);

#endif							/* REWRITESUPPORT_H */
