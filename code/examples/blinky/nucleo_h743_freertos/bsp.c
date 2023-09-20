#include "bsp.h"
#include "gpio.h"

void bsp_set_led_on(Led_ID led)
{
    switch(led)
    {
        case BSP_LED_RED:
            HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, 1);
        break;
        case BSP_LED_YELLOW:
            HAL_GPIO_WritePin(LED_YELLOW_GPIO_Port, LED_YELLOW_Pin, 1);
        break;
        case BSP_LED_GREEN:
            HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, 1);
        break;
    }
}

void bsp_set_led_off(Led_ID led)
{
    switch(led)
    {
        case BSP_LED_RED:
            HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, 0);
        break;
        case BSP_LED_YELLOW:
            HAL_GPIO_WritePin(LED_YELLOW_GPIO_Port, LED_YELLOW_Pin, 0);
        break;
        case BSP_LED_GREEN:
            HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, 0);
        break;
    }
}