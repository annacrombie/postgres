/*-------------------------------------------------------------------------
 *
 * fd.h
 *	  Virtual file descriptor definitions.
 *
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/storage/fd.h
 *
 *-------------------------------------------------------------------------
 */

/*
 * calls:
 *
 *	File {Close, Read, Write, Size, Sync}
 *	{Path Name Open, Allocate, Free} File
 *
 * These are NOT JUST RENAMINGS OF THE UNIX ROUTINES.
 * Use them for all file activity...
 *
 *	File fd;
 *	fd = PathNameOpenFile("foo", O_RDONLY);
 *
 *	AllocateFile();
 *	FreeFile();
 *
 * Use AllocateFile, not fopen, if you need a stdio file (FILE*); then
 * use FreeFile, not fclose, to close it.  AVOID using stdio for files
 * that you intend to hold open for any length of time, since there is
 * no way for them to share kernel file descriptors with other files.
 *
 * Likewise, use AllocateDir/FreeDir, not opendir/closedir, to allocate
 * open directories (DIR*), and OpenTransientFile/CloseTransientFile for an
 * unbuffered file descriptor.
 *
 * If you really can't use any of the above, at least call AcquireExternalFD
 * or ReserveExternalFD to report any file descriptors that are held for any
 * length of time.  Failure to do so risks unnecessary EMFILE errors.
 */
#ifndef FD_H
#define FD_H

#include <dirent.h>

typedef enum RecoveryInitSyncMethod
{
	RECOVERY_INIT_SYNC_METHOD_FSYNC,
	RECOVERY_INIT_SYNC_METHOD_SYNCFS
}			RecoveryInitSyncMethod;

struct iovec;					/* avoid including port/pg_iovec.h here */

typedef int File;


/* GUC parameter */
extern PGDLLIMPORT int max_files_per_process;
extern PGDLLIMPORT bool data_sync_retry;
extern PGDLLIMPORT int recovery_init_sync_method;

/*
 * This is private to fd.c, but exported for save/restore_backend_variables()
 */
extern PGDLLIMPORT int max_safe_fds;

/*
 * On Windows, we have to interpret EACCES as possibly meaning the same as
 * ENOENT, because if a file is unlinked-but-not-yet-gone on that platform,
 * that's what you get.  Ugh.  This code is designed so that we don't
 * actually believe these cases are okay without further evidence (namely,
 * a pending fsync request getting canceled ... see ProcessSyncRequests).
 */
#ifndef WIN32
#define FILE_POSSIBLY_DELETED(err)	((err) == ENOENT)
#else
#define FILE_POSSIBLY_DELETED(err)	((err) == ENOENT || (err) == EACCES)
#endif

/*
 * O_DIRECT is not standard, but almost every Unix has it.  We translate it
 * to the appropriate Windows flag in src/port/open.c.  We simulate it with
 * fcntl(F_NOCACHE) on macOS inside fd.c's open() wrapper.  We use the name
 * PG_O_DIRECT rather than defining O_DIRECT in that case (probably not a good
 * idea on a Unix).
 */
#if defined(O_DIRECT)
#define		PG_O_DIRECT O_DIRECT
#elif defined(F_NOCACHE)
#define		PG_O_DIRECT 0x80000000
#define		PG_O_DIRECT_USE_F_NOCACHE
#else
#define		PG_O_DIRECT 0
#endif

/*
 * prototypes for functions in fd.c
 */

/* Operations on virtual Files --- equivalent to Unix kernel file ops */
extern PGDLLIMPORT File PathNameOpenFile(const char *fileName, int fileFlags);
extern PGDLLIMPORT File PathNameOpenFilePerm(const char *fileName, int fileFlags, mode_t fileMode);
extern PGDLLIMPORT File OpenTemporaryFile(bool interXact);
extern PGDLLIMPORT void FileClose(File file);
extern PGDLLIMPORT int	FilePrefetch(File file, off_t offset, int amount, uint32 wait_event_info);
extern PGDLLIMPORT int	FileRead(File file, char *buffer, int amount, off_t offset, uint32 wait_event_info);
extern PGDLLIMPORT int	FileWrite(File file, char *buffer, int amount, off_t offset, uint32 wait_event_info);
extern PGDLLIMPORT int	FileSync(File file, uint32 wait_event_info);
extern PGDLLIMPORT off_t FileSize(File file);
extern PGDLLIMPORT int	FileTruncate(File file, off_t offset, uint32 wait_event_info);
extern PGDLLIMPORT void FileWriteback(File file, off_t offset, off_t nbytes, uint32 wait_event_info);
extern PGDLLIMPORT char *FilePathName(File file);
extern PGDLLIMPORT int	FileGetRawDesc(File file);
extern PGDLLIMPORT int	FileGetRawFlags(File file);
extern PGDLLIMPORT mode_t FileGetRawMode(File file);

