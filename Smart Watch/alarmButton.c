/*
 * alarmButton.c
 *
 *  Created on: Dec 4, 2019
 *      Author: johnc
 */


#include "msp.h"
#include "uart.h"
#include "bluetooth.h"
#include "circularBuf.h"
#include "screenRefresh.h"
#include "rtc.h"
#include "pwm.h"
#include "changeTime.h"

// Flag to track that an alarm is still going off, so the screen should be the alarm screen
extern volatile int ALARMBUTTONFLAG;

// Send a command to the screen to switch to the alarm off screen
void gotoAlarmButton(EUSCI_A_Type * uartportScreen){
    circ_buf_t * alarmButtonBuff = createBuffer(20);
    uint8_t * string = "ÿÿÿpage 2ÿÿÿ";
    addMultipleToBuffer(alarmButtonBuff,string, 12);
    uart_transmit_buffer(alarmButtonBuff, uartportScreen);
    deleteBuffer(alarmButtonBuff);
    ALARMBUTTONFLAG = 1;
}

// Send a command to the screen to switch to the clock screen
void gotoHome(EUSCI_A_Type * uartportScreen){
    circ_buf_t * alarmButtonBuff = createBuffer(20);
    uint8_t * string = "ÿÿÿpage 0ÿÿÿ";                    // 22ÿÿÿ";
    addMultipleToBuffer(alarmButtonBuff,string, 12);
    uart_transmit_buffer(alarmButtonBuff, uartportScreen);
    deleteBuffer(alarmButtonBuff);
    ALARMBUTTONFLAG = 0;
}

// Turn off the drv and buzzer
void alarmOff(EUSCI_A_Type * uartportScreen){
    // Turn of the LRA by setting the duty cycle to 0
    start_pwm(0);
    // Set the buzzer enable pin to low
    P4->OUT &= ~BIT7;

    changeMin(uartportScreen); //update the min value
    changeHour(uartportScreen); // update the hour value
}
