#include "msp.h"
#include "uart.h"
#include "bluetooth.h"
#include "circularBuf.h"

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
	config_uart(EUSCI_A2);

	__enable_irq();

	// Test circular buffer sending hello
	circ_buf_t * helloBuffer = createBuffer(6);
	uint8_t * string = "Hello";
	addMultipleToBuffer(helloBuffer, string, 5);
	uart_transmit_buffer(helloBuffer);
	deleteBuffer(helloBuffer);

	// Test that bluetooth is connected and transmit works
//	uart_transmit_byte((uint8_t) 1);

//	while(1) {
//	    read_register();
//	}

//	// Test for sending 1 byte of data (WORKS!)
//	int i;
//	for (i = 0; i < 255; i++) {
//	    uart_transmit_byte((uint8_t) i);
//	    // delay;
//	    int j;
//	    for (j = 0; j < 1000000; j++);
//	}
}
