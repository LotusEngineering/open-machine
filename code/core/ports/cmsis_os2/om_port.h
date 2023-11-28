// Copyright 2023 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OM_PORT_PORT_H_
#define OM_PORT_PORT_H_
#include "cmsis_os2.h"

/// Define mutex structure
typedef osMutexId_t  OmMutexPort;

/// @brief Actor Port definition for CMSIS RTOS API V2
typedef struct OmActorPort
{
    osThreadId_t thread_id;
    osThreadAttr_t thread_attr;
    osMessageQueueId_t queue_id;
}OmActorPort;

/// @brief Memory Pool Port definition for CMSIS RTOS API V2
typedef struct OmPoolPort
{
    osMemoryPoolId_t pool_id;
    size_t block_size;
}OmPoolPort;

#endif //OM_PORT_PORT_H_