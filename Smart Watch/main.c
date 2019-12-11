
#include "msp.h"
#include "uart.h"
#include "bluetooth.h"
#include "circularBuf.h"
#include "screenRefresh.h"
#include "rtc.h"
#include "changeTime.h"
#include "alarmButton.h"
#include "alarm.h"


/**
 * main.c
 */


// Variables to send the current time to the screen
volatile uint8_t sec = 0x00;
volatile uint8_t min = 0x00;
volatile uint8_t hour = 0x00;
volatile uint8_t day = 0x00;
volatile uint8_t month = 0x00;
volatile uint32_t year = 0x00;
volatile uint8_t dow = 0x00;

// Flags for interrupts so we aren't processing a lot of code while in the interrupts
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
volatile uint8_t ALARMBUTTONFLAG = 0;

// UART variables needed in bluetooth.c and the screen files
EUSCI_A_Type * bluetooth_port = EUSCI_A1;
uint8_t screenData = 0;

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

    __enable_irq();

    // Configure buzzer and haptic driver
    config_alarm_notifications();

    // Configure UART for the screen
	EUSCI_A_Type * uart_portScreen = EUSCI_A2;
	config_uart(uart_portScreen);

    // Configure UART for bluetooth
    config_uart(bluetooth_port);

    // Create a circular buffer for bluetooth and screen receiving
    circ_buf_t * bluetoothBuffer = createBuffer(5);
    circ_buf_t * displayBuffer = createBuffer(2);

    // Create a buffer to send a bluetooth message when an alarm fires
    circ_buf_t * alarmBuffer = createBuffer(6);

    // Sends "Connected" over bluetooth to verify the watch is connected
    circ_buf_t * testBuffer = createBuffer(9);
    addMultipleToBuffer(testBuffer, "Connected", 9);
    uart_transmit_buffer(testBuffer, bluetooth_port);
    deleteBuffer(testBuffer);

    // Default time is 12:00:00 on 12/08/2020
    const RTC_C_Calendar currentTime =
    {
         0x00,       // Seconds
         0x00,       // Minutes
         0x12,       // Hours
         0x00,       // Day of week (unused in our watch)
         0x8,        // Day of month
         0x12,       // Month
         0x2020      // Year
    };

    // Configure the RTC with the above time in BCD format
    RTC_config(&currentTime, RTC_C_CTL13_BCD);; // configure the rtc clock and interupts

    // Update the screen with the default time
    clockUpdate();
    changeAllTime(uart_portScreen);

    while(1){
        // Read in from bluetooth UART
        if (RX1FLAG) {
            uart_read_to_buffer(bluetoothBuffer, bluetooth_port);
            if (isBufferFull(bluetoothBuffer)) {
                decode_bluetooth(bluetoothBuffer);
            }
        }
        // Read in from screen UART
        if (RX2FLAG){
            screenReadRegister();
            decode_screen(uart_portScreen);
        }
        // Alarm or timer goes off
        if (ALARMFLAG) {
           // Turn on the Piezo buzzer and haptic driver
           trigger_alarm();
           ALARMFLAG = 0;
           gotoAlarmButton(uart_portScreen);
           // Check if it was a timer, if so disable alarms so the timer doesn't repeat
           if (TIMERFLAG) {
               RTC_C->AMINHR = 0x0000;
               RTC_C->ADOWDAY = 0x0000;
               TIMERFLAG = 0;
               addMultipleToBuffer(alarmBuffer, "Timer", 5);
           } else {
               addMultipleToBuffer(alarmBuffer, "Alarm", 5);
           }

           // Send a message over bluetooth indicating the alarm/timer finished
           uart_transmit_buffer(alarmBuffer, bluetooth_port);
        }
        // Second ticked by, time must be updated
        if (RDYFLAG) {
            clockUpdate();
            // Only update the time if an alarm is not currently going off
            if (ALARMBUTTONFLAG == 0){
                changeAllTime(uart_portScreen);
            }
            RDYFLAG = 0;
        }
    }
}

