/*
 * gestionertc.c
 *
 *  Created on: 03 set 2018
 *      Author: fancyble
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32l0xx_hal.h"
#include "gestionertc.h"

#ifndef NEW_FUNCTIONS
int32_t OraAttuale;
int _mytimezone = 0;

const int _ytab[2][12] = {
	{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
	{ 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
};

struct tm timep;

/* Input long ora espressa come epochtimer */
/* output carica i dati nella struttura struct tm. vedi gestionertc.h */
void gmtime(long timer) {
	static struct tm br_time;
	long time = timer;
	unsigned long dayclock, dayno;
	int year = EPOCH_YR;

	dayclock = (unsigned long)time % SECS_DAY;
	dayno = (unsigned long)time / SECS_DAY;

	timep.tm_sec = dayclock % 60;
	timep.tm_min = (dayclock % 3600) / 60;
	timep.tm_hour = dayclock / 3600;
	timep.tm_wday = (dayno + 4) % 7;       /* day 0 was a thursday */
	while (dayno >= YEARSIZE(year)) {
		dayno -= YEARSIZE(year);
		year++;
	}
	timep.tm_year = year - YEAR0;
	timep.tm_yday = dayno;
	timep.tm_mon = 0;
	while (dayno >= _ytab[LEAPYEAR(year)][timep.tm_mon]) {
		dayno -= _ytab[LEAPYEAR(year)][timep.tm_mon];
		timep.tm_mon++;
	}
	timep.tm_mday = dayno + 1;
	timep.tm_isdst = 0;

}

/* Input struttura dati struct tm */
/* Output long espresso in epochtimer  */
unsigned long
mymktime(struct tm *timep)
{
        long day, year;
        int tm_year;
        int yday, month;
        unsigned long seconds;
        int overflow;

        timep->tm_min += timep->tm_sec / 60;
        timep->tm_sec %= 60;
        if (timep->tm_sec < 0) {
                timep->tm_sec += 60;
                timep->tm_min--;
        }
        timep->tm_hour += timep->tm_min / 60;
        timep->tm_min = timep->tm_min % 60;
        if (timep->tm_min < 0) {
                timep->tm_min += 60;
                timep->tm_hour--;
        }
        day = timep->tm_hour / 24;
        timep->tm_hour= timep->tm_hour % 24;
        if (timep->tm_hour < 0) {
                timep->tm_hour += 24;
                day--;
        }
        timep->tm_year += timep->tm_mon / 12;
        timep->tm_mon %= 12;
        if (timep->tm_mon < 0) {
                timep->tm_mon += 12;
                timep->tm_year--;
        }
        day += (timep->tm_mday - 1);
        while (day < 0) {
                if(--timep->tm_mon < 0) {
                        timep->tm_year--;
                        timep->tm_mon = 11;
                }
                day += _ytab[LEAPYEAR((YEAR0 + timep->tm_year))][timep->tm_mon];
        }
        while (day >= _ytab[LEAPYEAR(YEAR0 + timep->tm_year)][timep->tm_mon]) {
                day -= _ytab[LEAPYEAR(YEAR0 + timep->tm_year)][timep->tm_mon];
                if (++(timep->tm_mon) == 12) {
                        timep->tm_mon = 0;
                        timep->tm_year++;
                }
        }
        timep->tm_mday = day + 1;
//        _tzset();                       /* set timezone and dst info  */
        year = EPOCH_YR;
        if (timep->tm_year < (year - YEAR0)) return -1;
        seconds = 0;
        day = 0;                        /* means days since day 0 now */
        overflow = 0;

        /* Assume that when day becomes negative, there will certainly
         * be overflow on seconds.
         * The check for overflow needs not to be done for leapyears
         * divisible by 400.
         * The code only works when year (1970) is not a leapyear.
         */
#if     EPOCH_YR != 1970
#error  EPOCH_YR != 1970
#endif
        tm_year = timep->tm_year + YEAR0;

        if (LONG_MAX / 365 < tm_year - year) overflow++;
        day = (tm_year - year) * 365;
        if (LONG_MAX - day < (tm_year - year) / 4 + 1) overflow++;
        day += (tm_year - year) / 4
                + ((tm_year % 4) && tm_year % 4 < year % 4);
        day -= (tm_year - year) / 100
                + ((tm_year % 100) && tm_year % 100 < year % 100);
        day += (tm_year - year) / 400
                + ((tm_year % 400) && tm_year % 400 < year % 400);

        yday = month = 0;
        while (month < timep->tm_mon) {
                yday += _ytab[LEAPYEAR(tm_year)][month];
                month++;
        }
        yday += (timep->tm_mday - 1);
        if (day + yday < 0) overflow++;
        day += yday;

        timep->tm_yday = yday;
        timep->tm_wday = (day + 4) % 7;         /* day 0 was thursday (4) */

        seconds = ((timep->tm_hour * 60L) + timep->tm_min) * 60L + timep->tm_sec;

        if ((TIME_MAX - seconds) / SECS_DAY < day) overflow++;
        seconds += day * SECS_DAY;

        /* Now adjust according to timezone and daylight saving time */

        if (((_mytimezone > 0) && (TIME_MAX - _mytimezone < seconds))
            || ((_mytimezone < 0) && (seconds < -_mytimezone)))
                overflow++;
        seconds += _mytimezone;
#ifdef notdef
        if (timep->tm_isdst < 0)
                dst = _dstget(timep);
        else if (timep->tm_isdst)
                dst = _dst_off;
        else dst = 0;

        if (dst > seconds) overflow++;  /* dst is always non-negative */
        seconds -= dst;

        if (overflow) return (time_t)-1;

        if ((time_t)seconds != seconds) return (time_t)-1;
#endif
		return seconds;
}


