/*
 * uart.h
 *
 *  Created on: Nov 10, 2019
 *      Author: Zac
 */

#ifndef UART_H_
#define UART_H_

#include <stdint.h>

#define BR_CLOCK    12000000                // SMCLK Frequency * 4
#define BAUD_RATE   9600                    // Baud Rate
#define BR_DIV      (BR_CLOCK / BAUD_RATE)  // Division factor for baud rate generator

void config_uart(void);

void config_uart_ports(void);

void config_clock(void);

void uart_transmit_byte(uint8_t data);

#endif /* UART_H_ */
