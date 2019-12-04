/*
 * uart.h
 *
 *  Created on: Nov 10, 2019
 *      Author: Zac
 */

#ifndef UART_H_
#define UART_H_

#include "circularBuf.h"
#include <stdint.h>

#define BR_CLOCK    12000000                // SMCLK Frequency * 4
#define BAUD_RATE   9600                    // Baud Rate
#define BR_DIV      (BR_CLOCK / BAUD_RATE)  // Division factor for baud rate generator

void config_uart(EUSCI_A_Type * uart_port);

void config_uart_ports(EUSCI_A_Type * uart_port);

void config_clock(void);

void uart_transmit_byte(uint8_t data, EUSCI_A_Type * uart_port);

void uart_transmit_buffer(circ_buf_t * circBuffer, EUSCI_A_Type * uart_port);

void uart_read_to_buffer(circ_buf_t * circBuffer, EUSCI_A_Type * uart_port);

#endif /* UART_H_ */