/* Legge da RTC del stm32 e torna l'ora espressa in epochtimer */

uint32_t  GetOra() {
    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;
    struct tm tm;
    uint32_t epochtime;

    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
/*
    if (sTime.TimeFormat == RTC_HOURFORMAT12_PM)
        tm.tm_hour = sTime.Hours +12;
    else */
    tm.tm_hour = sTime.Hours;

    tm.tm_min = sTime.Minutes;
    tm.tm_sec = sTime.Seconds;

    tm.tm_year  = sDate.Year +100;
    tm.tm_mon = sDate.Month -1;
    tm.tm_mday = sDate.Date;
    tm.tm_wday = sDate.WeekDay;

    epochtime = mymktime(&tm);
    return epochtime;

}

/* Input ora espressa in epochtimer e scrive su RTC */
void SetOra(uint32_t epochtime) {
    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;

    gmtime(epochtime);

/*    if (timep.tm_hour >= 12) {
        sTime.Hours =   timep.tm_hour -12;
        sTime.TimeFormat = RTC_HOURFORMAT12_PM;
    } else {
        sTime.Hours =   timep.tm_hour;
        sTime.TimeFormat = RTC_HOURFORMAT12_AM;
    }*/
    sTime.Hours =   timep.tm_hour;
    sTime.Minutes = timep.tm_min;
    sTime.Seconds = timep.tm_sec;
    sTime.SecondFraction = 0;
    sTime.SubSeconds = 0;
    if (timep.tm_year  > 100)  {
        sDate.Year =  timep.tm_year  - 100; // timep.tm_year + 1900; oltre il 2000.
    } else {
        sDate.Year =  timep.tm_year;
    }
    sDate.Month = timep.tm_mon + 1;
    sDate.Date = timep.tm_mday;
    sDate.WeekDay = timep.tm_wday;

    HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

}

#else

