#ifndef SRC_DISPLAY_H_
#define SRC_DISPLAY_H_

void d_setVariables(uint8_t matrixNum, TIM_HandleTypeDef *timer, uint32_t channel);
void calc_pos(uint32_t value_adc1, uint32_t value_adc2);
void d_blank(void);
void d_score(void);
void d_menu(void);
void d_start();
void d_update(ws2812_handleTypeDef *ws2812_49, ws2812_handleTypeDef *ws2812_256);
void d_menu1(void);

#endif
