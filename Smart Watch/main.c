#include "msp.h"
#include "uart.h"
#include "bluetooth.h"
#include "circularBuf.h"
#include "rtc.h"

/**
 * main.c
 */

// Flags for handling interrupts so we don't get stuck in an IRQHandler for too long
volatile uint8_t RDYFLAG = 0;
volatile uint8_t ALARMFLAG = 0;
volatile uint8_t TIMERFLAG = 0;
volatile uint8_t RX2FLAG = 0;
volatile uint8_t TX2FLAG = 0;
volatile uint8_t RX1FLAG = 0;
volatile uint8_t TX1FLAG = 0;

void main(void)
{
    // Stop watchdog timer.
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    __enable_irq();

    // Configure UART for bluetooth
    EUSCI_A_Type * bluetooth_port = EUSCI_A2;
    config_uart(bluetooth_port);

    // Configure output pins for RTC alarm/timer
    config_rtc_gpio();

    // Create a circular buffer for bluetooth receiving
    circ_buf_t * bluetoothBuffer = createBuffer(8);

    // Create a buffer to send a bluetooth message when an alarm fires
    circ_buf_t * alarmBuffer = createBuffer(6);

    // For debugging, a buffer to send the time over bluetooth
    circ_buf_t * timeBuffer = createBuffer(9);

    // For debugging, sends "Test!" over bluetooth
    circ_buf_t * testBuffer = createBuffer(6);
    addMultipleToBuffer(testBuffer, "Test!", 5);
    uart_transmit_buffer(testBuffer, bluetooth_port);
    deleteBuffer(testBuffer);

    while(1){

        // Read in from bluetooth
        if (RX2FLAG) {
            uart_read_to_buffer(bluetoothBuffer, bluetooth_port);
            if (isBufferFull(bluetoothBuffer)) {
                decode_bluetooth(bluetoothBuffer);
            }
        }

        // Alarm or timer goes off
        if (ALARMFLAG) {
           P2->OUT |= BIT4;
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
            addToBuffer(timeBuffer, RTCSEC);
            addToBuffer(timeBuffer, RTCMIN);
            addToBuffer(timeBuffer, RTCHOUR);
            addToBuffer(timeBuffer, RTCDAY);
            addToBuffer(timeBuffer, RTCMON);
            addToBuffer(timeBuffer, (RTCYEAR >> 8));
            addToBuffer(timeBuffer, RTCYEAR);
            uart_transmit_buffer(timeBuffer, bluetooth_port);
            RDYFLAG = 0;
        }
    }
}
