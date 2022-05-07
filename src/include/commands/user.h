/*-------------------------------------------------------------------------
 *
 * user.h
 *	  Commands for manipulating roles (formerly called users).
 *
 *
 * src/include/commands/user.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef USER_H
#define USER_H

#include "catalog/objectaddress.h"
#include "libpq/crypt.h"
#include "nodes/parsenodes.h"
#include "parser/parse_node.h"

/* GUC. Is actually of type PasswordType. */
extern PGDLLIMPORT int Password_encryption;

/* Hook to check passwords in CreateRole() and AlterRole() */
typedef void (*check_password_hook_type) (const char *username, const char *shadow_pass, PasswordType password_type, Datum validuntil_time, bool validuntil_null);

extern PGDLLIMPORT check_password_hook_type check_password_hook;

extern PGDLLIMPORT Oid	CreateRole(ParseState *pstate, CreateRoleStmt *stmt);
extern PGDLLIMPORT Oid	AlterRole(ParseState *pstate, AlterRoleStmt *stmt);
extern PGDLLIMPORT Oid	AlterRoleSet(AlterRoleSetStmt *stmt);
extern PGDLLIMPORT void DropRole(DropRoleStmt *stmt);
extern PGDLLIMPORT void GrantRole(GrantRoleStmt *stmt);
extern PGDLLIMPORT ObjectAddress RenameRole(const char *oldname, const char *newname);
extern PGDLLIMPORT void DropOwnedObjects(DropOwnedStmt *stmt);
extern PGDLLIMPORT void ReassignOwnedObjects(ReassignOwnedStmt *stmt);
extern PGDLLIMPORT List *roleSpecsToIds(List *memberNames);

#endif							/* USER_H */
