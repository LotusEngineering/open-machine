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

void om_uart_write(OmUart* self, 
                    uint8_t* data, 
                    uint16_t data_size)
{
    if (HAL_UART_Transmit_DMA(self->port.handle, data, data_size) != HAL_OK)
    {
        OM_ERROR(); // Send error event
    }
}

void om_uart_read(OmUart* self, 
                        uint8_t* data, 
                        uint16_t data_size)
{
    if (HAL_UARTEx_ReceiveToIdle_DMA(self->port.handle, data, data_size) != HAL_OK)
    {
        OM_ERROR(); // Send error event
    }
}

/////////////////// Private Functions ////////////////////////////
static inline void om_uart_stm32_send_ok_(UART_HandleTypeDef *huart)
{
    // Find matching base instance and send OK event
    for(int idx = 0; idx < om_uart_instance_count; idx++)
    {
        if( (om_uart_instance_table[idx]->port.handle == huart) &&
            (om_uart_instance_table[idx]->client != NULL) )
        {
            OMA_MSG(om_uart_instance_table[idx]->client, 
                    om_uart_instance_table[idx]->ok_event);
        } 
    }
}

#if 0
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    om_uart_stm32_send_ok_(huart);
}
#endif

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    om_uart_stm32_send_ok_(huart);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    om_uart_stm32_send_ok_(huart);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    // Find matching base instance and send Error event
    for(int idx = 0; idx < om_uart_instance_count; idx++)
    {
        if( (om_uart_instance_table[idx]->port.handle == huart) &&
            (om_uart_instance_table[idx]->client != NULL) )
        {
            OMA_MSG(om_uart_instance_table[idx]->client, 
                    om_uart_instance_table[idx]->error_event);
        } 
    }
}