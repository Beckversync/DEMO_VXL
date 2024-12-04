/*
 * Traffic_Light_FSM.c
 *
 *  Created on: Nov 24, 2024
 *      Author: Dell
 */

#include "Traffic_Light_FSM.h"

void turnOffAllLED(void) {
	HAL_GPIO_WritePin(GPIOB, LED1_A_Pin|LED1_B_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, LED2_A_Pin|LED2_B_Pin, GPIO_PIN_RESET);
}

// Turn on RED LED for specified way
void turnOnRed1() {
	HAL_GPIO_WritePin(GPIOB, LED1_A_Pin|LED1_B_Pin, GPIO_PIN_SET);
}

// Turn on AMBER LED for specified way
void turnOnAmber1() {
	HAL_GPIO_WritePin(GPIOB, LED1_A_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, LED1_B_Pin, GPIO_PIN_SET);
}

// Turn on GREEN LED for specified way
void turnOnGreen1() {
	HAL_GPIO_WritePin(GPIOB, LED1_B_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, LED1_A_Pin, GPIO_PIN_SET);
}
void turnOnRed2() {
	HAL_GPIO_WritePin(GPIOA, LED2_A_Pin|LED2_B_Pin, GPIO_PIN_SET);
}

// Turn on AMBER LED for specified way
void turnOnAmber2() {
	HAL_GPIO_WritePin(GPIOA, LED2_A_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, LED2_B_Pin, GPIO_PIN_SET);
}

// Turn on GREEN LED for specified way
void turnOnGreen2() {
	HAL_GPIO_WritePin(GPIOA, LED2_B_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, LED2_A_Pin, GPIO_PIN_SET);
}
// Blink RED LEDs
void blinkyRed(void) {
    if (timerFlag[5] == 1) {
        HAL_GPIO_TogglePin(GPIOA,LED2_B_Pin|LED2_A_Pin);
        HAL_GPIO_TogglePin(GPIOB, LED1_A_Pin | LED1_B_Pin);
        settimer1(5, 500);
    }
}

// Blink AMBER LEDs
void blinkyAmber(void) {
    if (timerFlag[7] == 1) {
        HAL_GPIO_WritePin(GPIOB, LED1_A_Pin, RESET);
        HAL_GPIO_TogglePin(GPIOB, LED1_B_Pin);
        HAL_GPIO_WritePin(GPIOA, LED2_A_Pin, RESET);
        HAL_GPIO_TogglePin(GPIOA, LED2_B_Pin);
        settimer1(7, 500);
    }
}

// Blink GREEN LEDs
void blinkyGreen(void) {
    if (timerFlag[6] == 1) {
        HAL_GPIO_WritePin(GPIOB, LED1_B_Pin, RESET);
        HAL_GPIO_TogglePin(GPIOB, LED1_A_Pin);
        HAL_GPIO_WritePin(GPIOA, LED2_B_Pin, RESET);
        HAL_GPIO_TogglePin(GPIOA, LED2_A_Pin);
        settimer1(6, 500);
    }
}

// Clear all road LEDs
void clearRoadLed(void) {
    turnOffAllLED();
}
