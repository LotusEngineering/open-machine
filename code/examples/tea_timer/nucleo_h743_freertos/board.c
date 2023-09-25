#include "board.h"
#include "gpio.h"


void board_set_leds(Led_ID led_mask)
{
    HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, led_mask & BOARD_LED_RED);
    HAL_GPIO_WritePin(LED_YELLOW_GPIO_Port, LED_YELLOW_Pin, led_mask & BOARD_LED_YELLOW);
    HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, led_mask & BOARD_LED_GREEN);

}


bool board_button_is_down(void)
{
    return HAL_GPIO_ReadPin(USER_BUTTON_GPIO_Port, USER_BUTTON_Pin); 
}