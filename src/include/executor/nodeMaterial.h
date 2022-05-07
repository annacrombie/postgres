/*-------------------------------------------------------------------------
 *
 * nodeMaterial.h
 *
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/executor/nodeMaterial.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef NODEMATERIAL_H
#define NODEMATERIAL_H

#include "nodes/execnodes.h"

extern PGDLLIMPORT MaterialState *ExecInitMaterial(Material *node, EState *estate, int eflags);
extern PGDLLIMPORT void ExecEndMaterial(MaterialState *node);
extern PGDLLIMPORT void ExecMaterialMarkPos(MaterialState *node);
extern PGDLLIMPORT void ExecMaterialRestrPos(MaterialState *node);
extern PGDLLIMPORT void ExecReScanMaterial(MaterialState *node);

#endif							/* NODEMATERIAL_H */
