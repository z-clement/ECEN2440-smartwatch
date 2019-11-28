/*
 * circularBuf.c
 *
 *  Created on: Nov 20, 2019
 *      Author: Zac
 */

#include "circularBuf.h"
#include <stdlib.h>

// Creates a circular buffer of the given size
circ_buf_t * createBuffer(uint32_t size) {
    // Allocate the memory needed to store the circular buffer struct
    circ_buf_t * newBuffer = malloc(sizeof(circ_buf_t));
    // Allocate the memory needed for all elements of the buffer
    newBuffer->buffer = malloc(sizeof(uint8_t) * size);
    // Initialize the new buffer
    newBuffer->size = size;
    newBuffer->head = 0;
    newBuffer->tail = 0;
    newBuffer->numElements = 0;
    return newBuffer;
}

// Delete the given circular buffer and free its memory
void deleteBuffer(circ_buf_t * circBuffer) {
    free(circBuffer->buffer);
    free(circBuffer);
}

// Add item to the head of the circular buffer, only succeeds if it's not full
void addToBuffer(circ_buf_t * circBuffer, uint8_t data) {
    if (!isBufferFull(circBuffer)) {
        uint8_t next = circBuffer->head + 1;  // Where head will point after the data is added
        // Check if the data needs to wrap around
        if (next >= circBuffer->size) {
            next = 0;
        }
        circBuffer->buffer[circBuffer->head] = data;    // Load in the data to the array at 'head' location
        circBuffer->head = next;                        // Increment head
        circBuffer->numElements++;  // Track the number of elements
    }
}

// Sends multiple bytes (characters) of data to the buffer
void addMultipleToBuffer(circ_buf_t * circBuffer, uint8_t * string, uint8_t length) {
    int i;
    for (i = 0; i < length; i++) {
        addToBuffer(circBuffer, string[i]);
    }
}

// Remove and return an item from the tail of the circular buffer, only succeeds if it's not empty
// If buffer is empty it returns 0
uint8_t readFromBuffer(circ_buf_t * circBuffer) {
    if (!isBufferEmpty(circBuffer)) {
        uint8_t next = circBuffer->tail + 1;  // Where tail will point after data is removed
        // Check if tail needs to wrap around
        if (next >= circBuffer->size) {
            next = 0;
        }
        uint8_t data = circBuffer->buffer[circBuffer->tail];
        circBuffer->tail = next;
        circBuffer->numElements--;
        return data;
    } else {
        return 0;
    }
}

// Function to check if the circular buffer is full (head = tail and the numElements = size)
// Return 1 means buffer is full, 0 if buffer is not full
uint8_t isBufferFull(circ_buf_t * circBuffer) {
    if (circBuffer->numElements == circBuffer->size && circBuffer->head == circBuffer->tail) {
        return 1;
    } else {
        return 0;
    }
}

// Function to check the buffer is empty (head = tail and numElements is 0)
// Return 1 means buffer is empty, 0 if buffer is not empty
uint8_t isBufferEmpty(circ_buf_t * circBuffer) {
    if (circBuffer->numElements == 0 && circBuffer->head == circBuffer->tail) {
        return 1;
    } else {
        return 0;
    }
}
