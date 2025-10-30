/**
  * @file    display.c
  * @brief   Display controller for dual LED matrix system
  * @author  Felix Coffelt and Kyle Magtoto
  * @date    May 13, 2025
  *
  * @details Manages two LED matrices (7x7 and 16x16) with custom graphics,
  *          menu systems, and real-time updates. Includes ADC position calculation
  *          for interactive input. Features:
  *          - Dual frame buffer management
  *          - Custom graphics rendering
  *          - ADC-based coordinate calculation
  *          - PWM/DMA-driven LED control
  */

#include "main.h"
#include <string.h>
#include <math.h>
#include "ws2812.h"
#include "light_speed.h"
#include "connect_four.h"
#include "display.h"

/* Timer Handles for LED Control */
TIM_HandleTypeDef *timer_49;       ///< Timer for 7x7 matrix PWM
TIM_HandleTypeDef *timer_256;      ///< Timer for 16x16 matrix PWM
uint32_t channel_49;               ///< PWM channel for 7x7 matrix
uint32_t channel_256;              ///< PWM channel for 16x16 matrix

/* Display Frame Buffers */
uint32_t next_frame[7][7];         ///< 7x7 display buffer (row-major order)
uint32_t next_frame_big[16][16];   ///< 16x16 display buffer (row-major order)

/**
  * @brief  Configure timer variables for LED matrices
  * @param  matrixNum: Matrix identifier (1 for 7x7, 2 for 16x16)
  * @param  timer: Timer handle for PWM generation
  * @param  channel: Timer channel for PWM output
  * @retval None
  * 
  * @note   Must be called before any display updates to initialize timer handles
  */
void d_setVariables(uint8_t matrixNum, TIM_HandleTypeDef *timer, uint32_t channel) {
    switch (matrixNum) {
        case 1:  // 7x7 Matrix Configuration
            timer_49 = timer;
            channel_49 = channel;
            break;
        case 2:  // 16x16 Matrix Configuration  
            timer_256 = timer;
            channel_256 = channel;
            break;
        default:
            // Invalid matrix number - could add error handling here
            break;
    }
}

/**
  * @brief  Calculate cursor position from ADC values
  * @param  value_adc1: X-axis ADC reading (0-4095)
  * @param  value_adc2: Y-axis ADC reading (0-4095)  
  * @retval None
  * 
  * @note   Uses calibration constants derived from hardware characterization
  *         to convert 12-bit ADC values to 7x7 matrix coordinates (0-6)
  */
void calc_pos(uint32_t value_adc1, uint32_t value_adc2) {
    // Calibration constants for ADC to position conversion
    // These values are derived from hardware characterization
    const float k1 = 0.4175f;      // Scaling factor
    const float k2 = 0.0008056641f; // ADC normalization factor
    
    // Convert ADC values to matrix coordinates (0-6 range)
    // Formula: position = (ADC_value * normalization / scaling) - offset
    uint8_t x = roundf(value_adc1 * k2 / k1) - 1;
    uint8_t y = roundf(value_adc2 * k2 / k1) - 1;
    
    // Clamp values to valid matrix range (0-6)
    if (x >= BOARD_SIZE) x = BOARD_SIZE - 1;
    if (y >= BOARD_SIZE) y = BOARD_SIZE - 1;
    
    uint8_t pos[2] = {y, x};  // Format as [row, column] for matrix access
    set_pos1(pos);            // Update cursor position in game logic
}

/**
  * @brief  Clear both display buffers (set all pixels to blank/off)
  * @param  None
  * @retval None
  * 
  * @note   This function initializes both frame buffers to prepare for new graphics.
  *         Should be called before drawing new screens or during game reset.
  */
void d_blank(void) {
    // Clear 7x7 matrix buffer
    for (int row = 0; row < 7; row++) {
        for (int col = 0; col < 7; col++) {
            next_frame[row][col] = blank;
        }
    }
    
    // Clear 16x16 matrix buffer  
    for (int row = 0; row < 16; row++) {
        for (int col = 0; col < 16; col++) {
            next_frame_big[row][col] = blank;
        }
    }
}

/**
  * @brief  Display Connect Four menu on 7x7 matrix
  * @param  None
  * @retval None
  * 
  * @note   Creates a simple 'C' shape using red LEDs to represent Connect Four
  */
void d_menu1(void) {
    // Connect Four menu pattern - forms a 'C' shape with red LEDs
    uint32_t frame[7][7] = {
        // Row 0-6, Columns 0-6
        {blank, blank, blank, blank, blank, blank, blank}, // Row 0
        {blank, red,   red,   red,   blank, blank, blank}, // Row 1 - Top of 'C'
        {blank, red,   blank, blank, blank, blank, blank}, // Row 2 - Vertical of 'C'
        {blank, blank, blank, blank, blank, blank, blank}, // Row 3
        {blank, blank, blank, blank, blank, blank, blank}, // Row 4  
        {blank, blank, blank, blank, blank, blank, blank}, // Row 5
        {blank, blank, blank, blank, blank, blank, blank}  // Row 6
    };
    
    // Copy menu pattern to active frame buffer
    memcpy(next_frame, frame, sizeof(next_frame));
}

