/*
 * changeTime.c
 *
 *  Created on: Dec 2, 2019
 *      Author: johnc
 */


#include "msp.h"
#include "uart.h"
#include "bluetooth.h"
#include "circularBuf.h"
#include "screenRefresh.h"
#include "rtc.h"
#include "clock.h"





EUSCI_A_Type * screenTransmit = EUSCI_A2;

changeHour(uint8_t hour){
    int hourTen;
    int hourOne;

    if (hour < 0x0A){
        circ_buf_t * timeBuffer = createBuffer(20);
        uint8_t * stringBegin = "ÿÿÿn0.val=";                    // 22ÿÿÿ";
        addMultipleToBuffer(timeBuffer,stringBegin, 10);
        addToBuffer(timeBuffer, (hour + 48));
        uint8_t * stringEnd = "ÿÿÿ";
        addMultipleToBuffer(timeBuffer, stringEnd, 3);
        uart_transmit_buffer(timeBuffer, screenTransmit);
        deleteBuffer(timeBuffer);
    }
    else{
        hourTen = hour >> 4;
        hourOne = hour & 0x0F;
        circ_buf_t * timeBuffer = createBuffer(20);
        uint8_t * stringBegin = "ÿÿÿn0.val=";                    // 22ÿÿÿ";
        addMultipleToBuffer(timeBuffer,stringBegin, 10);
        addToBuffer(timeBuffer, (hourTen + 48));
        addToBuffer(timeBuffer, (hourOne + 48));
        uint8_t * stringEnd = "ÿÿÿ";
        addMultipleToBuffer(timeBuffer, stringEnd, 3);
        uart_transmit_buffer(timeBuffer, screenTransmit);
        deleteBuffer(timeBuffer);
    }

}



changeMin(uint8_t min){
    int minOne;
    int minTen;
    minOne = min & 0x0F;
    minTen = min >> 4;
    circ_buf_t * timeBuffer = createBuffer(20);
    uint8_t * stringBegin = "ÿÿÿn1.val=";                    // 22ÿÿÿ";
    addMultipleToBuffer(timeBuffer,stringBegin, 10);
    addToBuffer(timeBuffer, (minTen + 48));
    uint8_t * stringEnd = "ÿÿÿ";
    addMultipleToBuffer(timeBuffer, stringEnd, 3);
    uart_transmit_buffer(timeBuffer, screenTransmit);
    deleteBuffer(timeBuffer);

    circ_buf_t * minOneBuffer = createBuffer(20);
    stringBegin = "ÿÿÿn2.val=";                    // 22ÿÿÿ";
    addMultipleToBuffer(minOneBuffer,stringBegin, 10);
    addToBuffer(timeBuffer, (minOne + 48));
    stringEnd = "ÿÿÿ";
    addMultipleToBuffer(minOneBuffer, stringEnd, 3);
    uart_transmit_buffer(minOneBuffer, screenTransmit);
    deleteBuffer(minOneBuffer);

}



