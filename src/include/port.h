/*-------------------------------------------------------------------------
 *
 * port.h
 *	  Header for src/port/ compatibility functions.
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/port.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PG_PORT_H
#define PG_PORT_H

#include <ctype.h>

/*
 * Windows has enough specialized port stuff that we push most of it off
 * into another file.
 * Note: Some CYGWIN includes might #define WIN32.
 */
#if defined(WIN32) && !defined(__CYGWIN__)
#include "port/win32_port.h"
#endif

/* socket has a different definition on WIN32 */
#ifndef WIN32
typedef int pgsocket;

#define PGINVALID_SOCKET (-1)
#else
typedef SOCKET pgsocket;

#define PGINVALID_SOCKET INVALID_SOCKET
#endif

/* if platform lacks socklen_t, we assume this will work */
#ifndef HAVE_SOCKLEN_T
typedef unsigned int socklen_t;
#endif

/* non-blocking */
extern PGDLLIMPORT bool pg_set_noblock(pgsocket sock);
extern PGDLLIMPORT bool pg_set_block(pgsocket sock);

/* Portable path handling for Unix/Win32 (in path.c) */

extern PGDLLIMPORT bool has_drive_prefix(const char *filename);
extern PGDLLIMPORT char *first_dir_separator(const char *filename);
extern PGDLLIMPORT char *last_dir_separator(const char *filename);
extern PGDLLIMPORT char *first_path_var_separator(const char *pathlist);
extern PGDLLIMPORT void join_path_components(char *ret_path,
								 const char *head, const char *tail);
extern PGDLLIMPORT void canonicalize_path(char *path);
extern PGDLLIMPORT void make_native_path(char *path);
extern PGDLLIMPORT void cleanup_path(char *path);
extern PGDLLIMPORT bool path_contains_parent_reference(const char *path);
extern PGDLLIMPORT bool path_is_relative_and_below_cwd(const char *path);
extern PGDLLIMPORT bool path_is_prefix_of_path(const char *path1, const char *path2);
extern PGDLLIMPORT char *make_absolute_path(const char *path);
extern PGDLLIMPORT const char *get_progname(const char *argv0);
extern PGDLLIMPORT void get_share_path(const char *my_exec_path, char *ret_path);
extern PGDLLIMPORT void get_etc_path(const char *my_exec_path, char *ret_path);
extern PGDLLIMPORT void get_include_path(const char *my_exec_path, char *ret_path);
extern PGDLLIMPORT void get_pkginclude_path(const char *my_exec_path, char *ret_path);
extern PGDLLIMPORT void get_includeserver_path(const char *my_exec_path, char *ret_path);
extern PGDLLIMPORT void get_lib_path(const char *my_exec_path, char *ret_path);
extern PGDLLIMPORT void get_pkglib_path(const char *my_exec_path, char *ret_path);
extern PGDLLIMPORT void get_locale_path(const char *my_exec_path, char *ret_path);
extern PGDLLIMPORT void get_doc_path(const char *my_exec_path, char *ret_path);
extern PGDLLIMPORT void get_html_path(const char *my_exec_path, char *ret_path);
extern PGDLLIMPORT void get_man_path(const char *my_exec_path, char *ret_path);
extern PGDLLIMPORT bool get_home_path(char *ret_path);
extern PGDLLIMPORT void get_parent_directory(char *path);

/* common/pgfnames.c */
extern PGDLLIMPORT char **pgfnames(const char *path);
extern PGDLLIMPORT void pgfnames_cleanup(char **filenames);

/*
 *	is_absolute_path
 *
 *	By making this a macro we avoid needing to include path.c in libpq.
 */
#ifndef WIN32
#define IS_DIR_SEP(ch)	((ch) == '/')

#define is_absolute_path(filename) \
( \
	IS_DIR_SEP((filename)[0]) \
)
#else
#define IS_DIR_SEP(ch)	((ch) == '/' || (ch) == '\\')

/* See path_is_relative_and_below_cwd() for how we handle 'E:abc'. */
#define is_absolute_path(filename) \
( \
	IS_DIR_SEP((filename)[0]) || \
	(isalpha((unsigned char) ((filename)[0])) && (filename)[1] == ':' && \
	 IS_DIR_SEP((filename)[2])) \
)
#endif

