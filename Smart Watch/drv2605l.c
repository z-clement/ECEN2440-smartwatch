/*
 * drv2605l.c
 *
 *  Created on: Oct 14, 2019
 *      Author: Zac
 */

#include "msp.h"
#include "i2c.h"
#include "gpio.h"

#define DRV2605L    0x5A    // 7-bit address (8.5.3.1 note)

/* Registers addresses */
#define STATUS      0x00
#define MODE        0x01
#define GO          0x0C
#define CONTROL3    0x1D
#define LIBRARY     0x03

/* Values for registers */
#define MODE_PWM    0x03
#define MODE_STANDBY    (1 << 6)
#define MODE_STANDBY_OFF    0x00
#define GO_GO       0x01
#define LRA_LIBRARY     0x06

void config_drv2605L(void) {
    config_drv_gpio();                          // configure EN pin

    int i;
    for (i = 0; i < 100000; i++);   // need to wait 250us before writing to it

    // come out of standby mode
    write_register(DRV2605L, MODE, MODE_STANDBY_OFF);
    uint8_t mode_reg = read_register(DRV2605L, MODE); // for debugging
    // set MODE reg to 3
    write_register(DRV2605L, MODE, MODE_PWM);
    uint8_t mode_reg1 = read_register(DRV2605L, MODE); // for debugging
    // set CONTROL3 to PWM
    write_register(DRV2605L, CONTROL3, STATUS);
    uint8_t mode_reg2 = read_register(DRV2605L, CONTROL3); // for debugging
    // select LRA library
    write_register(DRV2605L, LIBRARY, LRA_LIBRARY);
    uint8_t mode_reg3 = read_register(DRV2605L, LIBRARY); // for debugging

    mode_reg = read_register(DRV2605L, MODE);
    if(mode_reg != (MODE_PWM)) {
        P1->DIR |= BIT0;
        P1->OUT |= BIT0;        // indicate error occurred
    }
    uint8_t control_three_reg = read_register(DRV2605L, CONTROL3);
    if(control_three_reg != 0x00) {
        P1->DIR |= BIT0;
        P1->OUT |= BIT0;        // indicate error occured
    }

    // Turn off EN pin
//    P4->OUT &= ~BIT6;
}
