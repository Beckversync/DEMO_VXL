/*
 * System_FSM.c
 *
 *  Created on: Nov 24, 2024
 *      Author: Dell
 */


#include "System_FSM.h"
#include "Traffic_Light_FSM.h"


// ======================= CHẾ ĐỘ TỰ ĐỘNG =======================


void switchAutoState(int newState, int mainTime, int sideTime) {
    autoStatus = newState;
    clock_counter_main = mainTime;
    clock_counter_side = sideTime;
    clearRoadLed();
}

void fsm_traffic_auto_mode(void) {


    switch (autoStatus) {
        case OFF:
            break;
        case INIT:
            switchAutoState(RED_GREEN, timeRed, timeGreen);
            settimer1(1, 50);
            break;
        case RED_GREEN:
            turnOnRed1();
            turnOnGreen2();
            if (timerFlag[1] == 1) {
                if (clock_counter_side >= 0) {
                	lcd_clear_display();
                	HAL_Delay(10);
                	char buffer[16];
                	snprintf(buffer, sizeof(buffer), "RED: %d", clock_counter_main);
                	lcd_goto_XY(1, 0);
                	lcd_send_string(buffer);
                	char buffer1[16];
                	snprintf(buffer1, sizeof(buffer1), "GREEN: %d", clock_counter_side);
                	lcd_goto_XY(2, 0);
                	lcd_send_string(buffer1);
                    clock_counter_side--;
                    clock_counter_main--;
                }
                settimer1(1, 1000);
            }
            if (clock_counter_side < 0) {
                switchAutoState(RED_AMBER, clock_counter_main + 1, timeAmber);
                timerFlag[2] = 1;
            }
            break;
        case RED_AMBER:
            turnOnRed1();
            turnOnAmber2();
            if (timerFlag[2] == 1) {
                if (clock_counter_side >= 0) {
                	lcd_clear_display();
                	HAL_Delay(10);
                	char buffer[16];
                	snprintf(buffer, sizeof(buffer), "RED: %d", clock_counter_main);
                	lcd_goto_XY(1, 0);
                	lcd_send_string(buffer);
                	char buffer1[16];
                	snprintf(buffer1, sizeof(buffer1), "AMBER: %d", clock_counter_side);
                	lcd_goto_XY(2, 0);
                	lcd_send_string(buffer1);
                    clock_counter_side--;
                    clock_counter_main--;
                    settimer1(2, 1000);
                }
            }
            if (clock_counter_side < 0) {
                switchAutoState(GREEN_RED, timeGreen, timeRed);
                timerFlag[3] = 1;
            }
            break;
        case GREEN_RED:
            turnOnGreen1();
            turnOnRed2();
            if (timerFlag[3] == 1) {
                if (clock_counter_side >= 0) {
                	lcd_clear_display();
                	HAL_Delay(10);
                	char buffer[16];
                	snprintf(buffer, sizeof(buffer), "GREEN: %d", clock_counter_main);
                	lcd_goto_XY(1, 0);
                	lcd_send_string(buffer);
                	char buffer1[16];
                	snprintf(buffer1, sizeof(buffer1), "RED: %d", clock_counter_side);
                	lcd_goto_XY(2, 0);
                	lcd_send_string(buffer1);
                    clock_counter_side--;
                    clock_counter_main--;
                    settimer1(3, 1000);
                }
            }
            if (clock_counter_main < 0) {
                switchAutoState(AMBER_RED, timeAmber, clock_counter_side + 1);
                timerFlag[4] = 1;
            }
            break;
        case AMBER_RED:
            turnOnAmber1();
            turnOnRed2();
            if (timerFlag[4] == 1) {
                if (clock_counter_side >= 0) {
                	lcd_clear_display();
                	HAL_Delay(10);
                	char buffer[16];
                	snprintf(buffer, sizeof(buffer), "AMBER: %d", clock_counter_main);
                	lcd_goto_XY(1, 0);
                	lcd_send_string(buffer);
                	char buffer1[16];
                	snprintf(buffer1, sizeof(buffer1), "RED: %d", clock_counter_side);
                	lcd_goto_XY(2, 0);
                	lcd_send_string(buffer1);
                    clock_counter_side--;
                    clock_counter_main--;
                    settimer1(4, 1000);
                }
            }
            if (clock_counter_main < 0) {
                switchAutoState(RED_GREEN, timeRed, timeGreen);
                timerFlag[1] = 1;
            }
            break;
        default:
            break;
    }
}