/*
 * This macro provides a centralized list of all errnos that identify
 * hard failure of a previously-established network connection.
 * The macro is intended to be used in a switch statement, in the form
 * "case ALL_CONNECTION_FAILURE_ERRNOS:".
 *
 * Note: this groups EPIPE and ECONNRESET, which we take to indicate a
 * probable server crash, with other errors that indicate loss of network
 * connectivity without proving much about the server's state.  Places that
 * are actually reporting errors typically single out EPIPE and ECONNRESET,
 * while allowing the network failures to be reported generically.
 */
#define ALL_CONNECTION_FAILURE_ERRNOS \
	EPIPE: \
	case ECONNRESET: \
	case ECONNABORTED: \
	case EHOSTDOWN: \
	case EHOSTUNREACH: \
	case ENETDOWN: \
	case ENETRESET: \
	case ENETUNREACH: \
	case ETIMEDOUT

/* Portable locale initialization (in exec.c) */
extern PGDLLIMPORT void set_pglocale_pgservice(const char *argv0, const char *app);

/* Portable way to find and execute binaries (in exec.c) */
extern PGDLLIMPORT int	validate_exec(const char *path);
extern PGDLLIMPORT int	find_my_exec(const char *argv0, char *retpath);
extern PGDLLIMPORT int	find_other_exec(const char *argv0, const char *target,
							const char *versionstr, char *retpath);
extern PGDLLIMPORT char *pipe_read_line(char *cmd, char *line, int maxsize);

/* Doesn't belong here, but this is used with find_other_exec(), so... */
#define PG_BACKEND_VERSIONSTR "postgres (PostgreSQL) " PG_VERSION "\n"

#ifdef EXEC_BACKEND
/* Disable ASLR before exec, for developer builds only (in exec.c) */
extern PGDLLIMPORT int pg_disable_aslr(void);
#endif


#if defined(WIN32) || defined(__CYGWIN__)
#define EXE ".exe"
#else
#define EXE ""
#endif

#if defined(WIN32) && !defined(__CYGWIN__)
#define DEVNULL "nul"
#else
#define DEVNULL "/dev/null"
#endif

/* Portable delay handling */
extern PGDLLIMPORT void pg_usleep(long microsec);

/* Portable SQL-like case-independent comparisons and conversions */
extern PGDLLIMPORT int	pg_strcasecmp(const char *s1, const char *s2);
extern PGDLLIMPORT int	pg_strncasecmp(const char *s1, const char *s2, size_t n);
extern PGDLLIMPORT unsigned char pg_toupper(unsigned char ch);
extern PGDLLIMPORT unsigned char pg_tolower(unsigned char ch);
extern PGDLLIMPORT unsigned char pg_ascii_toupper(unsigned char ch);
extern PGDLLIMPORT unsigned char pg_ascii_tolower(unsigned char ch);

/*
 * Beginning in v12, we always replace snprintf() and friends with our own
 * implementation.  This symbol is no longer consulted by the core code,
 * but keep it defined anyway in case any extensions are looking at it.
 */
#define USE_REPL_SNPRINTF 1

/*
 * Versions of libintl >= 0.13 try to replace printf() and friends with
 * macros to their own versions that understand the %$ format.  We do the
 * same, so disable their macros, if they exist.
 */
#ifdef vsnprintf
#undef vsnprintf
#endif
#ifdef snprintf
#undef snprintf
#endif
#ifdef vsprintf
#undef vsprintf
#endif
#ifdef sprintf
#undef sprintf
#endif
#ifdef vfprintf
#undef vfprintf
#endif
#ifdef fprintf
#undef fprintf
#endif
#ifdef vprintf
#undef vprintf
#endif
#ifdef printf
#undef printf
#endif

extern PGDLLIMPORT int	pg_vsnprintf(char *str, size_t count, const char *fmt, va_list args);
extern PGDLLIMPORT int	pg_snprintf(char *str, size_t count, const char *fmt,...) pg_attribute_printf(3, 4);
extern PGDLLIMPORT int	pg_vsprintf(char *str, const char *fmt, va_list args);
extern PGDLLIMPORT int	pg_sprintf(char *str, const char *fmt,...) pg_attribute_printf(2, 3);
extern PGDLLIMPORT int	pg_vfprintf(FILE *stream, const char *fmt, va_list args);
extern PGDLLIMPORT int	pg_fprintf(FILE *stream, const char *fmt,...) pg_attribute_printf(2, 3);
extern PGDLLIMPORT int	pg_vprintf(const char *fmt, va_list args);
extern PGDLLIMPORT int	pg_printf(const char *fmt,...) pg_attribute_printf(1, 2);

