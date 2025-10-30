#include "ws2812_demos.h"
#include "main.h"
#include <stdlib.h>
#include "ws2812.h"

#ifndef SRC_CONNECT_FOUR_H_
#define SRC_CONNECT_FOUR_H_
extern ws2812_handleTypeDef ws2812; // The WS2812 panel handler

void end1(ws2812_handleTypeDef *ws2812_49, ws2812_handleTypeDef *ws2812_256, TIM_HandleTypeDef *htim3);
void menu1(ws2812_handleTypeDef *ws2812_49, ws2812_handleTypeDef *ws2812_256, TIM_HandleTypeDef *htim3);
void start1(ws2812_handleTypeDef *ws2812_49, ws2812_handleTypeDef *ws2812_256, TIM_HandleTypeDef *htim3);
void start(ws2812_handleTypeDef *ws2812_49, ws2812_handleTypeDef *ws2812_256, TIM_HandleTypeDef *htim3);
void fall_animation(ws2812_handleTypeDef *ws2812);
void drop_disc(ws2812_handleTypeDef *ws2812);
void set_pos1(uint8_t* pos);
void connect_four_init();


#endif