void SetOraNew(time_t epochtime){

	 RTC_TimeTypeDef sTime;
	 RTC_DateTypeDef sDate;

	 memset(&sTime,0,sizeof(RTC_TimeTypeDef));
	 memset(&sDate,0,sizeof(RTC_DateTypeDef));


	 struct tm time_tm;
	 time_tm = *(localtime(&epochtime));

	 sTime.Hours = (uint8_t)time_tm.tm_hour;
	 sTime.Minutes = (uint8_t)time_tm.tm_min;
	 sTime.Seconds = (uint8_t)time_tm.tm_sec;
	 if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
	 {
		 Error_Handler();
	 }

	 if (time_tm.tm_wday == 0) { time_tm.tm_wday = 7; } // the chip goes mon tue wed thu fri sat sun
	 sDate.WeekDay = (uint8_t)time_tm.tm_wday;
	 sDate.Month = (uint8_t)time_tm.tm_mon+1; //month 1- This is why date math is frustrating.
	 sDate.Date = (uint8_t)time_tm.tm_mday;
	 sDate.Year = (uint16_t)(time_tm.tm_year+1900-2000); // time.h is years since 1900, chip is years since 2000

	 /*
	 * update the RTC
	 */
	 if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
	 {
		 Error_Handler();
	 }

	 HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR0,0x32F2); // lock it in with the backup registers

	 /*
	 uint8_t buffer[128];
	 memset(buffer,0,128);


	 sprintf(buffer,"---- Dump Set Time (%u) ----\nYear = %u, Month = %u, Date = %u [%u:%u:%u]\n",(uint32_t)epochtime,
			 sDate.Year,sDate.Month,sDate.Date,sTime.Hours,sTime.Minutes,sTime.Seconds);

	 //sprintf(buffer,"test\n");
	HAL_UART_Transmit(&huart2, buffer, strnlen(buffer,128), HAL_MAX_DELAY);*/


}

time_t GetOraNew(){

	RTC_DateTypeDef rtcDate;
	RTC_TimeTypeDef rtcTime;

	memset(&rtcTime,0,sizeof(RTC_TimeTypeDef));
	memset(&rtcDate,0,sizeof(RTC_DateTypeDef));


	HAL_RTC_GetTime(&hrtc, &rtcTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &rtcDate, RTC_FORMAT_BIN);
	uint8_t hh = rtcTime.Hours;
	uint8_t mm = rtcTime.Minutes;
	uint8_t ss = rtcTime.Seconds;
	uint8_t d = rtcDate.Date;
	uint8_t m = rtcDate.Month;
	uint16_t y = rtcDate.Year;
	uint16_t yr = (uint16_t)(y+2000-1900);
	time_t currentTime = {0};
	struct tm tim = {0};
	tim.tm_year = yr;
	tim.tm_mon = m - 1;
	tim.tm_mday = d;
	tim.tm_hour = hh;
	tim.tm_min = mm;
	tim.tm_sec = ss;
	currentTime = mktime(&tim);

	/*
	uint8_t buffer[128];
	memset(buffer,0,128);


	sprintf(buffer,"---- Dump Get Time (%u) ----\nYear = %u, Month = %u, Date = %u\n",(uint32_t)currentTime,
			rtcDate.Year,rtcDate.Month,rtcDate.Date);

	HAL_UART_Transmit(&huart2, buffer, strnlen(buffer,128), HAL_MAX_DELAY);

	memset(buffer,0,128);
	sprintf("[%u:%u:%u]\n",rtcTime.Hours,rtcTime.Minutes,rtcTime.Seconds);

	HAL_UART_Transmit(&huart2, buffer, strnlen(buffer,128), HAL_MAX_DELAY);

	memset(buffer,0,128);
	sprintf("[%d-%d-%d %d:%d:%d]\n",tim.tm_year, tim.tm_mon, tim.tm_mday, tim.tm_hour, tim.tm_min, tim.tm_sec);

	//sprintf(buffer,"test get\n");
	HAL_UART_Transmit(&huart2, buffer, strnlen(buffer,128), HAL_MAX_DELAY);*/


	return currentTime;
}

#endif


