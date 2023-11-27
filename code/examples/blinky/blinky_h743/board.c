#include "board.h"
#include "gpio.h"
#include "om_trace.h"

void board_init(void)
{
    
}

void board_set_led_on(Led_ID led)
{
    switch(led)
    {
        case BOARD_LED_RED:
            HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, 1);
        break;
        case BOARD_LED_YELLOW:
            HAL_GPIO_WritePin(LED_YELLOW_GPIO_Port, LED_YELLOW_Pin, 1);
        break;
        case BOARD_LED_GREEN:
            HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, 1);
        break;
    }
}

void board_set_led_off(Led_ID led)
{
    switch(led)
    {
        case BOARD_LED_RED:
            HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, 0);
        break;
        case BOARD_LED_YELLOW:
            HAL_GPIO_WritePin(LED_YELLOW_GPIO_Port, LED_YELLOW_Pin, 0);
        break;
        case BOARD_LED_GREEN:
            HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, 0);
        break;
    }
}

void om_assert_handler(const char *file_name, int line)
{
    OM_TRACE_TWO(&application_trace, "ASSERT!!! File", file_name);
    OM_TRACE_VAL_DEC(&application_trace, line);
    board_set_led_on(BOARD_LED_RED);
    board_set_led_on(BOARD_LED_GREEN);
    board_set_led_on(BOARD_LED_YELLOW);

    while(1)
    {
        continue;
    }
}
