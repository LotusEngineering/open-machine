#ifndef BLINKY_H_
#define BLINKY_H_
#include <stdint.h>
#include "om.h"
#include "board.h"

// The machine under test
typedef struct 
{
    OmActor base;
    OmTimer timer;
    Led_ID  led_id;
    uint32_t blink_time_ms;
}Blinky;

void blinky_ctor(Blinky* self, const char* name, OmTrace* trace, Led_ID led_id, uint32_t blink_time_ms);


#endif // BLINKY_H_