// Copyright 2024 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.
#ifndef OM_PAL_PORT_I2C_H_
#define OM_PAL_PORT_I2C_H_

#include <stdint.h>

// The processor specific hal is provided in the generated "main.h"
#include "main.h" 

typedef struct 
{
    I2C_HandleTypeDef* handle;
}OmI2CPort;

// Forward declare
typedef struct OmI2C_t OmI2C;

/// @brief Port specific init function
/// @param self 
/// @param handle 
void om_i2c_control_stm32_init(OmI2C* self, I2C_HandleTypeDef* handle);

#endif //OM_PAL_PORT_I2C_H_