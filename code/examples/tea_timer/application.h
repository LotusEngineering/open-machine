// Copyright 2023 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef APPLICATION_H_
#define APPLICATION_H_
#include "om.h"


// Start application, to be called by platform before running OS.
void application_start(int priority);

// Global trace instance
extern OmTrace application_trace;

#endif//APPLICATION_H_