// ======================= CHẾ ĐỘ TINH CHỈNH =======================

static uint32_t lastButtonPressTime[3] = {0, 0, 0}; // Array to store last press time for each button

bool isButtonDebounced(int button) {
    uint32_t currentTime = HAL_GetTick();
    if (currentTime - lastButtonPressTime[button] > DEBOUNCE_DELAY) {
        lastButtonPressTime[button] = currentTime;
        return true;
    }
    return false;
}
void logNewTime() {
    if (tuningStatus == RED_ADJ) HAL_UART_Transmit(&huart2, (void*)str, sprintf(str, "!TIMER RED :%d#\r\n", timeRed), 500);
    if (tuningStatus == AMBER_ADJ) HAL_UART_Transmit(&huart2, (void*)str, sprintf(str, "!TIMER AMBER :%d#\r\n", timeAmber), 500);
    if (tuningStatus == GREEN_ADJ) HAL_UART_Transmit(&huart2, (void*)str, sprintf(str, "!TIMER GREEN :%d#\r\n", timeGreen), 500);
}

int initialTimeRed = 5;
int initialTimeAmber = 2;
int initialTimeGreen = 3;

void display_timerRed() {
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "TimeRED:%d", timeRed);
    lcd_goto_XY(1, 0);
    lcd_send_string(buffer);
}

void display_timerGreen() {
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "TimeGREEN:%d", timeGreen);
    lcd_goto_XY(1, 0);
    lcd_send_string(buffer);
}

void display_timerAmber() {
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "TimeAMBER:%d", timeAmber);
    lcd_goto_XY(1, 0);
    lcd_send_string(buffer);
}

void fsm_traffic_tunning_mode(void) {
    switch (tuningStatus) {
        case OFF:
            // do nothing
            break;
        case INIT:
            tuningStatus = RED_ADJ;
            clearRoadLed();
            logNewTime();
            settimer1(5, 5);
            break;
        case RED_ADJ:
            blinkyRed();
            if ((isButtonShortPressed(2) && isButtonDebounced(2)) ||receive_uart_command() == INCREMENT_RED_TIME) {
            	lcd_clear_display();
            	HAL_Delay(10);
                timeRed = (timeRed + 1) % 100;
                logNewTime();
                display_timerRed();
            } else if ((isButtonLongPressed(2) && isButtonDebounced(2))|| receive_uart_command() == DECREMENT_RED_TIME) {
            	lcd_clear_display();
            	HAL_Delay(10);
                timeRed = (timeRed - 1 + 100) % 100;
                logNewTime();
                display_timerRed();
            } else if ((isButtonShortPressed(1) && isButtonDebounced(1))|| receive_uart_command() == SWITCH) {
                tuningStatus = GREEN_ADJ;
                clearRoadLed();
                logNewTime();
                settimer1(6, 5);
                display_timerGreen();
            } else if ((isButtonShortPressed(0) && isButtonDebounced(0)) || receive_uart_command() == RESETALL) {
                // Reset all lights to initial times when button 1 is pressed
                timeRed = initialTimeRed;
                timeAmber = initialTimeAmber;
                timeGreen = initialTimeGreen;
                logNewTime();  // Send new time information
            	lcd_clear_display();
            	HAL_Delay(10);
                display_timerRed();
                tuningStatus = RED_ADJ;  // Keep state GREEN_ADJ
                clearRoadLed();  // Turn off all lights
            }
            break;
        case GREEN_ADJ:
            blinkyGreen();
            if ((isButtonShortPressed(2) && isButtonDebounced(2))||receive_uart_command() == INCREMENT_GREEN_TIME) {
            	lcd_clear_display();
            	HAL_Delay(10);
                timeGreen = (timeGreen + 1) % 100;
                logNewTime();
                display_timerGreen();
            } else if ((isButtonLongPressed(2) && isButtonDebounced(2))|| receive_uart_command() == DECREMENT_GREEN_TIME) {
            	lcd_clear_display();
            	HAL_Delay(10);
                timeGreen = (timeGreen - 1 + 100) % 100;
                logNewTime();
                display_timerGreen();
            } else if ((isButtonShortPressed(1) && isButtonDebounced(1))||receive_uart_command() == SWITCH) {
                tuningStatus = AMBER_ADJ;
                clearRoadLed();
                logNewTime();
                settimer1(7, 5);
                display_timerAmber();
            } else if ((isButtonShortPressed(0) && isButtonDebounced(0))|| receive_uart_command() == RESETALL) {
                // Reset all lights to initial times when button 1 is pressed
                timeRed = initialTimeRed;
                timeAmber = initialTimeAmber;
                timeGreen = initialTimeGreen;
                logNewTime();  // Send new time information
            	lcd_clear_display();
            	HAL_Delay(10);
                display_timerGreen();
                tuningStatus = GREEN_ADJ;  // Keep state GREEN_ADJ
                clearRoadLed();  // Turn off all lights
            }
            break;
        case AMBER_ADJ:
            blinkyAmber();
            if ((isButtonShortPressed(2) && isButtonDebounced(2))|| receive_uart_command() == INCREMENT_YELLOW_TIME) {
            	lcd_clear_display();
            	HAL_Delay(10);
                timeAmber = (timeAmber + 1) % 100;
                logNewTime();
                display_timerAmber();
            } else if ((isButtonLongPressed(2) && isButtonDebounced(2))|| receive_uart_command() == DECREMENT_YELLOW_TIME) {
            	lcd_clear_display();
            	HAL_Delay(10);
                timeAmber = (timeAmber - 1 + 100) % 100;
                logNewTime();
                display_timerAmber();
            } else if ((isButtonShortPressed(1) && isButtonDebounced(1))||receive_uart_command() == SWITCH) {
                tuningStatus = RED_ADJ;
                clearRoadLed();
                logNewTime();
                settimer1(5, 5);
                lcd_clear_display();
                HAL_Delay(50);
                display_timerRed();
            } else if ((isButtonShortPressed(0) && isButtonDebounced(0))|| receive_uart_command() == RESETALL) {
                // Reset all lights to initial times when button 1 is pressed
                timeRed = initialTimeRed;
                timeAmber = initialTimeAmber;
                timeGreen = initialTimeGreen;
                logNewTime();  // Send new time information
            	lcd_clear_display();
            	HAL_Delay(10);
                display_timerAmber();
                tuningStatus = AMBER_ADJ;  // Keep state GREEN_ADJ
                clearRoadLed();  // Turn off all lights
            }
            break;
        default:
            break;
    }
}

