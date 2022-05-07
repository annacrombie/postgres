/*
 * seclabel.h
 *
 * Prototypes for functions in commands/seclabel.c
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 */
#ifndef SECLABEL_H
#define SECLABEL_H

#include "catalog/objectaddress.h"

/*
 * Internal APIs
 */
extern PGDLLIMPORT char *GetSecurityLabel(const ObjectAddress *object,
							  const char *provider);
extern PGDLLIMPORT void SetSecurityLabel(const ObjectAddress *object,
							 const char *provider, const char *label);
extern PGDLLIMPORT void DeleteSecurityLabel(const ObjectAddress *object);
extern PGDLLIMPORT void DeleteSharedSecurityLabel(Oid objectId, Oid classId);

/*
 * Statement and ESP hook support
 */
extern PGDLLIMPORT ObjectAddress ExecSecLabelStmt(SecLabelStmt *stmt);

typedef void (*check_object_relabel_type) (const ObjectAddress *object,
										   const char *seclabel);
extern PGDLLIMPORT void register_label_provider(const char *provider,
									check_object_relabel_type hook);

#endif							/* SECLABEL_H */
