// Copyright 2024 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.
#ifndef OM_PAL_PORT_UART_H_
#define OM_PAL_PORT_UART_H_

#include <stdint.h>

// The processor specific hal is provided in the generated "main.h"
#include "main.h" 

typedef struct 
{
    UART_HandleTypeDef* handle;
}OmUartPort;

// Forward declare
typedef struct OmUart_t OmUart;

/// @brief Port specific init function
/// @param self 
/// @param handle 
void om_uart_stm32_init(OmUart* self, UART_HandleTypeDef* handle);

#endif //OM_PAL_PORT_UART_H_