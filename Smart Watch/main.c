#include "msp.h"
#include "uart.h"
#include "bluetooth.h"
//#include <stdio.h>

/**
 * main.c
 */

uint8_t bluetoothData = 0; // Variable for reading in bluetooth register
volatile uint8_t RXFLAG = 0;
volatile uint8_t TXFLAG = 0;
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	//bluetooth testing
	config_uart();

	__enable_irq();

	// Test that bluetooth is connected and transmit works
//	uart_transmit_byte((uint8_t) 1);

	while(1) {
	    read_register();
//        printf("Value received on rx pin: %x\n", bluetoothRx);
	}

	// Test for sending 1 byte of data (WORKS!)
//	int i;
//	for (i = 0; i < 255; i++) {
//	    uart_transmit_byte((uint8_t) i);
//	    // delay;
//	    int j;
//	    for (j = 0; j < 1000000; j++);
//	}
}
