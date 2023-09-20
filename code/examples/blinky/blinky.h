#ifndef BLINKY_H_
#define BLINKY_H_
#include "om.h"


// The machine under test
typedef struct 
{
    OmActor base;
    OmTimer timer;
}Blinky;

void blinky_ctor(Blinky* self, OmTrace* trace);

// Trace instance
extern OmTrace trace;

// Blinky main, to be called by platform before running OS.
void blinky_init(void);

#endif // BLINKY_H_