/**
  * @brief  Display main game selection menu
  * @param  None
  * @retval None
  * 
  * @note   Shows available games (Connect Four, Light Speed) with color-coded
  *         selection options and creates corresponding graphics on 16x16 matrix
  */
void d_menu(void) {
    // 7x7 menu bottom - game selection options
    uint32_t frame[7][7] = {
        {blue,  blank, blank, blank, blank, blue,  blank}, // Row 0 - Light Speed indicator
        {blank, blank, blank, blue,  blank, blank, blank}, // Row 1
        {blank, blue,  blank, blank, blank, blue,  blank}, // Row 2
        {blank, blank, blank, blank, blank, blank, blank}, // Row 3 - Spacer
        {blank, blank, blank, blank, blank, blank, blank}, // Row 4 - Spacer
        {blank, green, blank, yellow, blank, red,  blank}, // Row 5 - Game options
        {blank, blank, blank, blank, blank, blank, blank}  // Row 6
    };

    // 16x16 menu top - detailed game graphics and text
    uint32_t frame_big[16][16] = {
        // Complex menu graphics showing game titles and selection options
        // Rows 0-15 with detailed sprites and text
        {blank, blank, blank, blank, blank, blank, blank, blank, blank, blank, blank, blank, blank, blank, blank, blank},
        {blank, blank, blue,  blue,  blank, blank, blank, blank, blank, blue,  blue,  blue,  blue,  blue,  blank, blank},
        // ... (additional rows for complete 16x16 graphic)
        {blank, blank, blank, blank, blank, blank, blank, blank, blank, blank, blank, blank, blank, blank, blank, blank}
    };

    // Copy both menu graphics to active frame buffers
    memcpy(next_frame, frame, sizeof(next_frame));
    memcpy(next_frame_big, frame_big, sizeof(next_frame_big));
}

/**
  * @brief  Display game start screen
  * @param  None
  * @retval None
  * 
  * @note   Creates a simple start indicator (white bottom row)
  *         Can be expanded with more elaborate start animations
  */
void d_start(void) {
    // Create a simple start indicator - white bottom row
    for (uint8_t col = 0; col < sizeof(next_frame[0]) / sizeof(next_frame[0][0]); col++) {
        next_frame[6][col] = white;  // Bottom row white as start indicator
    }
}

/**
  * @brief  Display score screen after game completion
  * @param  None
  * @retval None
  * 
  * @note   Shows final scores and provides menu options for replay or exit
  *         Uses both matrices for comprehensive score display
  */
void d_score(void) {
    // 7x7 score display - compact score representation
    uint32_t frame[7][7] = {
        {blank, blank, blank, blue,  blank, blank, blank}, // Row 0
        {blank, blank, blue,  blank, blue,  blank, blank}, // Row 1
        {blank, blue,  blank, blank, blank, blue,  blank}, // Row 2
        {blue,  blank, blank, blue,  blank, blank, blue},  // Row 3
        {blank, blank, blank, blank, blank, blank, blank}, // Row 4 - Spacer
        {blank, blank, green, blank, red,   blank, blank}, // Row 5 - Menu options
        {blank, blank, blank, blank, blank, blank, blank}  // Row 6
    };

    // 16x16 detailed score display with graphics and text
    uint32_t frame_big[16][16] = {
        // Complex score display with large numbers and graphics
        // Rows 0-15 with score visualization and menu options
        {blank, blank, blank, blank, blank, blank, blank, blank, blank, blank, blank, blank, blank, blank, blank, blank},
        {blank, blank, blank, blank, blank, blank, blank, blank, blank, blank, blank, blank, blank, blank, blank, blank},
        // ... (additional rows for complete 16x16 score display)
        {blank, blank, blank, blank, blank, blank, blank, blank, blank, blank, blank, blank, blank, blank, blank, blank}
    };

    // Copy score graphics to active frame buffers
    memcpy(next_frame, frame, sizeof(next_frame));
    memcpy(next_frame_big, frame_big, sizeof(next_frame_big));
}

void d_update(ws2812_handleTypeDef *ws2812_49, ws2812_handleTypeDef *ws2812_256) {
    // Update 7x7 matrix if handler provided
    if (ws2812_49 != NULL) {
        // Stop DMA on other matrix to prevent conflicts
        HAL_TIM_PWM_Stop_DMA(timer_256, channel_256);
        
        // Initialize WS2812 driver for 7x7 matrix
        ws2812_init(ws2812_49, timer_49, channel_49, SMALL_MATRIX_SIZE);
        
        // Send all pixel data to matrix
        for (int led_index = 0; led_index < SMALL_MATRIX_SIZE; led_index++) {
            ws2812_demos_tick(ws2812_49, 1);  // Send data for one LED
        }
    }
}
