
#include "msp.h"
#include "uart.h"
#include "bluetooth.h"
#include "circularBuf.h"
#include "screenRefresh.h"
#include "rtc.h"
//#include "clock.h"
#include "changeTime.h"


/**
 * main.c
 */


uint8_t screenData = 0;
volatile uint8_t sec = 0x00;
volatile uint8_t min = 0x00;
volatile uint8_t hour = 0x00;
volatile uint8_t day = 0x00;
volatile uint8_t month = 0x00;
volatile uint32_t year = 0x00;
volatile uint8_t dow = 0x00;
volatile uint8_t MINUTEFLAG = 0;
volatile uint8_t RDYFLAG = 0;
volatile uint8_t ALARMFLAG = 0;
volatile uint8_t TIMERFLAG = 0;
volatile uint8_t RX2FLAG = 0;
volatile uint8_t TX2FLAG = 0;
volatile uint8_t RX1FLAG = 0;
volatile uint8_t TX1FLAG = 0;
volatile uint8_t CLOCKSETFLAG = 0;
volatile uint8_t ALARMSETFLAG = 0;
volatile uint8_t TIMERSETFLAG = 0;

EUSCI_A_Type * bluetooth_port = EUSCI_A1;

void main(void)
{

    //John's setup

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	//screen testing
	EUSCI_A_Type * uart_portScreen = EUSCI_A2;
	config_uart(uart_portScreen);
	__enable_irq();

    // Configure UART for bluetooth
    config_uart(bluetooth_port);

    // Configure output pins for RTC alarm/timer
    config_rtc_gpio();

    // Create a circular buffer for bluetooth receiving
    circ_buf_t * bluetoothBuffer = createBuffer(5);

    // Create a buffer to send a bluetooth message when an alarm fires
    circ_buf_t * alarmBuffer = createBuffer(6);

//    // For debugging, a buffer to send the time over bluetooth
//    circ_buf_t * timeBuffer = createBuffer(9);

    // For debugging, sends "Test!" over bluetooth
    circ_buf_t * testBuffer = createBuffer(6);
    addMultipleToBuffer(testBuffer, "Test!", 5);
    uart_transmit_buffer(testBuffer, bluetooth_port);
    deleteBuffer(testBuffer);

	circ_buf_t * screenBuffer = createBuffer(18);
	uint8_t * stringBye = "ÿÿÿn0.val=4ÿÿÿ"; //contols the hour of the screen. n0.val is the hour object ÿÿÿ is the termination character
	addMultipleToBuffer(screenBuffer,stringBye, 16);
	uart_transmit_buffer(screenBuffer, uart_portScreen);
	deleteBuffer(screenBuffer);

    const RTC_C_Calendar currentTime =
    {
         0x00,       // Seconds
         0x00,       // Minutes
         0x12,       // Hours
         0x00,       // Day of week (unused in our watch)
         0x00,       // Day of month
         0x00,       // Month
         0x0000      // Year
    };

    // Configure the RTC with the above time in BCD format
    RTC_config(&currentTime, RTC_C_CTL13_BCD);; // configure the rtc clock and interupts

    clockUpdate();
    changeHour(uart_portScreen);
    changeMin(uart_portScreen);
//    P2->DIR |= BIT0;
//    P2->DIR |= BIT1;
//    P2->OUT &= ~BIT0;
//    P2->OUT &= ~BIT1;
    //P2->OUT |= BIT0;
   // MINUTEFLAG = 1;

//    circ_buf_t * timeBuffer = createBuffer(20);
//    uint8_t * stringBegin = "ÿÿÿn0.val=";                    // 22ÿÿÿ";
//    addMultipleToBuffer(timeBuffer,stringBegin, 10);
//    addToBuffer(timeBuffer, (min + 48));
//    uint8_t * stringEnd = "ÿÿÿ";
//    addMultipleToBuffer(timeBuffer, stringEnd, 3);
//    uart_transmit_buffer(timeBuffer, uart_portScreen);
//    deleteBuffer(timeBuffer);

    while(1){
        // Read in from bluetooth
        if (RX1FLAG) {
            uart_read_to_buffer(bluetoothBuffer, bluetooth_port);
            if (isBufferFull(bluetoothBuffer)) {
                decode_bluetooth(bluetoothBuffer);
            }
        }
        // Alarm or timer goes off
        if (ALARMFLAG) {
           P4->OUT |= BIT7;
           ALARMFLAG = 0;

           // Check if it was a timer, if so disable alarms
           if (TIMERFLAG) {
               RTC_C->AMINHR = 0x0000;
               RTC_C->ADOWDAY = 0x0000;
               TIMERFLAG = 0;
               addMultipleToBuffer(alarmBuffer, "Timer", 5);
           } else {
               addMultipleToBuffer(alarmBuffer, "Alarm", 5);
           }

           // Send a message over bluetooth indciating the alarm/timer finished
           uart_transmit_buffer(alarmBuffer, bluetooth_port);
        }
        // Second ticked by, time must be updated
        if (RDYFLAG) {
            // For debugging, transmit current time as SEC_MIN_HOUR_DAY_MONTH_YEAR in one bluetooth transmission
            /*
            addToBuffer(timeBuffer, RTCSEC);
            addToBuffer(timeBuffer, RTCMIN);
            addToBuffer(timeBuffer, RTCHOUR);
            addToBuffer(timeBuffer, RTCDAY);
            addToBuffer(timeBuffer, RTCMON);
            addToBuffer(timeBuffer, (RTCYEAR >> 8));
            addToBuffer(timeBuffer, RTCYEAR);
            uart_transmit_buffer(timeBuffer, bluetooth_port);
            */
            clockUpdate();
//            P2->OUT |= BIT0;
            changeHour(uart_portScreen);
            changeMin(uart_portScreen);
            RDYFLAG = 0;
        }
        if (MINUTEFLAG != 0){
            clockUpdate();
//            P2->OUT |= BIT0;
            changeHour(uart_portScreen);
            changeMin(uart_portScreen);
        }

    }
}