/*
 * We use __VA_ARGS__ for printf to prevent replacing references to
 * the "printf" format archetype in format() attribute declarations.
 * That unfortunately means that taking a function pointer to printf
 * will not do what we'd wish.  (If you need to do that, you must name
 * pg_printf explicitly.)  For printf's sibling functions, use
 * parameterless macros so that function pointers will work unsurprisingly.
 */
#define vsnprintf		pg_vsnprintf
#define snprintf		pg_snprintf
#define vsprintf		pg_vsprintf
#define sprintf			pg_sprintf
#define vfprintf		pg_vfprintf
#define fprintf			pg_fprintf
#define vprintf			pg_vprintf
#define printf(...)		pg_printf(__VA_ARGS__)

/* This is also provided by snprintf.c */
extern PGDLLIMPORT int	pg_strfromd(char *str, size_t count, int precision, double value);

/* Replace strerror() with our own, somewhat more robust wrapper */
extern PGDLLIMPORT char *pg_strerror(int errnum);
#define strerror pg_strerror

/* Likewise for strerror_r(); note we prefer the GNU API for that */
extern PGDLLIMPORT char *pg_strerror_r(int errnum, char *buf, size_t buflen);
#define strerror_r pg_strerror_r
#define PG_STRERROR_R_BUFLEN 256	/* Recommended buffer size for strerror_r */

/* Wrap strsignal(), or provide our own version if necessary */
extern PGDLLIMPORT const char *pg_strsignal(int signum);

extern PGDLLIMPORT int	pclose_check(FILE *stream);

/* Global variable holding time zone information. */
#if defined(WIN32) || defined(__CYGWIN__)
#define TIMEZONE_GLOBAL _timezone
#define TZNAME_GLOBAL _tzname
#else
#define TIMEZONE_GLOBAL timezone
#define TZNAME_GLOBAL tzname
#endif

#if defined(WIN32) || defined(__CYGWIN__)
/*
 *	Win32 doesn't have reliable rename/unlink during concurrent access.
 */
extern PGDLLIMPORT int	pgrename(const char *from, const char *to);
extern PGDLLIMPORT int	pgunlink(const char *path);

/* Include this first so later includes don't see these defines */
#ifdef _MSC_VER
#include <io.h>
#endif

#define rename(from, to)		pgrename(from, to)
#define unlink(path)			pgunlink(path)
#endif							/* defined(WIN32) || defined(__CYGWIN__) */

/*
 *	Win32 also doesn't have symlinks, but we can emulate them with
 *	junction points on newer Win32 versions.
 *
 *	Cygwin has its own symlinks which work on Win95/98/ME where
 *	junction points don't, so use those instead.  We have no way of
 *	knowing what type of system Cygwin binaries will be run on.
 *		Note: Some CYGWIN includes might #define WIN32.
 */
#if defined(WIN32) && !defined(__CYGWIN__)
extern PGDLLIMPORT int	pgsymlink(const char *oldpath, const char *newpath);
extern PGDLLIMPORT int	pgreadlink(const char *path, char *buf, size_t size);
extern PGDLLIMPORT bool pgwin32_is_junction(const char *path);

#define symlink(oldpath, newpath)	pgsymlink(oldpath, newpath)
#define readlink(path, buf, size)	pgreadlink(path, buf, size)
#endif

extern PGDLLIMPORT bool rmtree(const char *path, bool rmtopdir);

#if defined(WIN32) && !defined(__CYGWIN__)

/*
 * open() and fopen() replacements to allow deletion of open files and
 * passing of other special options.
 */
#define		O_DIRECT	0x80000000
extern PGDLLIMPORT HANDLE pgwin32_open_handle(const char *, int, bool);
extern PGDLLIMPORT int	pgwin32_open(const char *, int,...);
extern PGDLLIMPORT FILE *pgwin32_fopen(const char *, const char *);
#define		open(a,b,c) pgwin32_open(a,b,c)
#define		fopen(a,b) pgwin32_fopen(a,b)

/*
 * Mingw-w64 headers #define popen and pclose to _popen and _pclose.  We want
 * to use our popen wrapper, rather than plain _popen, so override that.  For
 * consistency, use our version of pclose, too.
 */
