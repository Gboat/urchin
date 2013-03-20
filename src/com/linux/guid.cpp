#include "com/Com.h"
#include "com/comerror.h"
#include <stdlib.h>
#include <sys/time.h>

#define TICKSPERSEC 		10000000
#define TICKSPERMSEC       10000
#define SECSPERDAY         86400
#define SECSPERHOUR        3600
#define SECSPERMIN         60
#define MINSPERHOUR        60
#define HOURSPERDAY        24
#define EPOCHWEEKDAY       1  /* Jan 1, 1601 was Monday */
#define DAYSPERWEEK        7
#define EPOCHYEAR          1601
#define DAYSPERNORMALYEAR  365
#define DAYSPERLEAPYEAR    366
#define MONSPERYEAR        12
#define DAYSPERQUADRICENTENNIUM (365 * 400 + 97)
#define DAYSPERNORMALCENTURY (365 * 100 + 24)
#define DAYSPERNORMALQUADRENNIUM (365 * 4 + 1)

/* 1601 to 1970 is 369 years plus 89 leap days */
#define SECS_1601_TO_1970  ((369 * 365 + 89) * static_cast<uint64_t>(SECSPERDAY))
#define TICKS_1601_TO_1970 (SECS_1601_TO_1970 * TICKSPERSEC)
/* 1601 to 1980 is 379 years plus 91 leap days */
#define SECS_1601_TO_1980  ((379 * 365 + 91) * static_cast<uint64_t>(SECSPERDAY))
#define TICKS_1601_TO_1980 (SECS_1601_TO_1980 * TICKSPERSEC)
/* max ticks that can be represented as Unix time */
#define TICKS_1601_TO_UNIX_MAX ((SECS_1601_TO_1970 + INT_MAX) * TICKSPERSEC)
#define SECSPERDAY  86400
#define SECS_15_OCT_1582_TO_1601  ((17 + 30 + 31 + 365 * 18 + 5) * SECSPERDAY)
#define TICKS_15_OCT_1582_TO_1601 (static_cast<uint64_t>(SECS_15_OCT_1582_TO_1601) * TICKSPERSEC)
/* Number of 100ns ticks per clock tick. To be safe, assume that the clock
   resolution is at least 1000 * 100 * (1/1000000) = 1/10 of a second */
#define TICKS_PER_CLOCK_TICK 1000

#define MAX_ADAPTER_ADDRESS_LENGTH 8

static void RPC_UuidGetSystemTime(uint64_t *time)
{
    struct timeval now;
    uint64_t ft;

    //NtQuerySystemTime(&ft);
    gettimeofday( &now, 0 );
    //Time->QuadPart = now.tv_sec * (ULONGLONG)TICKSPERSEC + TICKS_1601_TO_1970;
    //Time->QuadPart += now.tv_usec * 10;
    ft = now.tv_sec * static_cast<uint64_t>(TICKSPERSEC) + TICKS_1601_TO_1970;
    ft += now.tv_usec * 10;

    *time = ft;
    *time += TICKS_15_OCT_1582_TO_1601;
}

static firtex::com::FX_HRESULT RPC_UuidGetNodeAddress(uint8_t *address,size_t address_len)
{
	// todo 
	// fix code so we can make proper UUID that are unique everywhere


	int i;
	for (i = 0; i < address_len; i++) {
		address[i] = rand() & 0xff;
	}

	address[0] |= 0x80;
	return FX_RPC_S_UUID_LOCAL_ONLY;
}

/*************************************************************************
 *           UuidCreate   [RPCRT4.@]
 */
static firtex::com::FX_HRESULT UuidCreate(firtex::com::FX_GUID *Uuid)
{
    static int initialised, count;

    uint64_t time;
    static uint64_t timelast;
    static uint16_t sequence;

    static uint32_t status;
    static uint8_t  address[MAX_ADAPTER_ADDRESS_LENGTH];

    // EnterCriticalSection(&uuid_cs);

    if (!initialised) {
        RPC_UuidGetSystemTime(&timelast);
        count = TICKS_PER_CLOCK_TICK;

        sequence = ((rand() & 0xff) << 8) + (rand() & 0xff);
        sequence &= 0x1fff;

        status = RPC_UuidGetNodeAddress(address, sizeof(address) );
        initialised = 1;
    }

    /* Generate time element of the UUID. Account for going faster
       than our clock as well as the clock going backwards. */
    while (1) {
        RPC_UuidGetSystemTime(&time);
        if (time > timelast) {
            count = 0;
            break;
        }
        if (time < timelast) {
            sequence = (sequence + 1) & 0x1fff;
            count = 0;
            break;
        }
        if (count < TICKS_PER_CLOCK_TICK) {
            count++;
            break;
        }
    }

    timelast = time;
    time += count;

    /* Pack the information into the UUID structure. */

    Uuid->Data1  = (unsigned long)(time & 0xffffffff);
    Uuid->Data2  = (unsigned short)((time >> 32) & 0xffff);
    Uuid->Data3  = (unsigned short)((time >> 48) & 0x0fff);

    /* This is a version 1 UUID */
    Uuid->Data3 |= (1 << 12);

    Uuid->Data4[0]  = sequence & 0xff;
    Uuid->Data4[1]  = (sequence & 0x3f00) >> 8;
    Uuid->Data4[1] |= 0x80;

    Uuid->Data4[2] = address[0];
    Uuid->Data4[3] = address[1];
    Uuid->Data4[4] = address[2];
    Uuid->Data4[5] = address[3];
    Uuid->Data4[6] = address[4];
    Uuid->Data4[7] = address[5];

    // LeaveCriticalSection(&uuid_cs);

    return status;
};

firtex::com::FX_HRESULT FX_STDAPICALLTYPE firtex::com::FX_CoCreateGuid(FX_GUID *pguid)
{
    return UuidCreate(pguid);
}

