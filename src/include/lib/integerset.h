/*
 * integerset.h
 *	  In-memory data structure to hold a large set of integers efficiently
 *
 * Portions Copyright (c) 2012-2022, PostgreSQL Global Development Group
 *
 * src/include/lib/integerset.h
 */
#ifndef INTEGERSET_H
#define INTEGERSET_H

typedef struct IntegerSet IntegerSet;

extern PGDLLIMPORT IntegerSet *intset_create(void);
extern PGDLLIMPORT void intset_add_member(IntegerSet *intset, uint64 x);
extern PGDLLIMPORT bool intset_is_member(IntegerSet *intset, uint64 x);

extern PGDLLIMPORT uint64 intset_num_entries(IntegerSet *intset);
extern PGDLLIMPORT uint64 intset_memory_usage(IntegerSet *intset);

extern PGDLLIMPORT void intset_begin_iterate(IntegerSet *intset);
extern PGDLLIMPORT bool intset_iterate_next(IntegerSet *intset, uint64 *next);

#endif							/* INTEGERSET_H */
