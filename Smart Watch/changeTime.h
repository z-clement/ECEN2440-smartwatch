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


// Change the hour on the screen
void changeHour(EUSCI_A_Type * uartportScreen);

// Change the minute display on the screen
void changeMin(EUSCI_A_Type * uartportScreen);

// Change the day of week on the screen
void changeDow(EUSCI_A_Type * uartportScreen);

// Change the day display on the screen
void changeDay(EUSCI_A_Type * uartportScreen);

// Change the month on the display
void changeMonth(EUSCI_A_Type * uartportScreen);

// Change the year on the display
void changeYear(EUSCI_A_Type * uartportScreen);

// Change all the time on the display
void changeAllTime(EUSCI_A_Type * uartportScreen);


#endif /* CHANGETIME_H_ */
