
/*
 * uart.c
 *
 *  Created on: Nov 10, 2019
 *      Author: Zac
 */

#include "msp.h"
#include <stdint.h>
#include "uart.h"
#include "circularBuf.h"

extern volatile uint8_t RX1FLAG;
extern volatile uint8_t TX1FLAG;
extern volatile uint8_t RX2FLAG;
extern volatile uint8_t TX2FLAG;
extern volatile circ_buf_t * receiveBuffer;


// Configure the EUSC_Ax module to operate in UART mode
// uart_port = EUSCI_A2 or EUSCI_A1
void config_uart(EUSCI_A_Type * uart_port) {
    // Set UCSWRST = 1 in CTLW0
    uart_port->CTLW0 |= EUSCI_A_CTLW0_SWRST;

    // Configure ports (Px.2, Px.3)
    config_uart_ports(uart_port);

    // Configure clock to run at 12 MHz
    config_clock();

    // Set baud rate & any other settings
    // Since the division factor is over 16, enable oversampling mode
    // From table 24-5 for 12 Mhz clock, UCBRx = 78, UCBRFx = 2, UCBRSx = 0x0
    //EUSCI_A2->MCTLW |= (0x4E00 | 0x0020 | EUSCI_A_MCTLW_OS16);
    uart_port->MCTLW |= (0x0000 | 0x0020 | EUSCI_A_MCTLW_OS16);
    uart_port->BRW |= 78;
    uart_port->CTLW0 |= (EUSCI_A_CTLW0_SSEL__SMCLK);    // select SMCLK as BRCLK, and enable parity bit

    // Clear UCSWRST to turn on
    uart_port->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;

    // Enable interrupts
    uart_port->IE |= (EUSCI_A_IE_RXIE| EUSCI_A_IE_TXIE); // set rx and tx interrupts
    if (uart_port == EUSCI_A2) {
        NVIC_EnableIRQ(EUSCIA2_IRQn);
        NVIC_SetPriority(EUSCIA2_IRQn, 1);            // Set priority of the EUSCI_A2 interrupt to 1
    } else if (uart_port == EUSCI_A1) {
        NVIC_EnableIRQ(EUSCIA1_IRQn);
        NVIC_SetPriority(EUSCIA1_IRQn, 1);
    }
}

// Configure the Tx and Rx pins for the given uart module
void config_uart_ports(EUSCI_A_Type * uart_port) {
    /* Tx is on Px.3, with primary module selected (SEL[1:0] = 01)
       Rx is on Px.2, with primary module selected
       EUSCI_A2 means ports 3.3 & 3.2, A1 = 2.3 & 2.2*/
    if (uart_port == EUSCI_A2) {
        P3->SEL0 |= (BIT2 | BIT3);
        P3->SEL1 &= ~(BIT2 | BIT3);
    } else if (uart_port == EUSCI_A1) {
        P2->SEL0 |= (BIT2 | BIT3);
        P2->SEL1 &= ~(BIT2 | BIT3);
    }
}

// Set the SMCLK to run at 12 MHz for the uart baud rate
void config_clock(void) {
    CS->KEY = 0x695A;                    // Unlock CS module for register access
    CS->CTL0 = 0;                        // Reset tuning parameters
    CS->CTL0 = (CS_CTL0_DCOEN | CS_CTL0_DCORSEL_4);  // Turn on DCO and set it to 24MHz
    CS->CTL1 |= CS_CTL1_SELS__DCOCLK;    // Select DCO as source for SMCLK
    CS->CTL1 |= CS_CTL1_DIVS__2;         // Divide the SMCLK source by 2 to get 12 MHz
    CS->KEY = 0;                         // Lock CS module from unintended access
}

// Transmit the 1 byte of data over uart
void uart_transmit_byte(uint8_t data, EUSCI_A_Type * uart_port) {
    // Wait for transmit buffer to be empty
    if (uart_port == EUSCI_A2) {
        while (TX2FLAG != 1);
    } else {
        while (TX1FLAG != 1);
    }

    uart_port->TXBUF = data; // send the data being transmitted
    if (uart_port == EUSCI_A2) {
        TX2FLAG = 0;
    } else {
        TX1FLAG = 0;
    }
}

// Transmit the entire contents of the circular buffer over uart
void uart_transmit_buffer(circ_buf_t * circBuffer, EUSCI_A_Type * uart_port) {
    int i;
    uint8_t sizeOfBuffer = circBuffer->numElements;
    for (i = 0; i < sizeOfBuffer; i++) {
        uint8_t character = readFromBuffer(circBuffer);
        uart_transmit_byte(character, uart_port);
    }
}

// Read in from the Rx line to the given circular buffer
void uart_read_to_buffer(circ_buf_t * circBuffer, EUSCI_A_Type * uart_port) {
    addToBuffer(circBuffer, uart_port->RXBUF);
    if (uart_port == EUSCI_A2) {
        RX2FLAG = 0;
    } else {
        RX1FLAG  = 0;
    }
}

// Interrupt handler for bluetooth uart
void EUSCIA2_IRQHandler() {
    // Rx interrupt
    if (EUSCI_A2->IFG & UCRXIFG) {
        RX2FLAG = 1;
        EUSCI_A2->IFG &= ~UCRXIFG;
    // Tx interrupt
    } else if (EUSCI_A2->IFG & UCTXIFG) {
        // Turn off the interrupt flag, and turns on the internal flag so the transmit function knows it's okay to transmit.
        TX2FLAG = 1;
        EUSCI_A2->IFG &= ~UCTXIFG;
    }
}

// Interrupt handler for screen uart
void EUSCIA1_IRQHandler() {
    // Rx interrupt
    if (EUSCI_A1->IFG & UCRXIFG) {
        RX1FLAG = 1;
        EUSCI_A1->IFG &= ~UCRXIFG;
    // Tx interrupt
    } else if (EUSCI_A1->IFG & UCTXIFG) {
        // Turn off the interrupt flag, and turns on the internal flag so the transmit function knows it's okay to transmit.
        TX1FLAG = 1;
        EUSCI_A1->IFG &= ~UCTXIFG;
    }
}
