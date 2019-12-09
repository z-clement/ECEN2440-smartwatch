/*
 * pwm.h
 *
 *  Created on: Oct 14, 2019
 *      Author: Zac
 */

#ifndef PWM_H_
#define PWM_H_


/* DEFINES */
#define SYSTEM_CLOCK    3000000 // [Hz] system_msp432p401r.c
#define PWM_FREQUENCY   100000  // [Hz] PWM frequency desired
#define CALC_PERIOD(X)  (SYSTEM_CLOCK / X) // calculate # of ticks in a period

/* Configure TIMER_A0 to produce PWM waveform */
void config_pwm_timer(void);

/* Start PWM signal on Pin XX at duty_cycle 100kHz */
void start_pwm(uint8_t duty_cycle);

/* Stop Mode: clear all Mode Control bits, MC, in TAxCTL register */
void stop_pwm(void);

#endif /* PWM_H_ */

