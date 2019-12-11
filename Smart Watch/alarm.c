/*
 * alarm.c
 *
 *  Created on: Dec 4, 2019
 *      Author: Zac
 */

#include "msp.h"
#include "drv2605l.h"
#include "gpio.h"
#include "i2c.h"
#include "pwm.h"
#include "alarm.h"

// Configure the DRV, LRA, and buzzer
void config_alarm_notifications(void) {
    // Configurations for drv and haptic driver
    // configure the I2C
    config_i2c();
    // configure the timer
    config_pwm_timer();
    // configure GPIO on P2.4
    config_pwm_gpio();
    // configure the DRV2605L
    config_drv2605L();

    // Configurations for buzzer pins
    config_buzzer_gpio();
}

// Turn the drv and buzzer on
void trigger_alarm(void) {
    // Turn the drv on
    start_pwm(90);
    // Turn the buzzer on
    P4->OUT |= BIT7;
}
