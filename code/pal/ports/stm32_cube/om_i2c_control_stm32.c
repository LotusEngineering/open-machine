// Copyright 2024 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.
#include "om_i2c_control.h"

#include "om_assert.h"

OM_ASSERT_SET_FILE_NAME("om_i2c_control_stm32.c");

void om_i2c_control_init(OmI2C* i2c, I2C_HandleTypeDef* handle)
{
    i2c->handle = handle;
    i2c->client = NULL;
    i2c->ok_event = NULL;
    i2c->error_event = NULL;
}

void om_i2c_control_attach(OmI2C* i2c,
                            OmActor * client,
                            OmEvent const * ok_event,
                            OmEvent const * error_event)
{
    i2c->client = client;
    i2c->ok_event = ok_event;
    i2c->error_event = error_event;
}
void om_i2c_control_write_memory(OmI2C* i2c, 
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

    if (HAL_I2C_Mem_Write_DMA(i2c->handle, 
                                device_address, 
                                (uint16_t)memory_address,
                                mem_add_size, 
                                data, 
                                data_size) != HAL_OK)
    {
        OM_ERROR();
    }

}

void om_i2c_control_read_memory(OmI2C* i2c, 
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

    if (HAL_I2C_Mem_Read_DMA(i2c->handle, 
                                device_address, 
                                (uint16_t)memory_address,
                                mem_add_size, 
                                data, 
                                data_size) != HAL_OK)
    {
        OM_ERROR();
    }

}
