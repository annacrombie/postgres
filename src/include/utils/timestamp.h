/*-------------------------------------------------------------------------
 *
 * timestamp.h
 *	  Definitions for the SQL "timestamp" and "interval" types.
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/utils/timestamp.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include "datatype/timestamp.h"
#include "fmgr.h"
#include "pgtime.h"


/*
 * Macros for fmgr-callable functions.
 *
 * For Timestamp, we make use of the same support routines as for int64.
 * Therefore Timestamp is pass-by-reference if and only if int64 is!
 */
#define DatumGetTimestamp(X)  ((Timestamp) DatumGetInt64(X))
#define DatumGetTimestampTz(X)	((TimestampTz) DatumGetInt64(X))
#define DatumGetIntervalP(X)  ((Interval *) DatumGetPointer(X))

#define TimestampGetDatum(X) Int64GetDatum(X)
#define TimestampTzGetDatum(X) Int64GetDatum(X)
#define IntervalPGetDatum(X) PointerGetDatum(X)

#define PG_GETARG_TIMESTAMP(n) DatumGetTimestamp(PG_GETARG_DATUM(n))
#define PG_GETARG_TIMESTAMPTZ(n) DatumGetTimestampTz(PG_GETARG_DATUM(n))
#define PG_GETARG_INTERVAL_P(n) DatumGetIntervalP(PG_GETARG_DATUM(n))

#define PG_RETURN_TIMESTAMP(x) return TimestampGetDatum(x)
#define PG_RETURN_TIMESTAMPTZ(x) return TimestampTzGetDatum(x)
#define PG_RETURN_INTERVAL_P(x) return IntervalPGetDatum(x)


#define TIMESTAMP_MASK(b) (1 << (b))
#define INTERVAL_MASK(b) (1 << (b))

/* Macros to handle packing and unpacking the typmod field for intervals */
#define INTERVAL_FULL_RANGE (0x7FFF)
#define INTERVAL_RANGE_MASK (0x7FFF)
#define INTERVAL_FULL_PRECISION (0xFFFF)
#define INTERVAL_PRECISION_MASK (0xFFFF)
#define INTERVAL_TYPMOD(p,r) ((((r) & INTERVAL_RANGE_MASK) << 16) | ((p) & INTERVAL_PRECISION_MASK))
#define INTERVAL_PRECISION(t) ((t) & INTERVAL_PRECISION_MASK)
#define INTERVAL_RANGE(t) (((t) >> 16) & INTERVAL_RANGE_MASK)

#define TimestampTzPlusMilliseconds(tz,ms) ((tz) + ((ms) * (int64) 1000))


/* Set at postmaster start */
extern PGDLLIMPORT TimestampTz PgStartTime;

/* Set at configuration reload */
extern PGDLLIMPORT TimestampTz PgReloadTime;


/* Internal routines (not fmgr-callable) */

extern PGDLLIMPORT int32 anytimestamp_typmod_check(bool istz, int32 typmod);

extern PGDLLIMPORT TimestampTz GetCurrentTimestamp(void);
extern PGDLLIMPORT TimestampTz GetSQLCurrentTimestamp(int32 typmod);
extern PGDLLIMPORT Timestamp GetSQLLocalTimestamp(int32 typmod);
extern PGDLLIMPORT void TimestampDifference(TimestampTz start_time, TimestampTz stop_time,
								long *secs, int *microsecs);
extern PGDLLIMPORT long TimestampDifferenceMilliseconds(TimestampTz start_time,
											TimestampTz stop_time);
extern PGDLLIMPORT bool TimestampDifferenceExceeds(TimestampTz start_time,
									   TimestampTz stop_time,
									   int msec);

extern PGDLLIMPORT TimestampTz time_t_to_timestamptz(pg_time_t tm);
extern PGDLLIMPORT pg_time_t timestamptz_to_time_t(TimestampTz t);

extern PGDLLIMPORT const char *timestamptz_to_str(TimestampTz t);

extern PGDLLIMPORT int	tm2timestamp(struct pg_tm *tm, fsec_t fsec, int *tzp, Timestamp *dt);
extern PGDLLIMPORT int	timestamp2tm(Timestamp dt, int *tzp, struct pg_tm *tm,
						 fsec_t *fsec, const char **tzn, pg_tz *attimezone);
extern PGDLLIMPORT void dt2time(Timestamp dt, int *hour, int *min, int *sec, fsec_t *fsec);

extern PGDLLIMPORT void interval2itm(Interval span, struct pg_itm *itm);
extern PGDLLIMPORT int	itm2interval(struct pg_itm *itm, Interval *span);
extern PGDLLIMPORT int	itmin2interval(struct pg_itm_in *itm_in, Interval *span);

extern PGDLLIMPORT Timestamp SetEpochTimestamp(void);
extern PGDLLIMPORT void GetEpochTime(struct pg_tm *tm);

extern PGDLLIMPORT int	timestamp_cmp_internal(Timestamp dt1, Timestamp dt2);

/* timestamp comparison works for timestamptz also */
#define timestamptz_cmp_internal(dt1,dt2)	timestamp_cmp_internal(dt1, dt2)

extern PGDLLIMPORT TimestampTz timestamp2timestamptz_opt_overflow(Timestamp timestamp,
													  int *overflow);
extern PGDLLIMPORT int32 timestamp_cmp_timestamptz_internal(Timestamp timestampVal,
												TimestampTz dt2);

extern PGDLLIMPORT int	isoweek2j(int year, int week);
extern PGDLLIMPORT void isoweek2date(int woy, int *year, int *mon, int *mday);
extern PGDLLIMPORT void isoweekdate2date(int isoweek, int wday, int *year, int *mon, int *mday);
extern PGDLLIMPORT int	date2isoweek(int year, int mon, int mday);
extern PGDLLIMPORT int	date2isoyear(int year, int mon, int mday);
extern PGDLLIMPORT int	date2isoyearday(int year, int mon, int mday);

extern PGDLLIMPORT bool TimestampTimestampTzRequiresRewrite(void);

#endif							/* TIMESTAMP_H */
