/*
 * alarm.h
 *
 *  Created on: Dec 4, 2019
 *      Author: Zac
 */

#ifndef ALARM_H_
#define ALARM_H_

// Configure the DRV, LRA, and buzzer
void config_alarm_notifications(void);

// Turn the drv and buzzer on
void trigger_alarm(void);

#endif /* ALARM_H_ */
