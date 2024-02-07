// Copyright 2024 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.
#include "om_i2c_control.h"
#include "om_pal_port.h"
#include "om.h"

OM_ASSERT_SET_FILE_NAME("om_i2c_control_stm32.c");

#define OM_I2C_STM32_MAX_INST 6

static OmI2C* om_i2c_instance_table[OM_I2C_STM32_MAX_INST];
static int om_i2c_instance_count = 0;

void om_i2c_control_stm32_init(OmI2C* self, I2C_HandleTypeDef* handle)
{
    OM_ASSERT(om_i2c_instance_count <= OM_I2C_STM32_MAX_INST);

    // Add instance to table for callback event lookup
    om_i2c_instance_table[om_i2c_instance_count] = self;

    // Increase instance count
    om_i2c_instance_count++;

    // Call base class init
    om_i2c_control_init(self);


    self->port.handle = handle;
}

void om_i2c_control_write_memory(OmI2C* self, 
                                    uint16_t device_address, 
                                    uint32_t memory_address, 
                                    uint8_t memory_address_width,
                                    uint8_t* data, 
                                    uint16_t data_size)
{
    uint16_t mem_add_size = 0;
    
    if(memory_address_width == 1)
    {
        mem_add_size = I2C_MEMADD_SIZE_8BIT;
    }
    else if (memory_address_width == 2)
    {
        mem_add_size = I2C_MEMADD_SIZE_16BIT;
    }
    else
    {
        // Only 1 or 2 byte addresses are supported
        OM_ERROR();
    }

    if (HAL_I2C_Mem_Write_DMA(self->port.handle, 
                                device_address << 1, 
                                (uint16_t)memory_address,
                                mem_add_size, 
                                data, 
                                data_size) != HAL_OK)
    {
        OM_ERROR();
    }

}

void om_i2c_control_read_memory(OmI2C* self, 
                                    uint16_t device_address, 
                                    uint32_t memory_address, 
                                    uint8_t memory_address_width,
                                    uint8_t* data, 
                                    uint16_t data_size)
{
    uint16_t mem_add_size = 0;
    
    if(memory_address_width == 1)
    {
        mem_add_size = I2C_MEMADD_SIZE_8BIT;
    }
    else if (memory_address_width == 2)
    {
        mem_add_size = I2C_MEMADD_SIZE_16BIT;
    }
    else
    {
        // Only 1 or 2 byte addresses are supported
        OM_ERROR();
    }

    if (self->port.handle->State == HAL_I2C_STATE_READY)
    {

        if (HAL_I2C_Mem_Read_DMA(self->port.handle, 
                                    ((device_address << 1) | 0x01), 
                                    (uint16_t)memory_address,
                                    mem_add_size, 
                                    data, 
                                    data_size) != HAL_OK)
        {
            OM_ERROR(); // Send error event
        }
    }


}

static inline void om_i2c_control_stm32_send_ok_(I2C_HandleTypeDef *hi2c)
{
    // Find matching base instance and send OK event
    for(int idx = 0; idx < om_i2c_instance_count; idx++)
    {
        if( (om_i2c_instance_table[idx]->port.handle == hi2c) &&
            (om_i2c_instance_table[idx]->client != NULL) )
        {
            OMA_MSG(om_i2c_instance_table[idx]->client, 
                    om_i2c_instance_table[idx]->ok_event);
        } 
    }
}

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    om_i2c_control_stm32_send_ok_(hi2c);
}

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    om_i2c_control_stm32_send_ok_(hi2c);
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
    // Find matching base instance and send Error event
    for(int idx = 0; idx < om_i2c_instance_count; idx++)
    {
        if( (om_i2c_instance_table[idx]->port.handle == hi2c) &&
            (om_i2c_instance_table[idx]->client != NULL) )
        {
            OMA_MSG(om_i2c_instance_table[idx]->client, 
                    om_i2c_instance_table[idx]->error_event);
        } 
    }
}