// Copyright 2023 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef BOARD_H_
#define BOARD_H_
#include <stdbool.h>

typedef enum 
{
    BOARD_LED_ALL_OFF = 0x00,
    BOARD_LED_RED =  0x01,
    BOARD_LED_YELLOW = 0x02,
    BOARD_LED_GREEN = 0x04,
    BOARD_LED_ALL_ON = BOARD_LED_RED | BOARD_LED_YELLOW | BOARD_LED_GREEN
}Led_ID;


void board_set_leds(Led_ID led_mask);


bool board_button_is_down(void);

#endif// BOARD_H_