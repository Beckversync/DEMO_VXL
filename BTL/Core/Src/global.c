/*
 * global.c
 *
 *  Created on: Nov 24, 2024
 *      Author: Dell
 */

#include "global.h"

// Trạng thái chính
int trafficMode = INIT;
int autoStatus = INIT;
int manualStatus = OFF;
int tuningStatus = OFF;

// Bộ đếm thời gian cho từng hướng
int clock_counter_main = 0;
int clock_counter_side = 0;

// Thời gian mỗi đèn trong chế độ tự động
int timeRed = 5;
int timeAmber = 2;
int timeGreen = 3;
char str[50];

UART_HandleTypeDef huart2;