#ifdef popen
#undef popen
#endif
#ifdef pclose
#undef pclose
#endif

/*
 * system() and popen() replacements to enclose the command in an extra
 * pair of quotes.
 */
extern PGDLLIMPORT int	pgwin32_system(const char *command);
extern PGDLLIMPORT FILE *pgwin32_popen(const char *command, const char *type);

#define system(a) pgwin32_system(a)
#define popen(a,b) pgwin32_popen(a,b)
#define pclose(a) _pclose(a)

/* New versions of MingW have gettimeofday, old mingw and msvc don't */
#ifndef HAVE_GETTIMEOFDAY
/* Last parameter not used */
extern PGDLLIMPORT int	gettimeofday(struct timeval *tp, struct timezone *tzp);
#endif
#else							/* !WIN32 */

/*
 *	Win32 requires a special close for sockets and pipes, while on Unix
 *	close() does them all.
 */
#define closesocket close
#endif							/* WIN32 */

/*
 * On Windows, setvbuf() does not support _IOLBF mode, and interprets that
 * as _IOFBF.  To add insult to injury, setvbuf(file, NULL, _IOFBF, 0)
 * crashes outright if "parameter validation" is enabled.  Therefore, in
 * places where we'd like to select line-buffered mode, we fall back to
 * unbuffered mode instead on Windows.  Always use PG_IOLBF not _IOLBF
 * directly in order to implement this behavior.
 */
#ifndef WIN32
#define PG_IOLBF	_IOLBF
#else
#define PG_IOLBF	_IONBF
#endif

/*
 * Default "extern" declarations or macro substitutes for library routines.
 * When necessary, these routines are provided by files in src/port/.
 */

/* Type to use with fseeko/ftello */
#ifndef WIN32					/* WIN32 is handled in port/win32_port.h */
#define pgoff_t off_t
#endif

#ifndef HAVE_FLS
extern PGDLLIMPORT int	fls(int mask);
#endif

#ifndef HAVE_GETPEEREID
/* On Windows, Perl might have incompatible definitions of uid_t and gid_t. */
#ifndef PLPERL_HAVE_UID_GID
extern PGDLLIMPORT int	getpeereid(int sock, uid_t *uid, gid_t *gid);
#endif
#endif

/*
 * Glibc doesn't use the builtin for clang due to a *gcc* bug in a version
 * newer than the gcc compatibility clang claims to have. This would cause a
 * *lot* of superfluous function calls, therefore revert when using clang. In
 * C++ there's issues with libc++ (not libstdc++), so disable as well.
 */
#if defined(__clang__) && !defined(__cplusplus)
/* needs to be separate to not confuse other compilers */
#if __has_builtin(__builtin_isinf)
/* need to include before, to avoid getting overwritten */
#include <math.h>
#undef isinf
#define isinf __builtin_isinf
#endif							/* __has_builtin(isinf) */
#endif							/* __clang__ && !__cplusplus */

#ifndef HAVE_EXPLICIT_BZERO
extern PGDLLIMPORT void explicit_bzero(void *buf, size_t len);
#endif

#ifndef HAVE_STRTOF
extern PGDLLIMPORT float strtof(const char *nptr, char **endptr);
#endif

#ifdef HAVE_BUGGY_STRTOF
extern PGDLLIMPORT float pg_strtof(const char *nptr, char **endptr);
#define strtof(a,b) (pg_strtof((a),(b)))
#endif

#ifndef HAVE_LINK
extern PGDLLIMPORT int	link(const char *src, const char *dst);
#endif

#ifndef HAVE_MKDTEMP
extern PGDLLIMPORT char *mkdtemp(char *path);
#endif

#ifndef HAVE_INET_ATON
#include <netinet/in.h>
#include <arpa/inet.h>
extern PGDLLIMPORT int	inet_aton(const char *cp, struct in_addr *addr);
#endif

/*
 * Windows and older Unix don't have pread(2) and pwrite(2).  We have
 * replacement functions, but they have slightly different semantics so we'll
 * use a name with a pg_ prefix to avoid confusion.
 */
#ifdef HAVE_PREAD
#define pg_pread pread
#else
extern PGDLLIMPORT ssize_t pg_pread(int fd, void *buf, size_t nbyte, off_t offset);
#endif

#ifdef HAVE_PWRITE
#define pg_pwrite pwrite
#else
extern PGDLLIMPORT ssize_t pg_pwrite(int fd, const void *buf, size_t nbyte, off_t offset);
#endif

