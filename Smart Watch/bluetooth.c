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
#include "uart.h"
#include "rtc.h"

extern EUSCI_A_Type * bluetooth_port;
extern volatile RTC_C_Calendar * realTime;
extern volatile uint8_t TIMERFLAG;
extern volatile uint8_t RX2FLAG;

/**
 *  Decode the bluetooth transmission for RTC functionality
 *  All bluetooth transmissions must be 8 bytes long, and start with 0x00, 0x01, or 0x02 to determine functionality.
 *  The bytes following the first bit depend on the functionality needed, setting the clock requires all 8,
 *  setting an alarm requires only 6, and the timer requires only 2. If the functionality requires less than
 *  8 bytes, the end can be padded with anything, but all 8 bytes must be filled before the decoder will work.
 */
void decode_bluetooth(circ_buf_t * receiveBuffer) {
    // receiveBuffer holds a string with the command we want to perform
    uint8_t * command = receiveBuffer->buffer;

    if (isStringEqual(command, "Clock", 5)) {
        // Send info for how to set the clock over bluetooth
        circ_buf_t * commandBuff1 = createBuffer(15);
        addMultipleToBuffer(commandBuff1, "Set clock with:", 15);
        uart_transmit_buffer(commandBuff1, bluetooth_port);
        deleteBuffer(commandBuff1);

        circ_buf_t * commandBuff2 = createBuffer(19);
        addMultipleToBuffer(commandBuff2, "mm:hh:wd:dd:MM:yyyy", 19);
        uart_transmit_buffer(commandBuff2, bluetooth_port);
        deleteBuffer(commandBuff2);

        // Read in clock settings over bluetooth
        circ_buf_t * inputBuffer = createBuffer(19);
        while (!isBufferFull(inputBuffer)) {
            if (RX2FLAG) {
                uart_read_to_buffer(inputBuffer, bluetooth_port);
            }
        }

        set_rtc_clock(inputBuffer);
    } else if (isStringEqual(command, "Alarm", 5)) {
        // Send info for how to set the alarm over bluetooth
        circ_buf_t * commandBuff = createBuffer(64);
        addMultipleToBuffer(commandBuff, "Set alarm with en:mm:hh:wd:dd ", 30);
        uart_transmit_buffer(commandBuff, bluetooth_port);
        addMultipleToBuffer(commandBuff, "en is alarm enable ", 19);
        uart_transmit_buffer(commandBuff, bluetooth_port);
        addMultipleToBuffer(commandBuff, "1 = minute, 2 = hour, 3 = dow, 4 = day ", 39);
        uart_transmit_buffer(commandBuff, bluetooth_port);
        deleteBuffer(commandBuff);

        // Read in alarm settings over bluetooth
        circ_buf_t * inputBuffer = createBuffer(13);
        while (!isBufferFull(inputBuffer)) {
            if (RX2FLAG) {
                uart_read_to_buffer(inputBuffer, bluetooth_port);
            }
        }

        set_alarm(inputBuffer);
    } else if (isStringEqual(command, "Timer", 5)) {
        // Send info for how to set the timer over bluetooth
        circ_buf_t * commandBuff = createBuffer(20);
        addMultipleToBuffer(commandBuff, "Set timer with mm:hh", 20);
        uart_transmit_buffer(commandBuff, bluetooth_port);
        deleteBuffer(commandBuff);

        // Read in timer settings over bluetooth
        circ_buf_t * inputBuffer = createBuffer(5);
        while (!isBufferFull(inputBuffer)) {
            if (RX2FLAG) {
                uart_read_to_buffer(inputBuffer, bluetooth_port);
            }
        }

        set_timer(inputBuffer);
    }

    // Clear the buffer so nothing gets carried over for the next bluetooth read
    clearBuffer(receiveBuffer);
}

// Set the rtc with the values in the buffer
void set_rtc_clock(circ_buf_t * valueBuffer) {
    // Take the UTF-8 numbers and convert them to binary
    circ_buf_t * timeBuffer = createBuffer(16);
    decodeUTFTime(valueBuffer, timeBuffer);

    // Get the minutes to set the clock
    uint8_t minTens = readFromBuffer(timeBuffer);
    uint8_t minOnes = readFromBuffer(timeBuffer);
    uint8_t minutes = ((minTens << 4) | minOnes);
    // Get the hours to set the clock
    uint8_t hourTens = readFromBuffer(timeBuffer);
    uint8_t hourOnes = readFromBuffer(timeBuffer);
    uint8_t hours = ((hourTens << 4) | hourOnes);
    // Get the day of week to set the clock
    uint8_t dowTens = readFromBuffer(timeBuffer);
    uint8_t dowOnes = readFromBuffer(timeBuffer);
    uint8_t dow = ((dowTens << 4) | dowOnes);
    // Get the day to set the clock
    uint8_t dayTens = readFromBuffer(timeBuffer);
    uint8_t dayOnes = readFromBuffer(timeBuffer);
    uint8_t day = ((dayTens << 4) | dayOnes);
    // Get the month to set the clock
    uint8_t monthTens = readFromBuffer(timeBuffer);
    uint8_t monthOnes = readFromBuffer(timeBuffer);
    uint8_t month = ((monthTens << 4) | monthOnes);
    // Get the year to set the clock
    uint8_t yearDigit1 = readFromBuffer(timeBuffer);
    uint8_t yearDigit2 = readFromBuffer(timeBuffer);
    uint8_t yearDigit3 = readFromBuffer(timeBuffer);
    uint8_t yearDigit4 = readFromBuffer(timeBuffer);
    uint16_t year = ((yearDigit1 << 12) | (yearDigit2 << 8) | (yearDigit3 << 4) | yearDigit4);

    const RTC_C_Calendar currentTime =
    {
         0x00,       // Seconds
         minutes,    // Minutes
         hours,      // Hours
         dow,        // Day of week (unused in our watch)
         day,        // Day of month
         month,      // Month
         year        // Year
    };

    // Configure the RTC with the above time in BCD format
    RTC_config(&currentTime, RTC_C_CTL13_BCD);
}

