#include "msp.h"
#include "uart.h"
#include "bluetooth.h"
#include "circularBuf.h"
#include "screenRefresh.h"
#include "rtc.h"
#include "clock.h"
#include "changeTime.h"

/**
 * main.c
 */

uint8_t bluetoothData = 0; // Variable for reading in bluetooth register
uint8_t screenData = 0;
volatile uint8_t RXFLAG = 0;
volatile uint8_t TXFLAG = 0;
volatile uint8_t sec = 0x00;
volatile uint8_t min = 0x00;
volatile uint8_t hour = 0x00;
volatile uint8_t day = 0x00;
volatile uint8_t month = 0x00;
volatile uint32_t year = 0x00;
volatile uint8_t dow = 0x00;
extern volatile uint8_t MINUTEFLAG;
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	//bluetooth testing
	EUSCI_A_Type * uart_port = EUSCI_A1;
	config_uart(uart_port);

	//screen testing
	EUSCI_A_Type * uart_portScreen = EUSCI_A2;
	config_uart(uart_portScreen);
	__enable_irq();

	// Test circular buffer sending hello
	circ_buf_t * helloBuffer = createBuffer(6);
	uint8_t * string = "Hello";
	addMultipleToBuffer(helloBuffer, string, 5);
	uart_transmit_buffer(helloBuffer, uart_port);
	deleteBuffer(helloBuffer);

	circ_buf_t * screenBuffer = createBuffer(18);
	uint8_t * stringBye = "ÿÿÿn0.val=4ÿÿÿ"; //contols the hour of the screen. n0.val is the hour object ÿÿÿ is the termination character
	addMultipleToBuffer(screenBuffer,stringBye, 16);
	uart_transmit_buffer(screenBuffer, uart_portScreen);
	deleteBuffer(screenBuffer);



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


    clock(); // configure the rtc clock and interupts
    clockUpdate();
    changeHour(uart_portScreen);
    changeMin(uart_portScreen);
    P2->DIR |= BIT0;
    P2->DIR |= BIT1;
    P2->OUT &= ~BIT0;
    P2->OUT &= ~BIT1;
    //P2->OUT |= BIT0;
   // MINUTEFLAG = 1;

    circ_buf_t * timeBuffer = createBuffer(20);
    uint8_t * stringBegin = "ÿÿÿn0.val=";                    // 22ÿÿÿ";
    addMultipleToBuffer(timeBuffer,stringBegin, 10);
    addToBuffer(timeBuffer, (min + 48));
    uint8_t * stringEnd = "ÿÿÿ";
    addMultipleToBuffer(timeBuffer, stringEnd, 3);
    uart_transmit_buffer(timeBuffer, uart_portScreen);
    deleteBuffer(timeBuffer);
    while(1){
        if (MINUTEFLAG != 0){
            clockUpdate();
            P2->OUT |= BIT0;
            changeHour(uart_portScreen);
            changeMin(uart_portScreen);
        }
        else{
            P2->OUT |= BIT1;
            //P2->OUT &= ~BIT0;
        }
    }

}
