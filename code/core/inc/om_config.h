// Copyright 2023 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OM_CONFIG_H_
#define OM_CONFIG_H_

#include <stddef.h>

#ifndef OM_TRACE_MAX_MESSAGE_LENGTH
    #define OM_TRACE_MAX_MESSAGE_LENGTH 80
#endif

#ifndef OM_ACTOR_MAX_ACTORS
    #define OM_ACTOR_MAX_ACTORS 16
#endif

#ifndef OM_MACHINE_MAX_STATE_DEPTH
    #define OM_MACHINE_MAX_STATE_DEPTH 6
#endif

#ifndef OM_POOL_NUM_POOLS
    #define OM_POOL_NUM_POOLS 3
#endif

#ifndef OM_TIMER_MAX_TIMERS
    #define OM_TIMER_MAX_TIMERS 16
#endif

#ifndef OM_POOL_SMALL_BLOCK_SIZE 
    #define OM_POOL_SMALL_BLOCK_SIZE  sizeof(uint32_t) * 4
#endif
#ifndef OM_POOL_SMALL_BLOCK_COUNT 
    #define OM_POOL_SMALL_BLOCK_COUNT  16
#endif


#ifndef OM_POOL_MEDIUM_BLOCK_SIZE 
    #define OM_POOL_MEDIUM_BLOCK_SIZE  sizeof(uint32_t) * 8
#endif
#ifndef OM_POOL_MEDIUM_BLOCK_COUNT 
    #define OM_POOL_MEDIUM_BLOCK_COUNT  8
#endif

#ifndef OM_POOL_LARGE_BLOCK_SIZE 
    #define OM_POOL_LARGE_BLOCK_SIZE  sizeof(uint32_t) * 16
#endif
#ifndef OM_POOL_LARGE_BLOCK_COUNT
    #define OM_POOL_LARGE_BLOCK_COUNT  4
#endif

#endif //OM_CONFIG_H_