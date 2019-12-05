/*
 * gpio.c
 *
 *  Created on: Oct 14, 2019
 *      Author: Zac
 */


#include "msp.h"
#include "gpio.h"

/* Config P2.4 to output TA0.1 waveform */
void config_pwm_gpio(void) {
    // Set port 2 pin 4 to output
    P2->DIR |= BIT4;
    // Set port 2 pin 4 to equal the out voltage of the timer
    P2->SEL0 |= BIT4;
    P2->SEL1 &= ~BIT4;
}

// Configure P4.6 as a GPIO output for the DRV EN pin
void config_drv_gpio(void) {
    P4->DIR |= BIT6;
    P4->OUT |= BIT6;
}

// Configure P4.7 as GPIO for the buzzer
void config_buzzer_gpio(void) {
    P4->DIR |= BIT7;
}
