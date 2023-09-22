#ifndef APPLICATION_H_
#define APPLICATION_H_
#include "om.h"

// Start application, to be called by platform before running OS.
void application_start(int priority);

// Global trace instance
extern OmTrace application_trace;


#endif//APPLICATION_H_