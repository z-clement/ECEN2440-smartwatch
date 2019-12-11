
/*
 * rtc.h
 *
 *  Created on: Nov 20, 2019
 *      Author: David
 */

#ifndef RTC_H_
#define RTC_H_

#include <stdint.h>

typedef struct _RTC_C_Calendar
{
    uint_fast8_t seconds;
    uint_fast8_t minutes;
    uint_fast8_t hours;
    uint_fast8_t dayOfWeek;
    uint_fast8_t dayOfmonth;
    uint_fast8_t month;
    uint_fast16_t year;
} RTC_C_Calendar;

// Provides the intial configuration for the RTC, with a current time
// and a format, BCD or hexadecimal.
void RTC_config(const RTC_C_Calendar *calendarTime, uint_fast16_t formatSelect);

void RTC_C_clearInterruptFlag(uint_fast8_t interruptFlagMask);

void RTC_C_enableInterrupt(uint8_t interruptMask);

void RTC_C_startClock(void);

void config_rtc_gpio(void);

void clockUpdate(void);

#endif /* RTC_H_ */

