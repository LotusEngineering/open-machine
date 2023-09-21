#ifndef BSP_H_
#define BSP_H_

typedef enum 
{
    BSP_LED_RED,
    BSP_LED_YELLOW,
    BSP_LED_GREEN,
}Led_ID;


void bsp_set_led_on(Led_ID led);

void bsp_set_led_off(Led_ID led);



#endif// BSP_H_