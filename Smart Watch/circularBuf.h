/*
 * circularBuf.h
 *
 *  Created on: Nov 20, 2019
 *      Author: Zac
 */

#ifndef CIRCULARBUF_H_
#define CIRCULARBUF_H_

#include <stdint.h>

typedef struct {
    uint8_t * const buffer;
    volatile uint8_t * head;
    volatile uint8_t * tail;
    const uint32_t size;
} circ_buf_t;

void addToBuffer(circ_buf_t * buffer, uint8_t data);
void removeFromBuffer(circ_buf_t * buffer);
uint8_t isBufferFull(circ_buf_t * buffer);
uint8_t isBufferEmpty(circ_buf_t * buffer);

#endif /* CIRCULARBUF_H_ */
