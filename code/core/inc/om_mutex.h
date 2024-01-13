// Copyright 2023 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OM_MUTEX_H_
#define OM_MUTEX_H_
#include "om_port.h"

/// @brief Mutex data type
typedef struct OmMutex_t
{
    OmMutexPort port;
}OmMutex;

/// @brief Initializes a mutex
/// @param mutex 
void om_mutex_init(OmMutex* mutex);

/// @brief Locks a mutex, blocks if already locked
/// @param mutex 
void om_mutex_lock(OmMutex* mutex);

/// @brief Unlocks a mutex
/// @param mutex 
void om_mutex_unlock(OmMutex* mutex);


#endif //OM_MUTEX_H_