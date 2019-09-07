#ifndef _RTC_DEFINED
#define _RTC_DEFINED

#include "recorder_inc.h"
#include "hal_com.h"

#define _RTC_TDIF	+2.0	/* JST = UTC+2.0 */
/* Bit definitions */
#define	_BV(bit) (1<<(bit))




int rtc_initialize (void);				/* Initialize RTC */
int rtc_gettime (RTCTIME* rtc);			/* Get time */
int rtc_settime (const RTCTIME* rtc);	/* Set time */
int rtc_getutc (uint32_t* utc);			/* Get time in UTC */
int rtc_setutc (uint32_t utc);			/* Set time in UTC */

#endif
