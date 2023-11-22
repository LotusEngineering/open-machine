#include "board.h"

void board_set_led_on(Led_ID led)
{
    switch(led)
    {
        case BOARD_LED_RED:
            printf("RED LED On\n");
        break;
        case BOARD_LED_YELLOW:
            printf("Yellow LED On\n");
        break;
        case BOARD_LED_GREEN:
            printf("Green LED On\n");
        break;
    }
}

void board_set_led_off(Led_ID led)
{
    switch(led)
    {
        case BOARD_LED_RED:
            printf("RED LED Off\n");
        break;
        case BOARD_LED_YELLOW:
            printf("Yellow LED Off\n");
        break;
        case BOARD_LED_GREEN:
            printf("Green LED Off\n");
        break;
    }
}