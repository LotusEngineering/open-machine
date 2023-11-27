#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include "om_mutex.h"

#define RED_MASK 0x01
#define YELLOW_MASK 0x02
#define GREEN_MASK 0x04


static unsigned char led_state;
static unsigned char led_state_last;
static OmMutex led_mutex;


void board_init(void)
{
    // Create a mutex since we are sharing a resource and state between actors
    // This is normally a bad pattern as only one actor should own a resource
    om_mutex_init(&led_mutex);    
}

void board_show_led_state_(void)
{
    switch(led_state)
    {
        case 0x00:
            printf("...\n");
        break;
        case RED_MASK:
            printf("R..\n");
        break;
        case YELLOW_MASK:
            printf(".Y.\n");
        break;
        case GREEN_MASK:
            printf("..G\n");
        break;
        case RED_MASK | YELLOW_MASK:
            printf("RY.\n");
        break;
        case RED_MASK | GREEN_MASK:
            printf("R.G\n");
        break;
        case YELLOW_MASK | GREEN_MASK:
            printf(".YG\n");
        break;
        case RED_MASK | YELLOW_MASK | GREEN_MASK:
            printf("RYG\n");
        break;
        default:
            printf("BADLED STATE\n");
        break;
    }

    
}

void board_set_led_on(Led_ID led)
{
    om_mutex_lock(&led_mutex);
    switch(led)
    {
        case BOARD_LED_RED:
            led_state |= RED_MASK;
        break;
        case BOARD_LED_YELLOW:
            led_state |= YELLOW_MASK;
        break;
        case BOARD_LED_GREEN:
            led_state |= GREEN_MASK;
        break;
    }
    if(led_state != led_state_last)
        board_show_led_state_();
    led_state_last = led_state;
    om_mutex_unlock(&led_mutex);
}

void board_set_led_off(Led_ID led)
{
    om_mutex_lock(&led_mutex);

    switch(led)
    {
        case BOARD_LED_RED:
            led_state &= ~RED_MASK;
        break;
        case BOARD_LED_YELLOW:
            led_state &= ~YELLOW_MASK;
        break;
        case BOARD_LED_GREEN:
            led_state &= ~GREEN_MASK;
        break;
    }
    if(led_state != led_state_last)
        board_show_led_state_();
    led_state_last = led_state;
    om_mutex_unlock(&led_mutex);
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
