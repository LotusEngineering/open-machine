// Copyright 2024 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.
#include "om_uart.h"
#include "om_pal_port.h"
#include "om.h"

OM_ASSERT_SET_FILE_NAME("om_uart_stm32.c");

#define OM_UART_STM32_MAX_INST 6

static OmUart* om_uart_instance_table[OM_UART_STM32_MAX_INST];
static int om_uart_instance_count = 0;


void om_uart_stm32_init(OmUart* self, UART_HandleTypeDef* handle)
{
    OM_ASSERT(om_uart_instance_count <= OM_UART_STM32_MAX_INST);

    // Add instance to table for callback event lookup
    om_uart_instance_table[om_uart_instance_count] = self;

    // Increase instance count
    om_uart_instance_count++;

    // Call base class init
    om_uart_init(self);


    self->port.handle = handle;
}