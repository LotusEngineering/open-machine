// Copyright 2024 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.
#ifndef OM_PAL_PORT_UART_H_
#define OM_PAL_PORT_UART_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// The processor specific hal is provided in the generated "main.h"
#include "main.h" 

typedef struct 
{
    UART_HandleTypeDef* handle;
    uint8_t* rx_double_buffer;
    size_t rx_double_buffer_size;
    uint8_t* rx_pointer; // Pointer to where we are receiving from
    bool dma_tx_busy;
}OmUartPort;

// Forward declare
typedef struct OmUart_t OmUart;

/// @brief Port specific init function
/// @param self 
/// @param handle 
void om_uart_stm32_init(OmUart* self, UART_HandleTypeDef* handle, uint8_t* rx_double_buffer, size_t rx_double_buffer_size);

#endif //OM_PAL_PORT_UART_H_