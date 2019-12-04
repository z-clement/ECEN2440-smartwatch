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
//#include "clock.h"
#include "changeTime.h"

gotoAlarmButton(EUSCI_A_Type * uartportScreen){
    circ_buf_t * alarmButtonBuff = createBuffer(20);
    uint8_t * string = "ÿÿÿpage 2ÿÿÿ";                    // 22ÿÿÿ";
    addMultipleToBuffer(alarmButtonBuff,string, 12);
    uart_transmit_buffer(alarmButtonBuff, uartportScreen);
    deleteBuffer(alarmButtonBuff);

}


alarmOff(EUSCI_A_Type * uartportScreen){
    P4->OUT &= ~BIT6; //turn off the DRV (DRV enable pin)
    P4->OUT &= ~BIT7; //turn off the buzzer (buzzer enable pin)
    int wait = 0;
    while(wait < 10000){
        wait++;
    }
    changeMin(uartportScreen); //update the min value
    changeHour(uartportScreen); // update the hour value
}
