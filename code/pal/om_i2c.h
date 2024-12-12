// Copyright 2024 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.


#ifndef OM_I2C_H_
#define OM_I2C_H_
#include <stdint.h>
#include <stdbool.h>

#include "om_event.h"
#include "om_actor.h"
#include "om_pool.h"
#include "om_pal_port_i2c.h"

typedef struct OmI2C_t
{
    OmActor* client;
    OmEvent const * read_ok_event;
    OmEvent const * write_ok_event;
    OmEvent const * error_event;
    OmI2CPort port;
}OmI2C;

/// @brief Event for I2C error sent with OM_EVT_I2C_ERROR
typedef struct OmI2CErrorEvent_t
{
    OmPoolEvent base;
    OmI2C* i2c; ///< I2C instance 
    uint32_t error_code;
    const char* error_message;
}OmI2CErrorEvent;

void om_i2c_init(OmI2C* self);

/// @brief Attaches an actor to the I2C 
void om_i2c_attach(OmI2C* self,
                    OmActor* client,
                    OmEvent const * read_ok_event,
                    OmEvent const * write_ok_event,
                    OmEvent const * error_event);



/// @brief Async Write data to I2C device and send OM_EVT_I2C_WRITE_DONE when complete
/// @param self 
/// @param device_address 
/// @param memory_address 
/// @param memory_address_width 
/// @param data 
/// @param data_size 
void om_i2c_write_memory(OmI2C* self, 
                            uint16_t device_address, 
                            uint32_t memory_address, 
                            uint8_t memory_address_width,
                            uint8_t* data, 
                            uint16_t data_size);

/// @brief Async Read data from I2C device and send OM_EVT_I2C_READ_DONE or OM_EVT_I2C_ERROR when complete
/// @param self 
/// @param device_address 
/// @param memory_address 
/// @param memory_address_width 
/// @param data 
/// @param data_size 
void om_i2c_read_memory(OmI2C* self, 
                            uint16_t device_address, 
                            uint32_t memory_address, 
                            uint8_t memory_address_width,
                            uint8_t* data, 
                            uint16_t data_size);


/// @brief Sync Write data to I2C device and return when complete
/// @param self 
/// @param device_address 
/// @param memory_address 
/// @param memory_address_width 
/// @param data 
/// @param data_size 
/// @param timeout_ms, how long to wait for write to complete
/// @return Non-zero on error
int om_i2c_write_memory_sync(OmI2C* self, 
                            uint16_t device_address, 
                            uint32_t memory_address, 
                            uint8_t memory_address_width,
                            uint8_t* data, 
                            uint16_t data_size,
                            uint32_t timeout_ms);

/// @brief Sync read data from I2C device and return when complete
/// @param self 
/// @param device_address 
/// @param memory_address 
/// @param memory_address_width 
/// @param data 
/// @param data_size 
/// @param timeout_ms 
/// @return Non-zero on error
int om_i2c_read_memory_sync(OmI2C* self, 
                            uint16_t device_address, 
                            uint32_t memory_address, 
                            uint8_t memory_address_width,
                            uint8_t* data, 
                            uint16_t data_size,
                            uint32_t timeout_ms);

#endif// OM_I2C_H_