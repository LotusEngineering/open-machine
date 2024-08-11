// Copyright 2024 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.
#ifndef OM_UART_H_
#define OM_UART_H_
#include <stdint.h>
#include <stdbool.h>

#include "om_event.h"
#include "om_actor.h"
#include "om_pal_port.h"

typedef struct OmUart_t
{
    OmActor* client;
    OmEvent const * ok_event;
    OmEvent const * error_event;
    OmUartPort port;
}OmUart;

void om_uart_init(OmUart* self);

void om_uart_attach(OmUart* self,
                            OmActor* client,
                            OmEvent const * ok_event,
                            OmEvent const * error_event);



void om_uart_write(OmUart* self, 
                                    uint8_t* data, 
                                    uint16_t data_size);

uint16_t om_uart_read(OmUart* self, 
                                    uint8_t* data, 
                                    uint16_t data_size);


#endif// OM_UART_H_