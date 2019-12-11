/*
 * screenRefresh.c
 *
 *  Created on: Nov 27, 2019
 *      Author: John
 *      Heavily based on Zac's bluethooth.c
 */

#include "screenRefresh.h"
#include "alarmButton.h"
#include "msp.h"
#include <stdint.h>

extern volatile uint8_t RX2FLAG;
extern uint8_t screenData;

// Decode the screen UART transmission, currently is just used to turn off the alarm
void decode_screen(EUSCI_A_Type * screenUart) {
    if (screenData == 0x01) {
        alarmOff(screenUart);
        gotoHome(screenUart);
    }
}

// Read in the screen UART data
void screenReadRegister() {
    if (RX2FLAG == 1) {
        screenData = EUSCI_A2->RXBUF;
        RX2FLAG = 0;
    }
}
