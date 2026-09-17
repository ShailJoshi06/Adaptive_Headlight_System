///*
// * ws2812b.h
// * Description: WS2812B LED Driver using STM32 HAL Timer & DMA
// */
//

#ifndef WS2812B_H_
#define WS2812B_H_

#include "main.h"

#define MAX_LED 10
#define USE_BRIGHTNESS 1

// 4 Strip Identifiers
#define STRIP_1 0
#define STRIP_2 1
#define STRIP_3 2
#define STRIP_4 3

// Added the 'strip' parameter to all functions
void WS2812_Set_LED(int strip, int LEDnum, int Red, int Green, int Blue);
void WS2812_Set_Brightness(int strip, int brightness);
void WS2812_Send(int strip);

#endif /* WS2812B_H_ */