// ======================= CHUYỂN ĐỔI CHẾ ĐỘ =======================
void fsm_switch_mode(void) {
    if (isButtonLongPressed(1)|| (receive_uart_command() == SWITCH_TO_TUNING_MODE)	) {
        if (trafficMode != TUNING_MODE) {
            lcd_clear_display();
            HAL_Delay(10);
            display_timerRed();
            trafficMode = TUNING_MODE;
            HAL_UART_Transmit(&huart2, (void*)str, sprintf(str, "!SWITCH TO TUNING MODE#\r\n"), 500);
            // TODO: prepare for next mode
            tuningStatus = INIT;
            autoStatus = OFF;
            manualStatus = OFF;
        } else {
            lcd_clear_display();
            HAL_Delay(10);
            trafficMode = INIT;
            HAL_UART_Transmit(&huart2, (void*)str, sprintf(str, "!SWITCH TO AUTO MODE#\r\n"), 500);
        }
    }
}

// ======================= VÒNG LẶP CHÍNH =======================
void fsm_traffic(void) {
    switch (trafficMode) {
        case INIT:
            if (timeRed != (timeAmber + timeGreen)) {
                trafficMode = ERROR_MODE;
                HAL_UART_Transmit(&huart2, (void*)str, sprintf(str, "!TIMER ERROR#\r\n"), 500);
                lcd_clear_display();
                HAL_Delay(10);
                lcd_goto_XY(1, 0);
                lcd_send_string("ERROR, TIME IS");
                lcd_goto_XY(2, 0);
                lcd_send_string("NOT EQUAL");
                autoStatus = OFF;
                manualStatus = OFF;
                tuningStatus = OFF;
            } else {
                trafficMode = AUTO_MODE;
                autoStatus = INIT;
                manualStatus = OFF;
                tuningStatus = OFF;
//                settimer1(0, 1000);
            }
            turnOffAllLED();
            break;
        case AUTO_MODE:
            fsm_traffic_auto_mode();
            break;
        case TUNING_MODE:
            fsm_traffic_tunning_mode();
            break;
        default:
            break;
    }
}
