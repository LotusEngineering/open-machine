#ifndef APPLICATION_H_
#define APPLICATION_H_
#include "om.h"


// These signals are shared amongst actors
enum SharedSignals
{
    EVT_BUTTON_PRESS = OM_EVT_USER,
    EVT_BUTTON_RELEASE,
    EVT_BUTTON_HELD,
    EVT_MAX_SHARED // Must be last
};


// Start application, to be called by platform before running OS.
void application_start(int priority);

// Global trace instance
extern OmTrace application_trace;

#endif//APPLICATION_H_