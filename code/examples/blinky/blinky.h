#ifndef BLINKY_H_
#define BLINKY_H_
#include <stdint.h>
#include "om.h"
#include "bsp.h"

// The machine under test
typedef struct 
{
    OmActor base;
    OmTimer timer;
    Led_ID  led_id;
    uint32_t blink_time_ms;
}Blinky;

void blinky_ctor(Blinky* self, const char* name, OmTrace* trace, Led_ID led_id, uint32_t blink_time_ms);

// Trace instance
extern OmTrace blinky_trace;

// Blinky demo start, to be called by platform before running OS.
void blinky_actors_start(void);

#endif // BLINKY_H_