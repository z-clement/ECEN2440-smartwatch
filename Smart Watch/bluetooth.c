/*
 * bluetooth.c
 *
 *  Created on: Nov 17, 2019
 *      Author: Zac
 */

#include <stdint.h>
#include "msp.h"
#include "bluetooth.h"
#include "circularBuf.h"
#include "rtc.h"

extern volatile RTC_C_Calendar * realTime;
extern volatile uint8_t TIMERFLAG;

/**
 *  Decode the bluetooth transmission for RTC functionality
 *  All bluetooth transmissions must be 8 bytes long, and start with 0x00, 0x01, or 0x02 to determine functionality.
 *  The bytes following the first bit depend on the functionality needed, setting the clock requires all 8,
 *  setting an alarm requires only 6, and the timer requires only 2. If the functionality requires less than
 *  8 bytes, the end can be padded with anything, but all 8 bytes must be filled before the decoder will work.
 */
void decode_bluetooth(circ_buf_t * receiveBuffer) {
    // First value sent over bluetooth should be for determining what functionality to use
    uint8_t firstVal = readFromBuffer(receiveBuffer);
    if (firstVal == 0) {
        // Means set the clock with remaining values in buffer
        uint8_t minutes = readFromBuffer(receiveBuffer);
        uint8_t hours = readFromBuffer(receiveBuffer);
        uint8_t dayWeek = readFromBuffer(receiveBuffer);
        uint8_t dayMonth = readFromBuffer(receiveBuffer);
        uint8_t month = readFromBuffer(receiveBuffer);
        uint8_t yearFirstTwoDigits = readFromBuffer(receiveBuffer);
        uint8_t yearSecondTwoDigits = readFromBuffer(receiveBuffer);
        uint16_t year = (yearFirstTwoDigits << 8) | yearSecondTwoDigits;

        const RTC_C_Calendar currentTime =
        {
             0x00,       // Seconds
             minutes,    // Minutes
             hours,      // Hours
             dayWeek,    // Day of week
             dayMonth,   // Day of month
             month,      // Month
             year       // Year
        };

        // Configure the RTC with the above time in BCD format
        RTC_config(&currentTime, RTC_C_CTL13_BCD);

    } else if (firstVal == 1) {
        // Means set min/hour alarm with value from buffer, second buffer value says which alarms to enable.
        // alarmEnables: 0x01 = minute, 0x02 = hour, 0x03 = dow, 0x04 = day, any combo works to enable multiple
        uint8_t alarmEnables = readFromBuffer(receiveBuffer);
        uint8_t minVal = readFromBuffer(receiveBuffer);
        uint8_t hourVal = readFromBuffer(receiveBuffer);
        uint8_t dowVal = readFromBuffer(receiveBuffer);
        uint8_t dayVal = readFromBuffer(receiveBuffer);

        // Clear all alarm registers
        RTC_C->AMINHR = 0x0000;
        RTC_C->ADOWDAY = 0x0000;

        // Enable the alarms that have a nonzero value passed in over bluetooth
        if (alarmEnables & 1) {
            RTC_C->AMINHR = RTC_C_AMINHR_MINAE;
        }
        if (alarmEnables & (1<<1)) {
            RTC_C->AMINHR |= RTC_C_AMINHR_HOURAE;
        }
        if (alarmEnables & (1<<2)) {
            RTC_C->ADOWDAY |= RTC_C_ADOWDAY_DOWAE;
        }
        if (alarmEnables & (1<<3)) {
            RTC_C->ADOWDAY |= RTC_C_ADOWDAY_DAYAE;
        }
        RTC_C->AMINHR |= minVal;            // Set minutes
        RTC_C->AMINHR |= (hourVal << 8);    // Set hours
        RTC_C->ADOWDAY |= dowVal;           // Set day of week
        RTC_C->ADOWDAY |= (dayVal << 8);    // Set day of month

    } else if (firstVal == 2) {
        // Means set timer with value from buffer, gets a value in minutes and uses the alarm module to set it
        uint8_t timerValue = readFromBuffer(receiveBuffer);
        // Get current hours & minutes in binary, and add the timer value
        RTC_C->BCD2BIN = RTCHOUR;
        uint8_t hours = (RTC_C->BCD2BIN) + (timerValue / 60);
        RTC_C->BCD2BIN = RTCMIN;
        uint8_t min = (RTC_C->BCD2BIN) + (timerValue % 60);

        // Check for current time + minutes to roll over
        if (min > 59) {
            min %= 60;
            hours += min / 60;
        }

        // Clear alarm registers
        RTC_C->AMINHR = 0x0000;
        RTC_C->ADOWDAY = 0x0000;

        // Enable minute and hour alarms;
        RTC_C->AMINHR |= (RTC_C_AMINHR_MINAE | RTC_C_AMINHR_HOURAE);

        // Set minute alarm
        RTC_C->BIN2BCD = min;
        RTC_C->AMINHR |= (RTC_C_AMINHR_MIN_MASK & RTC_C->BIN2BCD);
        // Set hours;
        RTC_C->BIN2BCD = hours;
        RTC_C->AMINHR |= (RTC_C->BIN2BCD << 8);
        // Set flag so timer isn't repeated like an alarm
        TIMERFLAG = 1;
    }

    // Clear the buffer so nothing gets carried over for the next bluetooth read
    clearBuffer(receiveBuffer);
}
