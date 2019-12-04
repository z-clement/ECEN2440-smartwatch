
/*
 * bluetooth.h
 *
 *  Created on: Nov 17, 2019
 *      Author: Zac
 */

#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

#include "circularBuf.h"

void decode_bluetooth(circ_buf_t * receiveBuffer);

void set_rtc_clock(circ_buf_t * valueBuffer);

void set_alarm(circ_buf_t * valueBuffer);

void set_timer(circ_buf_t * valueBuffer);

void decodeUTFTime(circ_buf_t * inputBuffer, circ_buf_t * outputBuffer);

uint8_t isStringEqual(uint8_t * str1, uint8_t * str2, int length);

#endif /* BLUETOOTH_H_ */
