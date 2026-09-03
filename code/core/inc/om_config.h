/// Copyright 2023 Lotus Engineering LLC
///
/// Use of this source code is governed by an MIT-style
/// license that can be found in the LICENSE file or at
/// https://opensource.org/licenses/MIT.
/// @file om_config.h
/// @brief Configuration header for Open Machine framework
/// This file defines default configuration values for the Open Machine framework.
/// To customize the configuration, define OM_CONFIG_CUSTOM and provide a header file with your custom settings.
//// Default values can be also overridden by defining the corresponding macros before including this header.

#ifndef OM_CONFIG_H_
#define OM_CONFIG_H_

#include <stddef.h>

/// To define a custom configuration file, define OM_CONFIG_CUSTOM and provide a header file
#ifdef OM_CONFIG_CUSTOM
#include "om_config_custom.h"
#endif


#ifndef OM_TRACE_MAX_MESSAGE_LENGTH
    #define OM_TRACE_MAX_MESSAGE_LENGTH 80
#endif

#ifndef OM_ACTOR_MAX_ACTORS
    #define OM_ACTOR_MAX_ACTORS 16
#endif

#ifndef OM_HSM_MAX_STATE_DEPTH
    #define OM_HSM_MAX_STATE_DEPTH 6
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
    #define OM_POOL_SMALL_BLOCK_COUNT  32
#endif


#ifndef OM_POOL_MEDIUM_BLOCK_SIZE 
    #define OM_POOL_MEDIUM_BLOCK_SIZE  sizeof(uint32_t) * 8
#endif
#ifndef OM_POOL_MEDIUM_BLOCK_COUNT 
    #define OM_POOL_MEDIUM_BLOCK_COUNT  16
#endif

#ifndef OM_POOL_LARGE_BLOCK_SIZE 
    #define OM_POOL_LARGE_BLOCK_SIZE  sizeof(uint32_t) * 32
#endif
#ifndef OM_POOL_LARGE_BLOCK_COUNT
    #define OM_POOL_LARGE_BLOCK_COUNT  8
#endif


/// UART printf buffer size, one per UART instance, used to format om_uart_printf output
#ifndef OM_UART_PRINTF_BUFFER_SIZE
    #define OM_UART_PRINTF_BUFFER_SIZE 128
#endif


/// Console Service Command buffer size for storing incoming commands
#ifndef OM_CONSOLE_CMD_BUFFER_SIZE
    #define OM_CONSOLE_CMD_BUFFER_SIZE 80
#endif

/// Console Service Transmit buffer size for building responses before sending over UART
#ifndef OM_CONSOLE_TX_BUFFER_SIZE
    #define OM_CONSOLE_TX_BUFFER_SIZE 256
#endif

/// Console Service Maximum number of arguments for console commands
#ifndef OM_CONSOLE_MAX_ARGS
    #define OM_CONSOLE_MAX_ARGS 10
#endif


#endif //OM_CONFIG_H_