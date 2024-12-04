/*
 * System_FSM.h
 *
 *  Created on: Nov 24, 2024
 *      Author: Dell
 */
#ifndef INC_SYSTEM_FSM_H_
#define INC_SYSTEM_FSM_H_

#include "global.h"
#include <stdio.h>
#include "i2c-lcd.h"
#include "software_timer.h"
#include <stdint.h>
#include <stdbool.h>
#include "stm32f1xx_hal.h"
#include "UartRingbuffer_multi.h"

#define DEBOUNCE_DELAY 200 // 200 ms debounce delay

void fsm_switch_mode(void);
void fsm_traffic(void);
void clock_counter_traffic_update(void);

#endif /* INC_SYSTEM_FSM_H_ */
