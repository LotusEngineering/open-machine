// Copyright 2024 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.
#include "om_uart.h"
#include "om_pal_port_uart.h"
#include "om.h"

OM_ASSERT_SET_FILE_NAME("om_uart_stm32.c");

#define OM_UART_STM32_MAX_INST 3

static OmUart* om_uart_instance_table[OM_UART_STM32_MAX_INST];
static int om_uart_instance_count = 0;


void om_uart_stm32_init(OmUart* self, UART_HandleTypeDef* handle, uint8_t* rx_double_buffer, size_t rx_double_buffer_size)
{
    OM_ASSERT(om_uart_instance_count <= OM_UART_STM32_MAX_INST);

    // Add instance to table for callback event lookup
    om_uart_instance_table[om_uart_instance_count] = self;

    // Increase instance count
    om_uart_instance_count++;

    // Call base class init
    om_uart_init(self);

    self->port.handle = handle;
    self->port.rx_double_buffer = rx_double_buffer;

    // Ensure double buffer size is even
    OM_ASSERT(rx_double_buffer_size % 2 == 0);
    self->port.rx_double_buffer_size = rx_double_buffer_size;
    self->port.dma_tx_busy = false;
}

void om_uart_attach(OmUart* self,
                            OmActor * client,
                            OmSignal tx_complete_sig,
                            OmSignal rx_data_sig,
                            OmSignal error_sig)
{
    self->client = client;
    
    self->tx_complete_event.type = OM_ET_STATIC;
    self->tx_complete_event.signal = tx_complete_sig;
    self->tx_complete_event.name = "UART_TX_COMPLETE";

    
    self->rx_data_sig = rx_data_sig;

    self->error_event.type = OM_ET_STATIC;
    self->error_event.signal = error_sig;
    self->error_event.name = "UART_ERROR";

    // First receive into the first half of the buffer
    self->port.rx_pointer = self->port.rx_double_buffer;

    // Enable UART DMA to receive into the first half of the buffer
    if (HAL_UARTEx_ReceiveToIdle_DMA(self->port.handle, self->port.rx_pointer, (uint16_t)self->port.rx_double_buffer_size / 2) != HAL_OK)
    {
        // Did you enable the UART DMA (tx/rx) and Global Usart interrupt in the CubeMX configuration?
        OM_ERROR(); 
    }
}


void om_uart_write(OmUart* self, 
                    uint8_t* data, 
                    size_t data_size)
{
    if (data_size == 0)
    {
        return;
    }
    
    // Wait for the DMA to complete
    while(self->port.dma_tx_busy)
    {
        // Wait for the DMA to complete
    };

    HAL_StatusTypeDef status = HAL_UART_Transmit_DMA(self->port.handle, data, (uint16_t)data_size);
    if (status != HAL_OK)
    {
            // Did you enable the UART DMA (tx/rx) and Global Usart interrupt in the CubeMX configuration?
            OM_ERROR(); 
    }
    else
    {
        self->port.dma_tx_busy = true;
    }
}



void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    // Find matching base instance and send Error event
    for(int idx = 0; idx < om_uart_instance_count; idx++)
    {
        if( (om_uart_instance_table[idx]->port.handle == huart) &&
            (om_uart_instance_table[idx]->client != NULL) )
        {
            // Create a new data event
            OmUartDataEvent* uart_data = om_uart_data_event_new_(om_uart_instance_table[idx], 
                                                                    om_uart_instance_table[idx]->rx_data_sig, 
                                                                    om_uart_instance_table[idx]->port.rx_pointer, 
                                                                    (size_t)Size);

            // Send data event to client
            OMA_MSG(om_uart_instance_table[idx]->client, uart_data);
                                
            // Switch the buffer pointer
            if(om_uart_instance_table[idx]->port.rx_pointer == om_uart_instance_table[idx]->port.rx_double_buffer)
            {
                om_uart_instance_table[idx]->port.rx_pointer = om_uart_instance_table[idx]->port.rx_double_buffer + om_uart_instance_table[idx]->port.rx_double_buffer_size / 2;
            }
            else
            {
                om_uart_instance_table[idx]->port.rx_pointer = om_uart_instance_table[idx]->port.rx_double_buffer;
            }

            // Enable UART DMA to receive into next half buffer
            if (HAL_UARTEx_ReceiveToIdle_DMA(om_uart_instance_table[idx]->port.handle, om_uart_instance_table[idx]->port.rx_pointer, (uint16_t)om_uart_instance_table[idx]->port.rx_double_buffer_size / 2) != HAL_OK)
            {
                // Did you enable the UART DMA (tx/rx) and Global Usart interrupt in the CubeMX configuration?
                OM_ERROR(); 
            }

        } 
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    // Find matching base instance and send Error event
    for(int idx = 0; idx < om_uart_instance_count; idx++)
    {
        if( (om_uart_instance_table[idx]->port.handle == huart) &&
            (om_uart_instance_table[idx]->client != NULL) )
        {
            om_uart_instance_table[idx]->port.dma_tx_busy = false;
            OMA_MSG(om_uart_instance_table[idx]->client, 
                    &om_uart_instance_table[idx]->tx_complete_event);
        } 
    }
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
                    &om_uart_instance_table[idx]->error_event);
        } 
    }
}