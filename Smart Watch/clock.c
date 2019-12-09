#include "msp.h"
#include <stdint.h>
#include "rtc.h"
#include "msp432p401r.h"
#include"clock.h"
#include "uart.h"
#include "bluetooth.h"
#include "circularBuf.h"
#include "screenRefresh.h"

/**
 * clock.c
 */

static volatile RTC_C_Calendar newTime;
static volatile RTC_C_Calendar realTime;    // For use later for reading the real time as it is updated.
volatile uint8_t RDYFLAG = 0;
volatile uint8_t ALARMFLAG = 0;
volatile uint8_t MINUTEFLAG = 0;
extern volatile uint8_t sec;
extern volatile uint8_t min;
extern volatile uint8_t hour;
extern volatile uint8_t day;
extern volatile uint8_t month;
extern volatile uint8_t dow;
extern volatile uint32_t year;
EUSCI_A_Type * uart_portScreen = EUSCI_A2;

void clock(void)
{
    // Provides a current time to initialize the RTC.
    // Later, values could be set using input (i.e. with a button press).

    const RTC_C_Calendar currentTime =
    {
            0x00,
            0x58,
            0x23,
            0x06,
            0x12,
            0x11,
            0x1955
    };

    // Stop watchdog timer.

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    // Clear interrupts.

    RTC_C_clearInterruptFlag(
            RTC_C_CTL0_RDYIE | RTC_C_CTL0_TEVIE
                        | RTC_C_CTL0_AIE);

    // Configure the RTC with the current time using BCD format.

    RTC_config(&currentTime, RTC_C_CTL13_BCD);

    // Set an alarm.

    RTC_C->AMINHR = ((0x22 ^ 0x80) << 8 )| (0x04 ^ 0x80);

    RTC_C_setCalendarEvent(RTC_C_CTL13_TEV_0);

    // Enable interrupts.

    RTC_C_enableInterrupt(RTC_C_CTL0_TEVIE | RTC_C_CTL0_AIE);
    RTC_C->PS1CTL |= (RTC_C_PS1CTL_RT1PSIE | RTC_C_PS1CTL_RT1IP__128);

    NVIC_EnableIRQ(RTC_C_IRQn);
    NVIC_SetPriority(RTC_C_IRQn, 2);
    __enable_irq();

    // Start the clock.

    RTC_C_startClock();


    return;
    // Initialize variables to hold values read from the RTC registers.
}
  void clockUpdate(void){
      // update the clock values
    MINUTEFLAG = 0;
    sec = RTCSEC;
    min = RTCMIN;
    hour = RTCHOUR;
    day = RTCDAY;
    month = RTCMON;
    year = RTCYEAR;
    dow = RTCDOW;


  }
/*
    while(1){
    if(MINUTEFLAG == 1){
                P2->OUT &= ~BIT0;
                MINUTEFLAG = 0;

                sec = RTCSEC;
                min = RTCMIN;
                hour = RTCHOUR;
                day = RTCDAY;
                month = RTCMON;
                year = RTCYEAR;
                RDYFLAG = 0;

                circ_buf_t * timeBuffer = createBuffer(20);
                uint8_t * stringBegin = "n0.val=";                    // 22ÿÿÿ";
                addMultipleToBuffer(timeBuffer,stringBegin, 6);
                addMultipleToBuffer(timeBuffer, (char)min, 2);
                uint8_t * stringEnd = "ÿÿÿ";
                addMultipleToBuffer(timeBuffer, stringEnd, 3);
                uart_transmit_buffer(timeBuffer, uart_portScreen);
                deleteBuffer(timeBuffer);








    }
    if(ALARMFLAG == 1){
               //P2->OUT |= BIT4;
                ALARMFLAG = 0;

                sec = RTCSEC;
                min = RTCMIN;
                hour = RTCHOUR;
                day = RTCDAY;
                month = RTCMON;
                year = RTCYEAR;
                RDYFLAG = 0;
    }
    else if(RDYFLAG == 1){
        sec = RTCSEC;
        min = RTCMIN;
        hour = RTCHOUR;
        day = RTCDAY;
        month = RTCMON;
        year = RTCYEAR;
        RDYFLAG = 0;
    }

    }

    */

