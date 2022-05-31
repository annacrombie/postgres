#ifndef MEMUTILS_INTERNAL_H
#define MEMUTILS_INTERNAL_H

#include "utils/memutils.h"

extern void *AllocSetAlloc(MemoryContext context, Size size);
extern void AllocSetFree(MemoryContext context, void *pointer);
extern void *AllocSetRealloc(MemoryContext context, void *pointer, Size size);
extern void AllocSetReset(MemoryContext context);
extern void AllocSetDelete(MemoryContext context);
extern Size AllocSetGetChunkSpace(MemoryContext context, void *pointer);
extern bool AllocSetIsEmpty(MemoryContext context);
extern void AllocSetStats(MemoryContext context,
						  MemoryStatsPrintFunc printfunc, void *passthru,
						  MemoryContextCounters *totals,
						  bool print_to_stderr);

#ifdef MEMORY_CONTEXT_CHECKING
extern void AllocSetCheck(MemoryContext context);
#endif


extern void *GenerationAlloc(MemoryContext context, Size size);
extern void GenerationFree(MemoryContext context, void *pointer);
extern void *GenerationRealloc(MemoryContext context, void *pointer, Size size);
extern void GenerationReset(MemoryContext context);
extern void GenerationDelete(MemoryContext context);
extern Size GenerationGetChunkSpace(MemoryContext context, void *pointer);
extern bool GenerationIsEmpty(MemoryContext context);
extern void GenerationStats(MemoryContext context,
							MemoryStatsPrintFunc printfunc, void *passthru,
							MemoryContextCounters *totals,
							bool print_to_stderr);

#ifdef MEMORY_CONTEXT_CHECKING
extern void GenerationCheck(MemoryContext context);
#endif


/*
 * These functions implement the MemoryContext API for Slab contexts.
 */
extern void *SlabAlloc(MemoryContext context, Size size);
extern void SlabFree(MemoryContext context, void *pointer);
extern void *SlabRealloc(MemoryContext context, void *pointer, Size size);
extern void SlabReset(MemoryContext context);
extern void SlabDelete(MemoryContext context);
extern Size SlabGetChunkSpace(MemoryContext context, void *pointer);
extern bool SlabIsEmpty(MemoryContext context);
extern void SlabStats(MemoryContext context,
					  MemoryStatsPrintFunc printfunc, void *passthru,
					  MemoryContextCounters *totals,
					  bool print_to_stderr);
#ifdef MEMORY_CONTEXT_CHECKING
extern void SlabCheck(MemoryContext context);
#endif


typedef enum MemoryContextMethodID
{
	MCTX_ASET_ID = 0,
	MCTX_GENERATION_ID,
	MCTX_SLAB_ID,
} MemoryContextMethodID;


/*
 * This routine handles the context-type-independent part of memory
 * context creation.  It's intended to be called from context-type-
 * specific creation routines, and noplace else.
 */
extern void MemoryContextCreate(MemoryContext node,
								NodeTag tag,
								MemoryContextMethodID method_id,
								MemoryContext parent,
								const char *name);

#endif
