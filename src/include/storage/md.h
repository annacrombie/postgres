/*-------------------------------------------------------------------------
 *
 * md.h
 *	  magnetic disk storage manager public interface declarations.
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/storage/md.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef MD_H
#define MD_H

#include "storage/block.h"
#include "storage/relfilenode.h"
#include "storage/smgr.h"
#include "storage/sync.h"

/* md storage manager functionality */
extern PGDLLIMPORT void mdinit(void);
extern PGDLLIMPORT void mdopen(SMgrRelation reln);
extern PGDLLIMPORT void mdclose(SMgrRelation reln, ForkNumber forknum);
extern PGDLLIMPORT void mdrelease(void);
extern PGDLLIMPORT void mdcreate(SMgrRelation reln, ForkNumber forknum, bool isRedo);
extern PGDLLIMPORT bool mdexists(SMgrRelation reln, ForkNumber forknum);
extern PGDLLIMPORT void mdunlink(RelFileNodeBackend rnode, ForkNumber forknum, bool isRedo);
extern PGDLLIMPORT void mdextend(SMgrRelation reln, ForkNumber forknum,
					 BlockNumber blocknum, char *buffer, bool skipFsync);
extern PGDLLIMPORT bool mdprefetch(SMgrRelation reln, ForkNumber forknum,
					   BlockNumber blocknum);
extern PGDLLIMPORT void mdread(SMgrRelation reln, ForkNumber forknum, BlockNumber blocknum,
				   char *buffer);
extern PGDLLIMPORT void mdwrite(SMgrRelation reln, ForkNumber forknum,
					BlockNumber blocknum, char *buffer, bool skipFsync);
extern PGDLLIMPORT void mdwriteback(SMgrRelation reln, ForkNumber forknum,
						BlockNumber blocknum, BlockNumber nblocks);
extern PGDLLIMPORT BlockNumber mdnblocks(SMgrRelation reln, ForkNumber forknum);
extern PGDLLIMPORT void mdtruncate(SMgrRelation reln, ForkNumber forknum,
					   BlockNumber nblocks);
extern PGDLLIMPORT void mdimmedsync(SMgrRelation reln, ForkNumber forknum);

extern PGDLLIMPORT void ForgetDatabaseSyncRequests(Oid dbid);
extern PGDLLIMPORT void DropRelationFiles(RelFileNode *delrels, int ndelrels, bool isRedo);

/* md sync callbacks */
extern PGDLLIMPORT int	mdsyncfiletag(const FileTag *ftag, char *path);
extern PGDLLIMPORT int	mdunlinkfiletag(const FileTag *ftag, char *path);
extern PGDLLIMPORT bool mdfiletagmatches(const FileTag *ftag, const FileTag *candidate);

#endif							/* MD_H */
