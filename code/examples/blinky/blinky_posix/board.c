#include <stdio.h>
#include <stdlib.h>
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

void om_assert_handler(const char *file_name, int line)
{
    printf("ASSERT!!! File: %s, Line:%d \n ", file_name, line);
    exit(-1);
    while(1)
    {
        continue;
    }
}
