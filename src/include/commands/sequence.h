/*-------------------------------------------------------------------------
 *
 * sequence.h
 *	  prototypes for sequence.c.
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/commands/sequence.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef SEQUENCE_H
#define SEQUENCE_H

#include "access/xlogreader.h"
#include "catalog/objectaddress.h"
#include "fmgr.h"
#include "lib/stringinfo.h"
#include "nodes/parsenodes.h"
#include "parser/parse_node.h"
#include "storage/relfilenode.h"


typedef struct FormData_pg_sequence_data
{
	int64		last_value;
	int64		log_cnt;
	bool		is_called;
} FormData_pg_sequence_data;

typedef FormData_pg_sequence_data *Form_pg_sequence_data;

/*
 * Columns of a sequence relation
 */

#define SEQ_COL_LASTVAL			1
#define SEQ_COL_LOG				2
#define SEQ_COL_CALLED			3

#define SEQ_COL_FIRSTCOL		SEQ_COL_LASTVAL
#define SEQ_COL_LASTCOL			SEQ_COL_CALLED

/* XLOG stuff */
#define XLOG_SEQ_LOG			0x00

typedef struct xl_seq_rec
{
	RelFileNode node;
	/* SEQUENCE TUPLE DATA FOLLOWS AT THE END */
} xl_seq_rec;

extern PGDLLIMPORT int64 nextval_internal(Oid relid, bool check_permissions);
extern PGDLLIMPORT Datum nextval(PG_FUNCTION_ARGS);
extern PGDLLIMPORT List *sequence_options(Oid relid);

extern PGDLLIMPORT ObjectAddress DefineSequence(ParseState *pstate, CreateSeqStmt *stmt);
extern PGDLLIMPORT ObjectAddress AlterSequence(ParseState *pstate, AlterSeqStmt *stmt);
extern PGDLLIMPORT void SequenceChangePersistence(Oid relid, char newrelpersistence);
extern PGDLLIMPORT void DeleteSequenceTuple(Oid relid);
extern PGDLLIMPORT void ResetSequence(Oid seq_relid);
extern PGDLLIMPORT void ResetSequenceCaches(void);

extern PGDLLIMPORT void seq_redo(XLogReaderState *rptr);
extern PGDLLIMPORT void seq_desc(StringInfo buf, XLogReaderState *rptr);
extern PGDLLIMPORT const char *seq_identify(uint8 info);
extern PGDLLIMPORT void seq_mask(char *pagedata, BlockNumber blkno);

#endif							/* SEQUENCE_H */
