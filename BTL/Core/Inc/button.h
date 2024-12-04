/*
 * button.h
 *
 *  Created on: Oct 5, 2023
 *      Author: KAI
 */

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

#include "main.h"
#include "global.h"

#define NORMAL_STATE SET
#define PRESSED_STATE RESET
#define NUM_BUTTONS 3
extern int button_flag[NUM_BUTTONS];

void getKeyInput();

#endif /* INC_BUTTON_H_ */

