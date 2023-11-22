// Copyright 2023 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.
#ifndef OM_MUTEX_H_
#define OM_MUTEX_H_
#include "om_mutex_port.h"

typedef struct
{
    OmMutexPort port;
}OmMutex;

void om_mutex_init(OmMutex* mutex);

void om_mutex_lock(OmMutex* mutex);

void om_mutex_unlock(OmMutex* mutex);


#endif //OM_MUTEX_H_