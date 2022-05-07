/*-------------------------------------------------------------------------
 *
 * buffile.h
 *	  Management of large buffered temporary files.
 *
 * The BufFile routines provide a partial replacement for stdio atop
 * virtual file descriptors managed by fd.c.  Currently they only support
 * buffered access to a virtual file, without any of stdio's formatting
 * features.  That's enough for immediate needs, but the set of facilities
 * could be expanded if necessary.
 *
 * BufFile also supports working with temporary files that exceed the OS
 * file size limit and/or the largest offset representable in an int.
 * It might be better to split that out as a separately accessible module,
 * but currently we have no need for oversize temp files without buffered
 * access.
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/storage/buffile.h
 *
 *-------------------------------------------------------------------------
 */

#ifndef BUFFILE_H
#define BUFFILE_H

#include "storage/fileset.h"

/* BufFile is an opaque type whose details are not known outside buffile.c. */

typedef struct BufFile BufFile;

/*
 * prototypes for functions in buffile.c
 */

extern PGDLLIMPORT BufFile *BufFileCreateTemp(bool interXact);
extern PGDLLIMPORT void BufFileClose(BufFile *file);
extern PGDLLIMPORT size_t BufFileRead(BufFile *file, void *ptr, size_t size);
extern PGDLLIMPORT void BufFileWrite(BufFile *file, void *ptr, size_t size);
extern PGDLLIMPORT int	BufFileSeek(BufFile *file, int fileno, off_t offset, int whence);
extern PGDLLIMPORT void BufFileTell(BufFile *file, int *fileno, off_t *offset);
extern PGDLLIMPORT int	BufFileSeekBlock(BufFile *file, long blknum);
extern PGDLLIMPORT int64 BufFileSize(BufFile *file);
extern PGDLLIMPORT long BufFileAppend(BufFile *target, BufFile *source);

extern PGDLLIMPORT BufFile *BufFileCreateFileSet(FileSet *fileset, const char *name);
extern PGDLLIMPORT void BufFileExportFileSet(BufFile *file);
extern PGDLLIMPORT BufFile *BufFileOpenFileSet(FileSet *fileset, const char *name,
								   int mode, bool missing_ok);
extern PGDLLIMPORT void BufFileDeleteFileSet(FileSet *fileset, const char *name,
								 bool missing_ok);
extern PGDLLIMPORT void BufFileTruncateFileSet(BufFile *file, int fileno, off_t offset);

#endif							/* BUFFILE_H */