/* Operations used for sharing named temporary files */
extern PGDLLIMPORT File PathNameCreateTemporaryFile(const char *name, bool error_on_failure);
extern PGDLLIMPORT File PathNameOpenTemporaryFile(const char *path, int mode);
extern PGDLLIMPORT bool PathNameDeleteTemporaryFile(const char *name, bool error_on_failure);
extern PGDLLIMPORT void PathNameCreateTemporaryDir(const char *base, const char *name);
extern PGDLLIMPORT void PathNameDeleteTemporaryDir(const char *name);
extern PGDLLIMPORT void TempTablespacePath(char *path, Oid tablespace);

/* Operations that allow use of regular stdio --- USE WITH CAUTION */
extern PGDLLIMPORT FILE *AllocateFile(const char *name, const char *mode);
extern PGDLLIMPORT int	FreeFile(FILE *file);

/* Operations that allow use of pipe streams (popen/pclose) */
extern PGDLLIMPORT FILE *OpenPipeStream(const char *command, const char *mode);
extern PGDLLIMPORT int	ClosePipeStream(FILE *file);

/* Operations to allow use of the <dirent.h> library routines */
extern PGDLLIMPORT DIR *AllocateDir(const char *dirname);
extern PGDLLIMPORT struct dirent *ReadDir(DIR *dir, const char *dirname);
extern PGDLLIMPORT struct dirent *ReadDirExtended(DIR *dir, const char *dirname,
									  int elevel);
extern PGDLLIMPORT int	FreeDir(DIR *dir);

/* Operations to allow use of a plain kernel FD, with automatic cleanup */
extern PGDLLIMPORT int	OpenTransientFile(const char *fileName, int fileFlags);
extern PGDLLIMPORT int	OpenTransientFilePerm(const char *fileName, int fileFlags, mode_t fileMode);
extern PGDLLIMPORT int	CloseTransientFile(int fd);

/* If you've really really gotta have a plain kernel FD, use this */
extern PGDLLIMPORT int	BasicOpenFile(const char *fileName, int fileFlags);
extern PGDLLIMPORT int	BasicOpenFilePerm(const char *fileName, int fileFlags, mode_t fileMode);

/* Use these for other cases, and also for long-lived BasicOpenFile FDs */
extern PGDLLIMPORT bool AcquireExternalFD(void);
extern PGDLLIMPORT void ReserveExternalFD(void);
extern PGDLLIMPORT void ReleaseExternalFD(void);

/* Make a directory with default permissions */
extern PGDLLIMPORT int	MakePGDirectory(const char *directoryName);

/* Miscellaneous support routines */
extern PGDLLIMPORT void InitFileAccess(void);
extern PGDLLIMPORT void InitTemporaryFileAccess(void);
extern PGDLLIMPORT void set_max_safe_fds(void);
extern PGDLLIMPORT void closeAllVfds(void);
extern PGDLLIMPORT void SetTempTablespaces(Oid *tableSpaces, int numSpaces);
extern PGDLLIMPORT bool TempTablespacesAreSet(void);
extern PGDLLIMPORT int	GetTempTablespaces(Oid *tableSpaces, int numSpaces);
extern PGDLLIMPORT Oid	GetNextTempTableSpace(void);
extern PGDLLIMPORT void AtEOXact_Files(bool isCommit);
extern PGDLLIMPORT void AtEOSubXact_Files(bool isCommit, SubTransactionId mySubid,
							  SubTransactionId parentSubid);
extern PGDLLIMPORT void RemovePgTempFiles(void);
extern PGDLLIMPORT void RemovePgTempFilesInDir(const char *tmpdirname, bool missing_ok,
								   bool unlink_all);
extern PGDLLIMPORT bool looks_like_temp_rel_name(const char *name);

extern PGDLLIMPORT int	pg_fsync(int fd);
extern PGDLLIMPORT int	pg_fsync_no_writethrough(int fd);
extern PGDLLIMPORT int	pg_fsync_writethrough(int fd);
extern PGDLLIMPORT int	pg_fdatasync(int fd);
extern PGDLLIMPORT void pg_flush_data(int fd, off_t offset, off_t amount);
extern PGDLLIMPORT ssize_t pg_pwritev_with_retry(int fd,
									 const struct iovec *iov,
									 int iovcnt,
									 off_t offset);
extern PGDLLIMPORT int	pg_truncate(const char *path, off_t length);
extern PGDLLIMPORT void fsync_fname(const char *fname, bool isdir);
extern PGDLLIMPORT int	fsync_fname_ext(const char *fname, bool isdir, bool ignore_perm, int elevel);
extern PGDLLIMPORT int	durable_rename(const char *oldfile, const char *newfile, int loglevel);
extern PGDLLIMPORT int	durable_unlink(const char *fname, int loglevel);
extern PGDLLIMPORT int	durable_rename_excl(const char *oldfile, const char *newfile, int loglevel);
extern PGDLLIMPORT void SyncDataDirectory(void);
extern PGDLLIMPORT int	data_sync_elevel(int elevel);

/* Filename components */
#define PG_TEMP_FILES_DIR "pgsql_tmp"
#define PG_TEMP_FILE_PREFIX "pgsql_tmp"

#endif							/* FD_H */
