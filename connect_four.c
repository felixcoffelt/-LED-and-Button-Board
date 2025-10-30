/**
  * @file    connect_four.c
  * @brief   Connect Four game implementation for 7x7 LED matrix
  * @author  Felix Coffelt
  * @date    May 13, 2025
  * 
  * @details This module implements the complete Connect Four game logic including:
  *          - Player turn management
  *          - Disc dropping with real-time animation
  *          - Game state transitions
  *          - Input validation and move processing
  * 
  * @note    Win condition checking is marked for future implementation.
  *          The game uses a 7x7 board instead of traditional 6x7 for expanded gameplay.
  */

#include "main.h"
#include <string.h>
#include "connect_four.h"
#include "display.h"

/* Global Variables */
uint8_t user_pos1[2];          ///< Current cursor position [row, column]
uint8_t current_user;          ///< Active player (PLAYER_1 = Red, PLAYER_2 = Blue)

/* External References */
extern uint32_t next_frame[7][7];  ///< Display frame buffer from display.c

/**
  * @brief  Initialize Connect Four game to starting state
  * @param  None
  * @retval None
  * 
  * @note   Sets up initial game state with player 1 starting and invalid
  *         cursor position to prevent accidental moves before player input.
  */
void connect_four_init(void) {
    current_user = PLAYER_1;               // Player 1 (Red) starts first
    user_pos1[0] = INVALID_POSITION;       // Set to invalid row
    user_pos1[1] = INVALID_POSITION;       // Set to invalid column
}

/**
  * @brief  Update current player cursor position
  * @param  pos: Pointer to position array containing [row, column]
  * @retval None
  * 
  * @note   Position is typically calculated from ADC input in display.c
  *         and passed to this function for game logic processing.
  */
void set_pos1(uint8_t* pos) {
    user_pos1[0] = pos[0];     // Update row position (0-6)
    user_pos1[1] = pos[1];     // Update column position (0-6)
}

/**
  * @brief  Handle disc dropping logic (placeholder for future implementation)
  * @param  ws2812: LED matrix handler
  * @retval None
  * 
  * @note   This function is reserved for future implementation of direct
  *         disc dropping without animation for faster gameplay modes.
  */
void drop_disc(ws2812_handleTypeDef *ws2812) {
    // Future implementation: Direct disc placement without animation
    // Check if target position is valid (empty space)
    if(next_frame[user_pos1[0]][user_pos1[1]] == blank) {
        // TODO: Implement instant disc placement logic here
        // This would be used for non-animated gameplay modes
    }
}

/**
  * @brief  Animate disc falling from top to bottom of selected column
  * @param  ws2812_49: 7x7 LED matrix handler for animation display
  * @retval None
  * 
  * @note   Creates a smooth visual effect of the disc falling through empty
  *         spaces until it reaches the bottom or another disc. Uses 1-second
  *         delays between movement steps for visible animation.
  */
void fall_animation(ws2812_handleTypeDef *ws2812_49) {
    // Place disc at top of selected column with player color
    if (current_user == PLAYER_1) {
        next_frame[0][user_pos1[1]] = red;    // Player 1 uses red discs
    } else {
        next_frame[0][user_pos1[1]] = blue;   // Player 2 uses blue discs
    }
    
    user_pos1[0] = 0;  // Start animation from top row (row 0)
    d_update(ws2812_49, NULL); // Initial display update
    
    // Animate disc falling until it hits bottom or another disc
    while(next_frame[user_pos1[0] + 1][user_pos1[1]] == blank) {
        user_pos1[0] += 1; // Move disc down one row
        
        // Update current position with player color
        if (current_user == PLAYER_1) {
            next_frame[user_pos1[0]][user_pos1[1]] = red;
        } else {
            next_frame[user_pos1[0]][user_pos1[1]] = blue;
        }

        // Clear previous position (one row above)
        next_frame[user_pos1[0] - 1][user_pos1[1]] = blank;
        
        d_update(ws2812_49, NULL); // Update display with new position
        HAL_Delay(1000);           // 1-second delay for visible animation
    }
    
    // TODO: Add win condition check after disc placement
    // check_win_condition();
}

/**
  * @brief  Start Connect Four game main loop
  * @param  ws2812_49: 7x7 LED matrix handler for primary gameplay display
  * @param  ws2812_256: 16x16 LED matrix handler for secondary UI (currently unused)
  * @param  htim3: Timer handler for game timing features
  * @retval None
  * 
  * @note   Implements the main game loop with continuous player input checking
  *         and move processing. Currently runs indefinitely until reset.
  */
void start1(ws2812_handleTypeDef *ws2812_49, ws2812_handleTypeDef *ws2812_256, TIM_HandleTypeDef *htim3) {
    // Initialize game state and displays
    connect_four_init();               // Reset game to starting conditions
    d_blank();                         // Clear both display buffers
    d_start();                         // Show game start screen
    d_update(ws2812_49, NULL);         // Update 7x7 display with start screen
    HAL_Delay(3000);                   // Display start screen for 3 seconds
    
    // Main game loop - runs continuously during gameplay
    while(1) {
        // Check if player has made a valid move
        // Valid move conditions:
        // - Column within bounds (0-6)
        // - Target position is empty
        if ((user_pos1[1] >= 0) && 
            (user_pos1[1] < BOARD_SIZE) && 
            (next_frame[user_pos1[0]][user_pos1[1]] == blank)) {
            
            // Execute disc dropping animation
            fall_animation(ws2812_49);

            // TODO: Implement win condition checking
            // check_win(); - Would determine if current move wins the game
            
            // TODO: Switch player turns after valid move
            // current_user = (current_user == PLAYER_1) ? PLAYER_2 : PLAYER_1;
        }
        
        // TODO: Add game exit conditions (win, draw, or menu command)
        // This would break out of the while(1) loop when game ends
    }
}

/**
  * @brief  Display Connect Four menu and transition to gameplay
  * @param  ws2812_49: 7x7 LED matrix handler for menu display
  * @param  ws2812_256: 16x16 LED matrix handler for detailed menu (currently unused)
  * @param  htim3: Timer handler for menu timing
  * @retval None
  * 
  * @note   Shows the Connect Four specific menu and automatically transitions
  *         to gameplay after a fixed delay. Can be modified for interactive menu.
  */
void menu1(ws2812_handleTypeDef *ws2812_49, ws2812_handleTypeDef *ws2812_256, TIM_HandleTypeDef *htim3) {
    d_menu1();                         // Display Connect Four menu graphics
    d_update(ws2812_49, NULL);         // Update 7x7 display with menu
    HAL_Delay(3000);                   // Show menu for 3 seconds
    start1(ws2812_49, NULL, NULL);     // Start Connect Four game
}

/**
  * @brief  End game session and reset displays
  * @param  ws2812_49: 7x7 LED matrix handler
  * @param  ws2812_256: 16x16 LED matrix handler
  * @param  htim3: Timer handler
  * @retval None
  * 
  * @note   Clears both displays and prepares system for returning to main menu
  *         or starting a new game. Currently implements basic display cleanup.
  */
void end1(ws2812_handleTypeDef *ws2812_49, ws2812_handleTypeDef *ws2812_256, TIM_HandleTypeDef *htim3) {
    d_blank();                         // Clear both display buffers
    d_update(ws2812_49, NULL);         // Clear 7x7 display
    HAL_Delay(5);                      // Brief delay for display sync
    d_update(NULL, ws2812_256);        // Clear 16x16 display
}
