/*
 * bluetooth.c
 *
 *  Created on: Nov 17, 2019
 *      Author: Zac
 */

#include "bluetooth.h"
#include "msp.h"
#include <stdint.h>

extern volatile uint8_t RXFLAG;
extern volatile uint8_t TXFLAG;
extern uint8_t bluetoothData;

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

void read_register() {
    if (RXFLAG == 1) {
        bluetoothData = EUSCI_A2->RXBUF;
        RXFLAG = 0;
    }
}
