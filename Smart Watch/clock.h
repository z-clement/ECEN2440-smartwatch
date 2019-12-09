/*
 * clock.h
 *
 *  Created on: Nov 27, 2019
 *      Author: johnc
 */

#ifndef CLOCK_H_
#define CLOCK_H_

#include "msp.h"
#include <stdint.h>
#include "rtc.h"
#include "msp432p401r.h"
#include "uart.h"
#include "bluetooth.h"
#include "circularBuf.h"
#include "screenRefresh.h"

void clock(void);

void clockUpdate(void);


#endif /* CLOCK_H_ */
