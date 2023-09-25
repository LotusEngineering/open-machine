#ifndef SHARED_SIGNALS_H_
#define SHARED_SIGNALS_H_
#include "om_event.h"

// These signals are shared amongst actors
enum SharedSignals
{
    EVT_BUTTON_PRESS = OM_EVT_USER,
    EVT_BUTTON_RELEASE,
    EVT_BUTTON_HELD,
    EVT_BREW_STATUS, 
    EVT_BREW_COMPLETE,
    EVT_MAX_SHARED // Must be last
};

#endif