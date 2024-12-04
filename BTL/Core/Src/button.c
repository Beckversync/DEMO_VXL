#include "button.h"

int KeyReg0[NUM_BUTTONS] = {NORMAL_STATE};
int KeyReg1[NUM_BUTTONS] = {NORMAL_STATE};
int KeyReg2[NUM_BUTTONS] = {NORMAL_STATE};
int KeyReg3[NUM_BUTTONS] = {NORMAL_STATE};

int TimeOutForKeyPress[NUM_BUTTONS] = {1000};
int button_pressed[NUM_BUTTONS] = {0};
int button_long_pressed[NUM_BUTTONS] = {0};
int button_flag[NUM_BUTTONS] = {0};

int isButtonShortPressed(int button_index){
    if(button_flag[button_index] == 1){
        button_flag[button_index] = 0;
        return 1;
    }
    return 0;
}

int isButtonLongPressed(int button_index){
    if(button_long_pressed[button_index] == 1){
        button_long_pressed[button_index] = 0;
        return 1;
    }
    return 0;
}

void subKeyProcess(int button_index) {
    switch (button_index) {
        case 0:
            break;
        case 1:
            break;
        case 2:
            break;
    }
    button_flag[button_index] = 1;
}

void getKeyInput() {
    KeyReg2[0] = KeyReg1[0];
    KeyReg1[0] = KeyReg0[0];
    KeyReg2[1] = KeyReg1[1];
    KeyReg1[1] = KeyReg0[1];
    KeyReg2[2] = KeyReg1[2];
    KeyReg1[2] = KeyReg0[2];

    KeyReg0[0] = HAL_GPIO_ReadPin(BUTTON1_GPIO_Port, BUTTON1_Pin);
    KeyReg0[1] = HAL_GPIO_ReadPin(BUTTON2_GPIO_Port, BUTTON2_Pin);
    KeyReg0[2] = HAL_GPIO_ReadPin(BUTTON3_GPIO_Port, BUTTON3_Pin);

    for (int i = 0; i < NUM_BUTTONS; i++) {
        if ((KeyReg1[i] == KeyReg0[i]) && (KeyReg1[i] == KeyReg2[i])) {
            if (KeyReg2[i] != KeyReg3[i]) {
                KeyReg3[i] = KeyReg2[i];

                if (KeyReg3[i] == PRESSED_STATE) {
                    TimeOutForKeyPress[i] = 1000;
                    subKeyProcess(i);
                }
            } else {
                TimeOutForKeyPress[i]--;
                if (TimeOutForKeyPress[i] == 0) {
                    TimeOutForKeyPress[i] = 1000;
                    if (KeyReg3[i] == PRESSED_STATE) {
                        button_long_pressed[i] = 1;
                        button_flag[i] = 0; // Ensure short press is not set
                    }
                }
            }
        }
    }
}
