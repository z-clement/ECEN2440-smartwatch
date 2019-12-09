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
//#include "clock.h"
#include "changeTime.h"


extern volatile int ALARMBUTTONFLAG;

void gotoAlarmButton(EUSCI_A_Type * uartportScreen){
    circ_buf_t * alarmButtonBuff = createBuffer(20);
    uint8_t * string = "ÿÿÿpage 2ÿÿÿ";                    // 22ÿÿÿ";
    addMultipleToBuffer(alarmButtonBuff,string, 12);
    uart_transmit_buffer(alarmButtonBuff, uartportScreen);
    deleteBuffer(alarmButtonBuff);
    ALARMBUTTONFLAG = 1;
}

void gotoHome(EUSCI_A_Type * uartportScreen){
    circ_buf_t * alarmButtonBuff = createBuffer(20);
    uint8_t * string = "ÿÿÿpage 0ÿÿÿ";                    // 22ÿÿÿ";
    addMultipleToBuffer(alarmButtonBuff,string, 12);
    uart_transmit_buffer(alarmButtonBuff, uartportScreen);
    deleteBuffer(alarmButtonBuff);
    ALARMBUTTONFLAG = 0;
}

void alarmOff(EUSCI_A_Type * uartportScreen){
//    P4->OUT &= ~BIT6; //turn off the DRV (DRV enable pin)
    start_pwm(0);
    P4->OUT &= ~BIT7; //turn off the buzzer (buzzer enable pin)
    //int wait = 0;

    changeMin(uartportScreen); //update the min value
    changeHour(uartportScreen); // update the hour value
}
