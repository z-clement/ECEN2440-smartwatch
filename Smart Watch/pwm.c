/*
 * pwm.c
 *
 *  Created on: Oct 14, 2019
 *      Author: Zac
 */


#include "msp.h"
#include "pwm.h"

/* Configure TIMER_A0 to produce PWM waveform */
void config_pwm_timer(void) {
    TIMER_A0->CTL |= 0x0002;     // Reset TAxR
    TIMER_A0->CTL |= 0x0200;     // Select SMCLK
    TIMER_A0->CCTL[1] |= 0x00E0;   // select reset/set output mode for T0.1 timer using CCTL[1]
}

/*  Start the timer counting
 *  @param uint8_t duty_cycle: 0-100, percentage of time ON */
void start_pwm(uint8_t duty_cycle) {
    // ticks for 100 kHz = CALC_PERIOD(100 kHz)
    uint16_t ticks_in_period = CALC_PERIOD(PWM_FREQUENCY);
    // Set the TA0CCR0 register to ticks -> the total period for duty cycle in processor ticks
    TIMER_A0->CCR[0] = ticks_in_period;
    // Set the TA0CCR1 register to ticks * duty_cycle / 100 -> ticks for a HIGH output
    uint8_t pwm_cycle = (ticks_in_period * duty_cycle) / 100;
    TIMER_A0->CCR[1] = pwm_cycle;
    // Set bits 4-5 in TA0CTL to 01 to turn the timer on in UP mode
    TIMER_A0->CTL |= 0x0010;
    TIMER_A0->CTL &= ~0x0020;
}

/* Stop Mode: clear all Mode Control bits, MC, in TAxCTL register */
void stop_pwm(void) {
    // Set bits 4-5 in TA0CTL to 00 to turn off the Mode Control bits
    TIMER_A0->CTL &= ~(BIT4 | BIT5);
    TIMER_A0->CCR[0] = 0;
    TIMER_A0->CCR[1] = 0;
}
