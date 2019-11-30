/*
 * bluetooth.h
 *
 *  Created on: Nov 17, 2019
 *      Author: Zac
 */

#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

#include "circularBuf.h"

void decode_bluetooth(circ_buf_t * receiveBuffer);

#endif /* BLUETOOTH_H_ */
