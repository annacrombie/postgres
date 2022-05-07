/*
 * xloginsert.h
 *
 * Functions for generating WAL records
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/access/xloginsert.h
 */
#ifndef XLOGINSERT_H
#define XLOGINSERT_H

#include "access/rmgr.h"
#include "access/xlogdefs.h"
#include "storage/block.h"
#include "storage/buf.h"
#include "storage/relfilenode.h"
#include "utils/relcache.h"

/*
 * The minimum size of the WAL construction working area. If you need to
 * register more than XLR_NORMAL_MAX_BLOCK_ID block references or have more
 * than XLR_NORMAL_RDATAS data chunks in a single WAL record, you must call
 * XLogEnsureRecordSpace() first to allocate more working memory.
 */
#define XLR_NORMAL_MAX_BLOCK_ID		4
#define XLR_NORMAL_RDATAS			20

/* flags for XLogRegisterBuffer */
#define REGBUF_FORCE_IMAGE	0x01	/* force a full-page image */
#define REGBUF_NO_IMAGE		0x02	/* don't take a full-page image */
#define REGBUF_WILL_INIT	(0x04 | 0x02)	/* page will be re-initialized at
											 * replay (implies NO_IMAGE) */
#define REGBUF_STANDARD		0x08	/* page follows "standard" page layout,
									 * (data between pd_lower and pd_upper
									 * will be skipped) */
#define REGBUF_KEEP_DATA	0x10	/* include data even if a full-page image
									 * is taken */

/* prototypes for public functions in xloginsert.c: */
extern PGDLLIMPORT void XLogBeginInsert(void);
extern PGDLLIMPORT void XLogSetRecordFlags(uint8 flags);
extern PGDLLIMPORT XLogRecPtr XLogInsert(RmgrId rmid, uint8 info);
extern PGDLLIMPORT void XLogEnsureRecordSpace(int max_block_id, int ndatas);
extern PGDLLIMPORT void XLogRegisterData(char *data, int len);
extern PGDLLIMPORT void XLogRegisterBuffer(uint8 block_id, Buffer buffer, uint8 flags);
extern PGDLLIMPORT void XLogRegisterBlock(uint8 block_id, RelFileNode *rnode,
							  ForkNumber forknum, BlockNumber blknum, char *page,
							  uint8 flags);
extern PGDLLIMPORT void XLogRegisterBufData(uint8 block_id, char *data, int len);
extern PGDLLIMPORT void XLogResetInsertion(void);
extern PGDLLIMPORT bool XLogCheckBufferNeedsBackup(Buffer buffer);

extern PGDLLIMPORT XLogRecPtr log_newpage(RelFileNode *rnode, ForkNumber forkNum,
							  BlockNumber blk, char *page, bool page_std);
extern PGDLLIMPORT void log_newpages(RelFileNode *rnode, ForkNumber forkNum, int num_pages,
						 BlockNumber *blknos, char **pages, bool page_std);
extern PGDLLIMPORT XLogRecPtr log_newpage_buffer(Buffer buffer, bool page_std);
extern PGDLLIMPORT void log_newpage_range(Relation rel, ForkNumber forkNum,
							  BlockNumber startblk, BlockNumber endblk, bool page_std);
extern PGDLLIMPORT XLogRecPtr XLogSaveBufferForHint(Buffer buffer, bool buffer_std);

extern PGDLLIMPORT void InitXLogInsert(void);

#endif							/* XLOGINSERT_H */
