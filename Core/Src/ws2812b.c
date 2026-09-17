#include "ws2812b.h"
#include "math.h"

// Bring in both timers!
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;

#define WS2812_0 34
#define WS2812_1 67
#define RESET_PULSE_LEN 300

uint8_t LED_Data[4][MAX_LED][4];
uint8_t LED_Mod[4][MAX_LED][4];
uint32_t pwmData[4][(24 * MAX_LED) + RESET_PULSE_LEN];

volatile int datasentflag[4] = {0, 0, 0, 0};

void WS2812_Set_LED(int strip, int LEDnum, int Red, int Green, int Blue) {
    if (strip < 4 && LEDnum < MAX_LED) {
        LED_Data[strip][LEDnum][0] = LEDnum;
        LED_Data[strip][LEDnum][1] = Green;
        LED_Data[strip][LEDnum][2] = Red;
        LED_Data[strip][LEDnum][3] = Blue;
    }
}

void WS2812_Set_Brightness(int strip, int brightness) {
#if USE_BRIGHTNESS
    if (brightness > 45) brightness = 45;
    for (int i = 0; i < MAX_LED; i++) {
        LED_Mod[strip][i][0] = LED_Data[strip][i][0];
        for (int j = 1; j < 4; j++) {
            float angle = 90 - brightness;
            angle = angle * M_PI / 180;
            LED_Mod[strip][i][j] = (LED_Data[strip][i][j]) / (tan(angle));
        }
    }
#endif
}

void WS2812_Send(int strip) {
    uint32_t indx = 0;
    uint32_t color;

    for (int i = 0; i < MAX_LED; i++) {
#if USE_BRIGHTNESS
        color = ((LED_Mod[strip][i][1] << 16) | (LED_Mod[strip][i][2] << 8) | (LED_Mod[strip][i][3]));
#else
        color = ((LED_Data[strip][i][1] << 16) | (LED_Data[strip][i][2] << 8) | (LED_Data[strip][i][3]));
#endif
        for (int j = 23; j >= 0; j--) {
            if (color & (1UL << j)) {
                pwmData[strip][indx] = WS2812_1;
            } else {
                pwmData[strip][indx] = WS2812_0;
            }
            indx++;
        }
    }

    for (int i = 0; i < RESET_PULSE_LEN; i++) {
        pwmData[strip][indx] = 0;
        indx++;
    }

    // Trigger the correct DMA Channel based on the strip
        uint32_t tickstart = HAL_GetTick();

        if (strip == STRIP_1) {
            HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_1, pwmData[STRIP_1], indx);
            while (!datasentflag[STRIP_1] && (HAL_GetTick() - tickstart < 5)) {};
            datasentflag[STRIP_1] = 0;
        }
        else if (strip == STRIP_2) {
            HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_2, pwmData[STRIP_2], indx);
            while (!datasentflag[STRIP_2] && (HAL_GetTick() - tickstart < 5)) {};
            datasentflag[STRIP_2] = 0;
        }
        else if (strip == STRIP_3) {
            HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_1, pwmData[STRIP_3], indx);
            while (!datasentflag[STRIP_3] && (HAL_GetTick() - tickstart < 5)) {};
            datasentflag[STRIP_3] = 0;
        }
        else if (strip == STRIP_4) {
            // MAKE SURE THIS SAYS TIM_CHANNEL_3, NOT CHANNEL_2!
            HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_3, pwmData[STRIP_4], indx);
            while (!datasentflag[STRIP_4] && (HAL_GetTick() - tickstart < 5)) {};
            datasentflag[STRIP_4] = 0;
        }
}

// --- INTERRUPT HANDLER FOR BOTH TIMERS ---
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {
        if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
            HAL_TIM_PWM_Stop_DMA(&htim2, TIM_CHANNEL_1);
            datasentflag[STRIP_1] = 1;
        }
        else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {
            HAL_TIM_PWM_Stop_DMA(&htim2, TIM_CHANNEL_2);
            datasentflag[STRIP_2] = 1;
        }
    }
    else if (htim->Instance == TIM3) {
        if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
            HAL_TIM_PWM_Stop_DMA(&htim3, TIM_CHANNEL_1);
            datasentflag[STRIP_3] = 1;
        }
        else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3) {
            HAL_TIM_PWM_Stop_DMA(&htim3, TIM_CHANNEL_2);
            datasentflag[STRIP_4] = 1;
        }
    }
}
