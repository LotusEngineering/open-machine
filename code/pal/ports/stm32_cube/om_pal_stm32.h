// Copyright 2024 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.
#ifndef OM_PAL_PORT_H_
#define OM_PAL_PORT_H_

// The processor specific hal is provided in the generated "main.h"
#include "main.h"
#include "om_i2c_control.h"


typedef struct OmI2CStm32
{
    OmI2C base;
    I2C_HandleTypeDef* handle;
}OmI2CStm32;

/// @brief Port specific init function
/// @param i2c 
/// @param handle 
void om_i2c_control_stm32_init(OmI2CStm32* self, I2C_HandleTypeDef* handle);

#endif //OM_PAL_PORT_H_
