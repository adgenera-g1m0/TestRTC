#ifndef _GESTIONERTC_H
#define _GESTIONERTC_H

#ifdef NEW_FUNCTIONS
#include "time.h"
#endif

#ifndef NEW_FUNCTIONS

struct tm {
	int tm_sec; //	int	seconds after the minute	0-61*
	int tm_min; //	int	minutes after the hour	0-59
	int tm_hour; //		int	hours since midnight	0-23
	int tm_mday; //		int	day of the month	1-31
	int tm_mon; //		int	months since January	0-11
	int tm_year; //		int	years since 1900
	int tm_wday; //		int	days since Sunday	0-6
	int tm_yday; //		int	days since January 1	0-365
	int tm_isdst; //		int	Daylight Saving Time flag
};


#define YEAR0           1900                    /* the first year */
#define EPOCH_YR        1970            /* EPOCH = Jan 1 1970 00:00:00 */
#define SECS_DAY        (24L * 60L * 60L)
#define LEAPYEAR(year)  (!((year) % 4) && (((year) % 100) || !((year) % 400)))
#define YEARSIZE(year)  (LEAPYEAR(year) ? 366 : 365)
#define FIRSTSUNDAY(timp)       (((timp)->tm_yday - (timp)->tm_wday + 420) % 7)
#define FIRSTDAYOF(timp)        (((timp)->tm_wday - (timp)->tm_yday + 420) % 7)
#define TIME_MAX        4294967296L
#define LONG_MAX        2147483648L
#define ABB_LEN         3

extern const int _ytab[2][12];
extern const char *_days[];
extern const char *_months[];
extern int _mytimezone;

#endif

extern RTC_HandleTypeDef hrtc;

#ifndef NEW_FUNCTIONS
extern struct tm timep;
void gmtime(long timer);
unsigned long mymktime(struct tm *timep);
uint32_t  GetOra();
void SetOra(uint32_t epochtime);
#else
void SetOraNew(time_t epochtime);
time_t GetOraNew();
#endif

#endif
