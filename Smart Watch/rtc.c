/*
 * rtc.c
 *
 *  Created on: Nov 20, 2019
 *      Author: David
 */

#include "msp.h"
#include <stdint.h>
#include "rtc.h"
#include "msp432p401r.h"

extern volatile uint8_t RDYFLAG;
extern volatile uint8_t ALARMFLAG;
extern volatile uint8_t MINUTEFLAG;


void RTC_config(const RTC_C_Calendar *calendarTime,
                uint_fast16_t formatSelect){

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

}

void RTC_C_clearInterruptFlag(uint_fast8_t interruptFlagMask)
{
    if (interruptFlagMask
            & (RTC_C_CTL0_TEVIE + RTC_C_CTL0_AIE
                    + RTC_C_CTL0_RDYIE
                    + RTC_C_CTL0_OFIE))
    {
        RTC_C->CTL0 = RTC_C_KEY
                | (RTC_C->CTL0 & ~((interruptFlagMask >> 4) | RTC_C_CTL0_KEY_MASK));
        BITBAND_PERI(RTC_C->CTL0, RTC_C_CTL0_KEY_OFS) = 0;
    }

//    if (interruptFlagMask & RTC_C_PRESCALE_TIMER0_INTERRUPT)
    if (interruptFlagMask & RTC_C_PS0CTL_RT0PSIE)
    {
        BITBAND_PERI(RTC_C->PS0CTL, RTC_C_PS0CTL_RT0PSIFG_OFS) = 0;
    }

//    if (interruptFlagMask & RTC_C_PRESCALE_TIMER1_INTERRUPT)
    if (interruptFlagMask & RTC_C_PS1CTL_RT1PSIE)
    {
        BITBAND_PERI(RTC_C->PS1CTL, RTC_C_PS1CTL_RT1PSIFG_OFS) = 0;
    }
}

void RTC_C_enableInterrupt(uint8_t interruptMask)
{
    if (interruptMask & (RTC_C_CTL0_OFIE + RTC_C_CTL0_TEVIE + RTC_C_CTL0_AIE
            + RTC_C_CTL0_RDYIE))
    {
        RTC_C->CTL0 = RTC_C_KEY | (interruptMask
                & (RTC_C_CTL0_OFIE + RTC_C_CTL0_TEVIE + RTC_C_CTL0_AIE
                + RTC_C_CTL0_RDYIE));
        BITBAND_PERI(RTC_C->CTL0, RTC_C_CTL0_KEY_OFS) = 0;
    }

//    if (interruptMask & RTC_C_PRESCALE_TIMER0_INTERRUPT)
    if (interruptMask & RTC_C_PS0CTL_RT0IP_MASK)
    {
        BITBAND_PERI(RTC_C->PS0CTL, RTC_C_PS0CTL_RT0PSIE_OFS) = 1;
    }

//    if (interruptMask & RTC_C_PRESCALE_TIMER1_INTERRUPT)
    if (interruptMask & RTC_C_PS1CTL_RT1IP_MASK)
    {
        BITBAND_PERI(RTC_C->PS1CTL,RTC_C_PS1CTL_RT1PSIE_OFS) = 1;
    }
}

void RTC_C_setCalendarEvent(uint_fast16_t eventSelect)
{
    RTC_C->CTL0 = (RTC_C->CTL0 & ~RTC_C_CTL0_KEY_MASK) | RTC_C_KEY;
    RTC_C->CTL13 = (RTC_C->CTL13 & ~(RTC_C_CTL13_TEV_3)) | eventSelect;
    BITBAND_PERI(RTC_C->CTL0, RTC_C_CTL0_KEY_OFS) = 0;
}

void RTC_C_startClock(void)
{
    RTC_C->CTL0 = (RTC_C->CTL0 & ~RTC_C_CTL0_KEY_MASK) | RTC_C_KEY;
    BITBAND_PERI(RTC_C->CTL13, RTC_C_CTL13_HOLD_OFS) = 0;
    BITBAND_PERI(RTC_C->CTL0, RTC_C_CTL0_KEY_OFS) = 0;
}

void RTC_C_IRQHandler(void){
    if(RTC_C->CTL0 & RTCTEVIFG){
            MINUTEFLAG = 1;
            RDYFLAG = 1;
            //RTC_C->CTL0 &= ~RTCTEVIFG;
            RTC_C_clearInterruptFlag(RTC_C_CTL0_TEVIE);
    }
    if(RTC_C->CTL0 & RTCAIFG){
            ALARMFLAG = 1;
            RDYFLAG = 1;
    }
    else if(RTC_C->PS1CTL &  RT1PSIFG){

       RDYFLAG = 1;
       RTC_C->PS1CTL &= (~RT1PSIFG);
    }

    /* else if(RTC_C->CTL0 & RTCRDYIFG){
        //RDYFLAG = 1;
        RTC_C_clearInterruptFlag(RTC_C_CTL0_RDYIE);
//        RTC_C->CTL0 &= ~RTCRDYIFG;
    }
*/
}