// Set the alarm with the value in the buffer
void set_alarm(circ_buf_t * valueBuffer) {
    // alarmEnables: 1 = minute, 2 = hour, 3 = dow, 4 = day, any combo works to enable multiple (add numbers)
    // Take the UTF-8 numbers and convert them to binary
    circ_buf_t * timeBuffer = createBuffer(9);
    decodeUTFTime(valueBuffer, timeBuffer);

    // Get the alarm enable information
    uint8_t alarmEnables = readFromBuffer(timeBuffer);
    // Get the minutes to set the alarm
    uint8_t minTens = readFromBuffer(timeBuffer);
    uint8_t minOnes = readFromBuffer(timeBuffer);
    uint8_t minVal = ((minTens << 4) | minOnes);
    // Get the hours to set the alarm
    uint8_t hourTens = readFromBuffer(timeBuffer);
    uint8_t hourOnes = readFromBuffer(timeBuffer);
    uint8_t hourVal = ((hourTens << 4) | hourOnes);
    // Get the day of week to set the alarm
    uint8_t dowTens = readFromBuffer(timeBuffer);
    uint8_t dowOnes = readFromBuffer(timeBuffer);
    uint8_t dowVal = ((dowTens << 4) | dowOnes);
    // Get the day to set the alarm
    uint8_t dayTens = readFromBuffer(timeBuffer);
    uint8_t dayOnes = readFromBuffer(timeBuffer);
    uint8_t dayVal = ((dayTens << 4) | dayOnes);

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
}

// Set the timer with the hours & minutes in the buffer
void set_timer(circ_buf_t * valueBuffer) {
    circ_buf_t * timeBuffer = createBuffer(4);
    decodeUTFTime(valueBuffer, timeBuffer);

    // Get the minutes in BCD to set the timer
    uint8_t minTens = readFromBuffer(timeBuffer);
    uint8_t minOnes = readFromBuffer(timeBuffer);
    uint8_t minBCD = ((minTens << 4) | minOnes);
    // Get the hours in BCD to set the timer
    uint8_t hourTens = readFromBuffer(timeBuffer);
    uint8_t hourOnes = readFromBuffer(timeBuffer);
    uint8_t hourBCD = ((hourTens << 4) | hourOnes);

    // Convert input and current time to binary for easier addition
    RTC_C->BCD2BIN = minBCD;
    uint8_t minBin = RTC_C->BCD2BIN;
    RTC_C->BCD2BIN = hourBCD;
    uint8_t hourBin = RTC_C->BCD2BIN;

    RTC_C->BCD2BIN = RTCMIN;
    uint8_t currentMinBin = RTC_C->BCD2BIN;
    RTC_C->BCD2BIN = RTCHOUR;
    uint8_t currentHourBin = RTC_C->BCD2BIN;

    // Add current time and input timer values for time to set alarm
    uint8_t min = minBin + currentMinBin;
    uint8_t hours = hourBin + currentHourBin;

    // Check for current time + minutes to roll over if min is over 60
    if (min >= 60) {
        // Temp variable so min can change and hours can change
        uint8_t tempMin = min;
        min = tempMin % 60;
        hours += tempMin / 60;
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

// Decodes the UTF-8 characters to uint8_t numbers, outputBuffer should have the same size (or larger) as inputBuffer
void decodeUTFTime(circ_buf_t * inputBuffer, circ_buf_t * outputBuffer) {
    int i;
    for (i = 0; i < inputBuffer->size; i++) {
        uint8_t utfNumber = readFromBuffer(inputBuffer);
        // Throw out colons (':')
        if (utfNumber != 0x3A) {
            // Subtract 30 from a UTF-8 number to get the integer value of it (0-9 corresponds to 30-39)
            addToBuffer(outputBuffer, (utfNumber - 48));
        }
    }
}

// Check if two strings are equal
uint8_t isStringEqual(uint8_t * str1, uint8_t * str2, int length) {
    int i;
    for (i=0; i<length; i++) {
        if (str1[i] != str2[i]) {
            return 0;
        }
    }
    return 1;
}
