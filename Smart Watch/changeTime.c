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

// Variables to send the current time to the screen
extern volatile uint8_t min;
extern volatile uint8_t hour;
extern volatile uint8_t dow;
extern volatile uint8_t day;
extern volatile uint8_t month;
extern volatile uint32_t year;

// Specific for the screen we're using, this denotes the end of a UART transmission
uint8_t * stringEnd = "ÿÿÿ";

// Change the hour on the screen
void changeHour(EUSCI_A_Type * uartportScreen){
    int hourTen;
    int hourOne;
    uint8_t * stringBegin = "ÿÿÿn0.val=";
    // If the hour is over 10, the screen needs a different transmission to have hours display a single digit
    if (hour < 0x0A){
        circ_buf_t * timeBuffer = createBuffer(20);
        addMultipleToBuffer(timeBuffer, stringBegin, 10);

        // Send the ASCII representation of the hour number to the screen
        addToBuffer(timeBuffer, (hour + 48));

        addMultipleToBuffer(timeBuffer, stringEnd, 3);
        uart_transmit_buffer(timeBuffer, uartportScreen);
        deleteBuffer(timeBuffer);
    }
    else {
        // If the hour has 2 digits they need to be sent separately in BCD
        hourTen = hour >> 4;
        hourOne = hour & 0x0F;

        circ_buf_t * timeBuffer = createBuffer(20);
        addMultipleToBuffer(timeBuffer,stringBegin, 10);

        // Convert the tens and ones digits to ASCII and send them to the screen
        addToBuffer(timeBuffer, (hourTen + 48));
        addToBuffer(timeBuffer, (hourOne + 48));

        addMultipleToBuffer(timeBuffer, stringEnd, 3);
        uart_transmit_buffer(timeBuffer, uartportScreen);
        deleteBuffer(timeBuffer);
    }

}

// Change the minute display on the screen
void changeMin(EUSCI_A_Type * uartportScreen){
    int minOne;
    int minTen;
    uint8_t * stringBegin = "ÿÿÿn1.val=";

    // Get the minute digits from the BCD representation
    minOne = min & 0x0F;
    minTen = min >> 4;

    circ_buf_t * timeBuffer = createBuffer(20);
    addMultipleToBuffer(timeBuffer,stringBegin, 10);
    // Send the tens minute digit in ASCII
    addToBuffer(timeBuffer, (minTen + 48));
    addMultipleToBuffer(timeBuffer, stringEnd, 3);
    uart_transmit_buffer(timeBuffer, uartportScreen);
    deleteBuffer(timeBuffer);

    // The ones digit needs a different beginning code for the screen to update properly
    stringBegin = "ÿÿÿn2.val=";
    circ_buf_t * minOneBuffer = createBuffer(20);
    addMultipleToBuffer(minOneBuffer,stringBegin, 10);
    // Send the ones digit to the screen in ASCII
    addToBuffer(timeBuffer, (minOne + 48));
    addMultipleToBuffer(minOneBuffer, stringEnd, 3);
    uart_transmit_buffer(minOneBuffer, uartportScreen);
    deleteBuffer(minOneBuffer);

}

// Change the day of week on the screen
void changeDow(EUSCI_A_Type * uartportScreen){
    uint8_t * dayString;
    uint8_t * stringBegin = "ÿÿÿ";
    // Send a different three character screen for each day of the week 0-6 (Sunday - Saturday)
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

// Change the day display on the screen
void changeDay(EUSCI_A_Type * uartportScreen){
    int dayTen;
    int dayOne;
    uint8_t * stringBegin = "ÿÿÿn4.val=";
    // If the day is < 10 send only the day value
    if (day < 0x0A){
        circ_buf_t * dayBuffer = createBuffer(20);
        addMultipleToBuffer(dayBuffer,stringBegin, 10);
        // Send the day value in ASCII
        addToBuffer(dayBuffer, (day + 48));
        addMultipleToBuffer(dayBuffer, stringEnd, 3);
        uart_transmit_buffer(dayBuffer, uartportScreen);
        deleteBuffer(dayBuffer);
    } else {
        // Get the BCD digits representing the day
        dayTen = day >> 4;
        dayOne = day & 0x0F;
        circ_buf_t * dayBuffer = createBuffer(20);
        addMultipleToBuffer(dayBuffer,stringBegin, 10);
        // Send each digit individually in ASCII
        addToBuffer(dayBuffer, (dayTen + 48));
        addToBuffer(dayBuffer, (dayOne + 48));
        addMultipleToBuffer(dayBuffer, stringEnd, 3);
        uart_transmit_buffer(dayBuffer, uartportScreen);
        deleteBuffer(dayBuffer);
    }
}

// Change the month on the display
void changeMonth(EUSCI_A_Type * uartportScreen){
    int monthTen;
    int monthOne;
    monthTen = month >> 4;
    monthOne = month & 0x0F;
    uint8_t * stringBegin = "ÿÿÿn3.val=";
    // If the month is one digit ( < 10) send only the month value
    if (month < 0x0A){
        circ_buf_t * monthBuffer = createBuffer(20);
        addMultipleToBuffer(monthBuffer,stringBegin, 10);
        // Send the month to the screen in ASCII
        addToBuffer(monthBuffer, (month + 48));
        addMultipleToBuffer(monthBuffer, stringEnd, 3);
        uart_transmit_buffer(monthBuffer, uartportScreen);
        deleteBuffer(monthBuffer);
    }

    else{
        // Get the month digits from BCD
        monthTen = month >> 4;
        monthOne = month & 0x0F;
        circ_buf_t * monthBuffer = createBuffer(20);
        addMultipleToBuffer(monthBuffer,stringBegin, 10);
        // Send the tens and ones digits in ASCII
        addToBuffer(monthBuffer, (monthTen + 48));
        addToBuffer(monthBuffer, (monthOne + 48));
        addMultipleToBuffer(monthBuffer, stringEnd, 3);
        uart_transmit_buffer(monthBuffer, uartportScreen);
        deleteBuffer(monthBuffer);
    }
}

// Change the year on the display
void changeYear(EUSCI_A_Type * uartportScreen){
    // Get the year digits from BCD
    int yearThou = year >> 12;
    int yearHun = year >> 16;
    int yearTen = year >> 4;
    int yearOne = year & 0x0F;
    uint8_t * stringBegin = "ÿÿÿn5.val=";
    circ_buf_t * yearBuffer = createBuffer(40);
    addMultipleToBuffer(yearBuffer,stringBegin, 10);
    // Send all the digits to the screen, converted to ASCII
    addToBuffer(yearBuffer, (yearThou + 48));
    addToBuffer(yearBuffer, (yearHun + 48));
    addToBuffer(yearBuffer, (yearTen + 48));
    addToBuffer(yearBuffer, (yearOne + 48));
    addMultipleToBuffer(yearBuffer, stringEnd, 3);
    uart_transmit_buffer(yearBuffer, uartportScreen);
    deleteBuffer(yearBuffer);

}

// Change all the time on the display
void changeAllTime(EUSCI_A_Type * uartportScreen){
    changeHour(uartportScreen);
    changeMin(uartportScreen);
    changeDow(uartportScreen);
    changeDay(uartportScreen);
    changeMonth(uartportScreen);
    changeYear(uartportScreen);
}
