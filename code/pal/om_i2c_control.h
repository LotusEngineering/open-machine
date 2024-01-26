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
#include "om_pal_port.h"


void om_i2c_control_attach(OmI2C* i2c,
                            OmActor* client,
                            OmEvent const * ok_event,
                            OmEvent const * error_event);



void om_i2c_control_write_memory(OmI2C* i2c, 
                                    uint16_t device_address, 
                                    uint32_t memory_address, 
                                    uint8_t memory_address_width, 
                                    uint8_t* data, 
                                    uint16_t data_size);

void om_i2c_control_read_memory(OmI2C* i2c, 
                                    uint16_t device_address, 
                                    uint32_t memory_address, 
                                    uint8_t memory_address_width, 
                                    uint8_t* data, 
                                    uint16_t data_size);


#endif// OM_I2C_H_