/*
 * screenRefresh.c
 *
 *  Created on: Nov 27, 2019
 *      Author: John
 *      Heavily based on Zac's bluethooth.c
 */

#include "screenRefresh.h"
#include "msp.h"
#include <stdint.h>

extern volatile uint8_t RX2FLAG;
//extern volatile uint8_t TXFLAG;
extern uint8_t screenData;


void screenReadRegister() {
    if (RX2FLAG == 1) {
        screenData = EUSCI_A2->RXBUF;
        RX2FLAG = 0;
    }
}
