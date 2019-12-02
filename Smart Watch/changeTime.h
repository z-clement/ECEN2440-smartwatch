/*
 * changeTime.h
 *
 *  Created on: Dec 2, 2019
 *      Author: johnc
 */

#ifndef CHANGETIME_H_
#define CHANGETIME_H_


#include "msp.h"
#include "uart.h"
#include "bluetooth.h"
#include "circularBuf.h"
#include "screenRefresh.h"
#include "rtc.h"
#include "clock.h"


void changeHour(uint8_t hour);
void changeMin(uint8_t min);


#endif /* CHANGETIME_H_ */
