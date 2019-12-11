
/*
 * bluetooth.h
 *
 *  Created on: Nov 17, 2019
 *      Author: Zac
 */

#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

#include "circularBuf.h"

// Decode the bluetooth transmission for RTC functionality
void decode_bluetooth(circ_buf_t * receiveBuffer);

// Set the RTC with the values in the buffer
void set_rtc_clock(circ_buf_t * valueBuffer);

// Set the alarm with the value in the buffer
void set_alarm(circ_buf_t * valueBuffer);

// Set the timer with the hours & minutes in the buffer
void set_timer(circ_buf_t * valueBuffer);

// Decodes the UTF-8 characters to uint8_t numbers, outputBuffer should have the same size (or larger) as inputBuffer
void decodeUTFTime(circ_buf_t * inputBuffer, circ_buf_t * outputBuffer);

// Check if two strings are equal
uint8_t isStringEqual(uint8_t * str1, uint8_t * str2, int length);

#endif /* BLUETOOTH_H_ */
