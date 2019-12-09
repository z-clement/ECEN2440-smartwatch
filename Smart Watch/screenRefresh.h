/*
 * screenRefresh.h
 *
 *  Created on: Nov 27, 2019
 *      Author: johnc
 */

#ifndef SCREENREFRESH_H_
#define SCREENREFRESH_H_

#include <stdint.h>
#include "screenRefresh.h"
#include "alarmButton.h"
#include "msp.h"



void decode_screen(EUSCI_A_Type * screenUart);

void screenReadRegister(void);



#endif /* SCREENREFRESH_H_ */
