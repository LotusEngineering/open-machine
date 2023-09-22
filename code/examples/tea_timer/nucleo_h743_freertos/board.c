#include "board.h"
#include "gpio.h"

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

bool board_button_is_down(void)
{
    return HAL_GPIO_ReadPin(USER_BUTTON_GPIO_Port, USER_BUTTON_Pin); 
}