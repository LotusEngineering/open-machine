#ifndef BOARD_H_
#define BOARD_H_
#include <stdbool.h>

typedef enum 
{
    BOARD_LED_RED,
    BOARD_LED_YELLOW,
    BOARD_LED_GREEN,
}Led_ID;


void board_set_led_on(Led_ID led);

void board_set_led_off(Led_ID led);

bool board_button_is_down(void);

#endif// BOARD_H_