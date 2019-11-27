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
    uint8_t * buffer;               // Array storing the data
    volatile uint8_t head;          // Int for indexing the head of the array (where new data is added)
    volatile uint8_t tail;          // Int for indexing the tail of the array (where data is removed/read)
    volatile uint32_t numElements;  // Int tracking the number of elements stored in the buffer
    uint32_t size;                  // Max size of the buffer
} circ_buf_t;

circ_buf_t* createBuffer(uint32_t length);

void deleteBuffer(circ_buf_t * circBuffer);

void addToBuffer(circ_buf_t * circBuffer, uint8_t data);

void addMultipleToBuffer(circ_buf_t * circBuffer, uint8_t * string, uint8_t length);

uint8_t readFromBuffer(circ_buf_t * circBuffer);

uint8_t isBufferFull(circ_buf_t * circBuffer);

uint8_t isBufferEmpty(circ_buf_t * circBuffer);

#endif /* CIRCULARBUF_H_ */
