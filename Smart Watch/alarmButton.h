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
#include "changeTime.h"

// Send a command to the screen to switch to the alarm off screen
void gotoAlarmButton(EUSCI_A_Type * uartportScreen);

// Send a command to the screen to switch to the clock screen
void gotoHome(EUSCI_A_Type * uartportScreen);

// Turn off the drv and buzzer
void alarmOff(EUSCI_A_Type * uartportScreen);

#endif /* ALARMBUTTON_H_ */
