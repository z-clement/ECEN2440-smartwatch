/*
 * alarmButton.h
 *
 *  Created on: Dec 4, 2019
 *      Author: johnc
 */

#ifndef ALARMBUTTON_H_
#define ALARMBUTTON_H_

#include "msp.h"
#include "uart.h"
#include "bluetooth.h"
#include "circularBuf.h"
#include "screenRefresh.h"
#include "rtc.h"
#include "pwm.h"
//#include "clock.h"
#include "changeTime.h"


void gotoAlarmButton(EUSCI_A_Type * uartportScreen);
void gotoHome(EUSCI_A_Type * uartportScreen);
void alarmOff(EUSCI_A_Type * uartportScreen);


#endif /* ALARMBUTTON_H_ */
