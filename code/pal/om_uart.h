// Copyright 2024 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.
#ifndef OM_UART_H_
#define OM_UART_H_
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "om_event.h"
#include "om_pool.h"
#include "om_actor.h"
#include "om_pal_port_uart.h"

typedef struct OmUart_t
{
    OmActor* client;
    OmEvent  tx_complete_event;
    OmSignal rx_data_sig;
    OmEvent error_event;
    OmUartPort port;
}OmUart;

typedef struct OmUartDataEvent_t
{
    OmPoolEvent base;
    uint8_t * data;
    size_t data_size;
}OmUartDataEvent;

void om_uart_init(OmUart* self);

/// @brief Attaches an actor to the UART and starts reception
/// @param self 
/// @param client 
/// @param tx_complete_sig 
/// @param rx_data_sig 
/// @param error_sig 
void om_uart_attach(OmUart* self,
                            OmActor* client,
                            OmSignal tx_complete_sig,
                            OmSignal rx_data_sig,
                            OmSignal error_sig);


/// @brief Non-blocking write to UART
/// @param self UART instance
/// @param data Data to write
/// @param data_size Data size
void om_uart_write(OmUart* self, 
                    uint8_t* data, 
                    size_t data_size);


// Internal function to create a new event
OmUartDataEvent* om_uart_data_event_new_(OmUart* uart, OmSignal rx_signal, uint8_t* data, size_t data_size);

#endif// OM_UART_H_