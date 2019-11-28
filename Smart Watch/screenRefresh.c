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

extern volatile uint8_t RXFLAG;
extern volatile uint8_t TXFLAG;
extern uint8_t screenData;


void EUSCIA2_IRQHandler() {
    // Rx interrupt
    if (EUSCI_A2->IFG & UCRXIFG) {
        RXFLAG = 1;
        EUSCI_A2->IFG &= ~UCRXIFG;
    // Tx interrupt
    } else if (EUSCI_A2->IFG & UCTXIFG) {
        // TODO: check if the transfer buffer is empty, if it is just turn off interrupt
        //       otherwise, call the transmit function on the data in the buffer.
        // For now this just turns off the interrupt flag, and turns on the internal flag so the
        // transmit function knows it's okay to transmit.
        TXFLAG = 1;
        EUSCI_A2->IFG &= ~UCTXIFG;
    }
}

void johndog() {
    if (RXFLAG == 1) {
        screenData = EUSCI_A2->RXBUF;
        RXFLAG = 0;
    }
}
