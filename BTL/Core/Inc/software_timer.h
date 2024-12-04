/*
 * software_timer.h
 *
 *  Created on: Sep 23, 2024
 *      Author: DUC TIN
 */

#ifndef INC_SOFTWARE_TIMER_H_
#define INC_SOFTWARE_TIMER_H_

#define TICK 10;
#include "main.h"

extern int timerCouter[10];
extern int timerFlag[10];
void settimer1(int index, int counter);
void settimer2(int index, int counter);
void timerRun();


#endif /* INC_SOFTWARE_TIMER_H_ */