/* For pg_pwritev() and pg_preadv(), see port/pg_iovec.h. */

#if !HAVE_DECL_STRLCAT
extern PGDLLIMPORT size_t strlcat(char *dst, const char *src, size_t siz);
#endif

#if !HAVE_DECL_STRLCPY
extern PGDLLIMPORT size_t strlcpy(char *dst, const char *src, size_t siz);
#endif

#if !HAVE_DECL_STRNLEN
extern PGDLLIMPORT size_t strnlen(const char *str, size_t maxlen);
#endif

#ifndef HAVE_SETENV
extern PGDLLIMPORT int	setenv(const char *name, const char *value, int overwrite);
#endif

#ifndef HAVE_UNSETENV
extern PGDLLIMPORT int	unsetenv(const char *name);
#endif

#ifndef HAVE_DLOPEN
extern PGDLLIMPORT void *dlopen(const char *file, int mode);
extern PGDLLIMPORT void *dlsym(void *handle, const char *symbol);
extern PGDLLIMPORT int	dlclose(void *handle);
extern PGDLLIMPORT char *dlerror(void);
#endif

/*
 * In some older systems, the RTLD_NOW flag isn't defined and the mode
 * argument to dlopen must always be 1.
 */
#if !HAVE_DECL_RTLD_NOW
#define RTLD_NOW 1
#endif

/*
 * The RTLD_GLOBAL flag is wanted if available, but it doesn't exist
 * everywhere.  If it doesn't exist, set it to 0 so it has no effect.
 */
#if !HAVE_DECL_RTLD_GLOBAL
#define RTLD_GLOBAL 0
#endif

/* thread.c */
#ifndef WIN32
extern PGDLLIMPORT bool pg_get_user_name(uid_t user_id, char *buffer, size_t buflen);
extern PGDLLIMPORT bool pg_get_user_home_dir(uid_t user_id, char *buffer, size_t buflen);
#endif

extern PGDLLIMPORT void pg_qsort(void *base, size_t nel, size_t elsize,
					 int (*cmp) (const void *, const void *));
extern PGDLLIMPORT int	pg_qsort_strcmp(const void *a, const void *b);

#define qsort(a,b,c,d) pg_qsort(a,b,c,d)

typedef int (*qsort_arg_comparator) (const void *a, const void *b, void *arg);

extern PGDLLIMPORT void qsort_arg(void *base, size_t nel, size_t elsize,
					  qsort_arg_comparator cmp, void *arg);

extern PGDLLIMPORT void *bsearch_arg(const void *key, const void *base,
						 size_t nmemb, size_t size,
						 int (*compar) (const void *, const void *, void *),
						 void *arg);

/* port/chklocale.c */
extern PGDLLIMPORT int	pg_get_encoding_from_locale(const char *ctype, bool write_message);

#if defined(WIN32) && !defined(FRONTEND)
extern PGDLLIMPORT int	pg_codepage_to_encoding(UINT cp);
#endif

/* port/inet_net_ntop.c */
extern PGDLLIMPORT char *pg_inet_net_ntop(int af, const void *src, int bits,
							  char *dst, size_t size);

/* port/pg_strong_random.c */
extern PGDLLIMPORT void pg_strong_random_init(void);
extern PGDLLIMPORT bool pg_strong_random(void *buf, size_t len);

/*
 * pg_backend_random used to be a wrapper for pg_strong_random before
 * Postgres 12 for the backend code.
 */
#define pg_backend_random pg_strong_random

/* port/pgcheckdir.c */
extern PGDLLIMPORT int	pg_check_dir(const char *dir);

/* port/pgmkdirp.c */
extern PGDLLIMPORT int	pg_mkdir_p(char *path, int omode);

/* port/pqsignal.c */
typedef void (*pqsigfunc) (int signo);
extern PGDLLIMPORT pqsigfunc pqsignal(int signo, pqsigfunc func);

/* port/quotes.c */
extern PGDLLIMPORT char *escape_single_quotes_ascii(const char *src);

/* common/wait_error.c */
extern PGDLLIMPORT char *wait_result_to_str(int exit_status);
extern PGDLLIMPORT bool wait_result_is_signal(int exit_status, int signum);
extern PGDLLIMPORT bool wait_result_is_any_signal(int exit_status, bool include_command_not_found);

#endif							/* PG_PORT_H */
