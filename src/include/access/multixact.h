/*
 * multixact.h
 *
 * PostgreSQL multi-transaction-log manager
 *
 * Portions Copyright (c) 1996-2022, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/access/multixact.h
 */
#ifndef MULTIXACT_H
#define MULTIXACT_H

#include "access/xlogreader.h"
#include "lib/stringinfo.h"
#include "storage/sync.h"


/*
 * The first two MultiXactId values are reserved to store the truncation Xid
 * and epoch of the first segment, so we start assigning multixact values from
 * 2.
 */
#define InvalidMultiXactId	((MultiXactId) 0)
#define FirstMultiXactId	((MultiXactId) 1)
#define MaxMultiXactId		((MultiXactId) 0xFFFFFFFF)

#define MultiXactIdIsValid(multi) ((multi) != InvalidMultiXactId)

#define MaxMultiXactOffset	((MultiXactOffset) 0xFFFFFFFF)

/* Number of SLRU buffers to use for multixact */
#define NUM_MULTIXACTOFFSET_BUFFERS		8
#define NUM_MULTIXACTMEMBER_BUFFERS		16

/*
 * Possible multixact lock modes ("status").  The first four modes are for
 * tuple locks (FOR KEY SHARE, FOR SHARE, FOR NO KEY UPDATE, FOR UPDATE); the
 * next two are used for update and delete modes.
 */
typedef enum
{
	MultiXactStatusForKeyShare = 0x00,
	MultiXactStatusForShare = 0x01,
	MultiXactStatusForNoKeyUpdate = 0x02,
	MultiXactStatusForUpdate = 0x03,
	/* an update that doesn't touch "key" columns */
	MultiXactStatusNoKeyUpdate = 0x04,
	/* other updates, and delete */
	MultiXactStatusUpdate = 0x05
} MultiXactStatus;

#define MaxMultiXactStatus MultiXactStatusUpdate

/* does a status value correspond to a tuple update? */
#define ISUPDATE_from_mxstatus(status) \
			((status) > MultiXactStatusForUpdate)


typedef struct MultiXactMember
{
	TransactionId xid;
	MultiXactStatus status;
} MultiXactMember;


/* ----------------
 *		multixact-related XLOG entries
 * ----------------
 */

#define XLOG_MULTIXACT_ZERO_OFF_PAGE	0x00
#define XLOG_MULTIXACT_ZERO_MEM_PAGE	0x10
#define XLOG_MULTIXACT_CREATE_ID		0x20
#define XLOG_MULTIXACT_TRUNCATE_ID		0x30

typedef struct xl_multixact_create
{
	MultiXactId mid;			/* new MultiXact's ID */
	MultiXactOffset moff;		/* its starting offset in members file */
	int32		nmembers;		/* number of member XIDs */
	MultiXactMember members[FLEXIBLE_ARRAY_MEMBER];
} xl_multixact_create;

#define SizeOfMultiXactCreate (offsetof(xl_multixact_create, members))

typedef struct xl_multixact_truncate
{
	Oid			oldestMultiDB;

	/* to-be-truncated range of multixact offsets */
	MultiXactId startTruncOff;	/* just for completeness' sake */
	MultiXactId endTruncOff;

	/* to-be-truncated range of multixact members */
	MultiXactOffset startTruncMemb;
	MultiXactOffset endTruncMemb;
} xl_multixact_truncate;

#define SizeOfMultiXactTruncate (sizeof(xl_multixact_truncate))


extern PGDLLIMPORT MultiXactId MultiXactIdCreate(TransactionId xid1,
									 MultiXactStatus status1, TransactionId xid2,
									 MultiXactStatus status2);
extern PGDLLIMPORT MultiXactId MultiXactIdExpand(MultiXactId multi, TransactionId xid,
									 MultiXactStatus status);
extern PGDLLIMPORT MultiXactId MultiXactIdCreateFromMembers(int nmembers,
												MultiXactMember *members);

extern PGDLLIMPORT MultiXactId ReadNextMultiXactId(void);
extern PGDLLIMPORT void ReadMultiXactIdRange(MultiXactId *oldest, MultiXactId *next);
extern PGDLLIMPORT bool MultiXactIdIsRunning(MultiXactId multi, bool isLockOnly);
extern PGDLLIMPORT void MultiXactIdSetOldestMember(void);
extern PGDLLIMPORT int	GetMultiXactIdMembers(MultiXactId multi, MultiXactMember **xids,
								  bool allow_old, bool isLockOnly);
extern PGDLLIMPORT bool MultiXactIdPrecedes(MultiXactId multi1, MultiXactId multi2);
extern PGDLLIMPORT bool MultiXactIdPrecedesOrEquals(MultiXactId multi1,
										MultiXactId multi2);

extern PGDLLIMPORT int	multixactoffsetssyncfiletag(const FileTag *ftag, char *path);
extern PGDLLIMPORT int	multixactmemberssyncfiletag(const FileTag *ftag, char *path);

extern PGDLLIMPORT void AtEOXact_MultiXact(void);
extern PGDLLIMPORT void AtPrepare_MultiXact(void);
extern PGDLLIMPORT void PostPrepare_MultiXact(TransactionId xid);

extern PGDLLIMPORT Size MultiXactShmemSize(void);
extern PGDLLIMPORT void MultiXactShmemInit(void);
extern PGDLLIMPORT void BootStrapMultiXact(void);
extern PGDLLIMPORT void StartupMultiXact(void);
extern PGDLLIMPORT void TrimMultiXact(void);
extern PGDLLIMPORT void SetMultiXactIdLimit(MultiXactId oldest_datminmxid,
								Oid oldest_datoid,
								bool is_startup);
extern PGDLLIMPORT void MultiXactGetCheckptMulti(bool is_shutdown,
									 MultiXactId *nextMulti,
									 MultiXactOffset *nextMultiOffset,
									 MultiXactId *oldestMulti,
									 Oid *oldestMultiDB);
extern PGDLLIMPORT void CheckPointMultiXact(void);
extern PGDLLIMPORT MultiXactId GetOldestMultiXactId(void);
extern PGDLLIMPORT void TruncateMultiXact(MultiXactId oldestMulti, Oid oldestMultiDB);
extern PGDLLIMPORT void MultiXactSetNextMXact(MultiXactId nextMulti,
								  MultiXactOffset nextMultiOffset);
extern PGDLLIMPORT void MultiXactAdvanceNextMXact(MultiXactId minMulti,
									  MultiXactOffset minMultiOffset);
extern PGDLLIMPORT void MultiXactAdvanceOldest(MultiXactId oldestMulti, Oid oldestMultiDB);
extern PGDLLIMPORT int	MultiXactMemberFreezeThreshold(void);

extern PGDLLIMPORT void multixact_twophase_recover(TransactionId xid, uint16 info,
									   void *recdata, uint32 len);
extern PGDLLIMPORT void multixact_twophase_postcommit(TransactionId xid, uint16 info,
										  void *recdata, uint32 len);
extern PGDLLIMPORT void multixact_twophase_postabort(TransactionId xid, uint16 info,
										 void *recdata, uint32 len);

extern PGDLLIMPORT void multixact_redo(XLogReaderState *record);
extern PGDLLIMPORT void multixact_desc(StringInfo buf, XLogReaderState *record);
extern PGDLLIMPORT const char *multixact_identify(uint8 info);
extern PGDLLIMPORT char *mxid_to_string(MultiXactId multi, int nmembers,
							MultiXactMember *members);

#endif							/* MULTIXACT_H */
