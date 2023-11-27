// Copyright 2023 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef BOARD_H_
#define BOARD_H_

typedef enum 
{
    BOARD_LED_RED,
    BOARD_LED_YELLOW,
    BOARD_LED_GREEN,
}Led_ID;

void board_init(void);

void board_set_led_on(Led_ID led);

void board_set_led_off(Led_ID led);

#endif// BOARD_H_