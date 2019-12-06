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
//#include "clock.h"


extern volatile uint8_t min;
extern volatile uint8_t hour;
extern volatile uint8_t dow;
extern volatile uint8_t day;


uint8_t * stringEnd = "ÿÿÿ";

void changeHour(EUSCI_A_Type * uartportScreen){
    int hourTen;
    int hourOne;
    uint8_t * stringBegin = "ÿÿÿn0.val=";
    if (hour < 0x0A){
        circ_buf_t * timeBuffer = createBuffer(20);
        addMultipleToBuffer(timeBuffer,stringBegin, 10);
        addToBuffer(timeBuffer, (hour + 48));
        addMultipleToBuffer(timeBuffer, stringEnd, 3);
        uart_transmit_buffer(timeBuffer, uartportScreen);
        deleteBuffer(timeBuffer);
    }
    else{
        hourTen = hour >> 4;
        hourOne = hour & 0x0F;
        circ_buf_t * timeBuffer = createBuffer(20);
        addMultipleToBuffer(timeBuffer,stringBegin, 10);
        addToBuffer(timeBuffer, (hourTen + 48));
        addToBuffer(timeBuffer, (hourOne + 48));
        addMultipleToBuffer(timeBuffer, stringEnd, 3);
        uart_transmit_buffer(timeBuffer, uartportScreen);
        deleteBuffer(timeBuffer);
    }

}



void changeMin(EUSCI_A_Type * uartportScreen){
    int minOne;
    int minTen;
    uint8_t * stringBegin = "ÿÿÿn1.val=";
    minOne = min & 0x0F;
    minTen = min >> 4;
    circ_buf_t * timeBuffer = createBuffer(20);
    addMultipleToBuffer(timeBuffer,stringBegin, 10);
    addToBuffer(timeBuffer, (minTen + 48));
    addMultipleToBuffer(timeBuffer, stringEnd, 3);
    uart_transmit_buffer(timeBuffer, uartportScreen);
    deleteBuffer(timeBuffer);
    stringBegin = "ÿÿÿn2.val=";
    circ_buf_t * minOneBuffer = createBuffer(20);
    addMultipleToBuffer(minOneBuffer,stringBegin, 10);
    addToBuffer(timeBuffer, (minOne + 48));
    addMultipleToBuffer(minOneBuffer, stringEnd, 3);
    uart_transmit_buffer(minOneBuffer, uartportScreen);
    deleteBuffer(minOneBuffer);

}


void changeDow(EUSCI_A_Type * uartportScreen){
    uint8_t * dayString;
    uint8_t * stringBegin = "ÿÿÿ";
    if(dow == 0){
        dayString = "\"Sun\"";
    }
    else if(dow == 1){
        dayString = "\"Mon\"";
    }
    else if(dow == 2){
        dayString = "\"Tue\"";
        }
    else if(dow == 3){
        dayString = "\"Wed\"";
            }
    else if(dow == 4){
        dayString = "\"Thu\"";
                }
    else if(dow == 5){
        dayString = "\"Fri\"";
                    }
    else if(dow == 6){
        dayString = "\"Sat\"";
                        }

    circ_buf_t * dowBuffer = createBuffer(25);
    addMultipleToBuffer(dowBuffer,stringBegin, 3);
    addMultipleToBuffer(dowBuffer,"t1.txt=", 7);
    addMultipleToBuffer(dowBuffer, dayString, 5);
    addMultipleToBuffer(dowBuffer, stringEnd, 3);
    uart_transmit_buffer(dowBuffer, uartportScreen);
    deleteBuffer(dowBuffer);

}


void changeDay(EUSCI_A_Type * uartportScreen){
    int dayTen;
    int dayOne;
    dayTen = day >> 4;
    dayOne = day & 0x0F;
    uint8_t * stringBegin = "ÿÿÿn3.val=";
    if (day < 0x0A){
            circ_buf_t * dayBuffer = createBuffer(20);
            addMultipleToBuffer(dayBuffer,stringBegin, 10);
            addToBuffer(dayBuffer, (day + 48));
            addMultipleToBuffer(dayBuffer, stringEnd, 3);
            uart_transmit_buffer(dayBuffer, uartportScreen);
            deleteBuffer(dayBuffer);
        }

    else{
            dayTen = day >> 4;
            dayOne = day & 0x0F;
            circ_buf_t * dayBuffer = createBuffer(20);
            addMultipleToBuffer(dayBuffer,stringBegin, 10);
            addToBuffer(dayBuffer, (dayTen + 48));
            addToBuffer(dayBuffer, (dayOne + 48));
            addMultipleToBuffer(dayBuffer, stringEnd, 3);
            uart_transmit_buffer(dayBuffer, uartportScreen);
            deleteBuffer(dayBuffer);
        }


}
