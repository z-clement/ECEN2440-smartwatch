
/*
 * rtc.c
 *
 *  Created on: Nov 20, 2019
 *      Author: David
 */

#include "msp.h"
#include <stdint.h>
#include "rtc.h"

// Interrupt flags
extern volatile uint8_t RDYFLAG;
extern volatile uint8_t ALARMFLAG;

// Variables to track the current time
extern volatile uint8_t sec;
extern volatile uint8_t min;
extern volatile uint8_t hour;
extern volatile uint8_t day;
extern volatile uint8_t month;
extern volatile uint8_t dow;
extern volatile uint32_t year;


// Configure the real time clock with the given start time, and BCD or Hex format
void RTC_config(const RTC_C_Calendar *calendarTime, uint_fast16_t formatSelect){

    // Unlock the RTC register to write the intial time values
    RTC_C->CTL0 = (RTC_C->CTL0 & ~RTC_C_CTL0_KEY_MASK) | RTC_C_KEY;

    // Halt the RTC counters in order to write to registers
    BITBAND_PERI(RTC_C->CTL13, RTC_C_CTL13_HOLD_OFS) = 1;

    // Check to see if the format specified in the function call was
    // BCD or hexadecimal.
    if (formatSelect)
        BITBAND_PERI(RTC_C->CTL13, RTC_C_CTL13_BCD_OFS) = 1;
    else
        BITBAND_PERI(RTC_C->CTL13, RTC_C_CTL13_BCD_OFS) = 0;

    // Write the values provided in calendarTime to the appropriate registers.
    RTC_C->TIM0 = (calendarTime->minutes<<RTC_C_TIM0_MIN_OFS) | calendarTime->seconds;
    RTC_C->TIM1 = (calendarTime->dayOfWeek<<RTC_C_TIM1_DOW_OFS) | calendarTime->hours;
    RTC_C->DATE = (calendarTime->month<<RTC_C_DATE_MON_OFS) | calendarTime->dayOfmonth;
    RTC_C->YEAR = calendarTime->year;

    // Reset the key to lock the key - protected registers.
    BITBAND_PERI(RTC_C->CTL0, RTC_C_CTL0_KEY_OFS) = 0;

    // Clear RTC interrupts.
    RTC_C_clearInterruptFlag(RTC_C_CTL0_RDYIE | RTC_C_CTL0_TEVIE | RTC_C_CTL0_AIE);

    // Enable RTC interrupts for the alarm
    RTC_C_enableInterrupt(RTC_C_CTL0_AIE);

    // Enable RT1PSIFG with the prescalar set so it fires every second (1 Hz frequency)
    RTC_C->PS1CTL |= (RTC_C_PS1CTL_RT1PSIE | RTC_C_PS1CTL_RT1IP__128);   // divide by 128 to get 1 Hz frequency

    NVIC_EnableIRQ(RTC_C_IRQn);
    NVIC_SetPriority(RTC_C_IRQn, 2);

    RTC_C_startClock();
}

// Clear the interrupt flags in CTL0 register that're passed in
void RTC_C_clearInterruptFlag(uint_fast8_t interruptFlagMask)
{
    // Check for event interrupt (TEVIE), alarm interrupt (AIE), ready interrupt (RDYIE), or clock error (OFIE)
    if (interruptFlagMask & (RTC_C_CTL0_TEVIE + RTC_C_CTL0_AIE + RTC_C_CTL0_RDYIE + RTC_C_CTL0_OFIE))
    {
        RTC_C->CTL0 = RTC_C_KEY | (RTC_C->CTL0 & ~((interruptFlagMask >> 4) | RTC_C_CTL0_KEY_MASK));
        BITBAND_PERI(RTC_C->CTL0, RTC_C_CTL0_KEY_OFS) = 0;
    }
}

// Enable interrupts within CTL0 register that must be unlocked
void RTC_C_enableInterrupt(uint8_t interruptMask)
{
    // Enable event interrupt (TEVIE), alarm interrupt (AIE), ready interrupt (RDYIE), or clock error (OFIE)
    if (interruptMask & (RTC_C_CTL0_OFIE + RTC_C_CTL0_TEVIE + RTC_C_CTL0_AIE + RTC_C_CTL0_RDYIE))
    {
        RTC_C->CTL0 = RTC_C_KEY | (interruptMask & (RTC_C_CTL0_OFIE + RTC_C_CTL0_TEVIE + RTC_C_CTL0_AIE + RTC_C_CTL0_RDYIE));
        BITBAND_PERI(RTC_C->CTL0, RTC_C_CTL0_KEY_OFS) = 0;
    }
}

// Start the Real Time Clock
void RTC_C_startClock(void)
{
    RTC_C->CTL0 = (RTC_C->CTL0 & ~RTC_C_CTL0_KEY_MASK) | RTC_C_KEY;
    BITBAND_PERI(RTC_C->CTL13, RTC_C_CTL13_HOLD_OFS) = 0;
    BITBAND_PERI(RTC_C->CTL0, RTC_C_CTL0_KEY_OFS) = 0;
}


void RTC_C_IRQHandler(void){
    if(RTC_C->CTL0 & RTCAIFG){
            ALARMFLAG = 1;
            RDYFLAG = 1;
            RTC_C_clearInterruptFlag(RTC_C_CTL0_AIE);
    }
    else if(RTC_C->PS1CTL &  RT1PSIFG){

       RDYFLAG = 1;
       RTC_C->PS1CTL &= (~RT1PSIFG);
    }
}

// Update the clock values
void clockUpdate(void){
    sec = RTCSEC;
    min = RTCMIN;
    hour = RTCHOUR;
    day = RTCDAY;
    month = RTCMON;
    year = RTCYEAR;
    dow = RTCDOW;
}
