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

typedef struct OmI2C
{
    I2C_HandleTypeDef* handle;
    OmActor* client;
    OmEvent const * ok_event;
    OmEvent const * error_event;
}OmI2C;

/// @brief Port specific init function
/// @param i2c 
/// @param handle 
void om_i2c_control_init(OmI2C* i2c, I2C_HandleTypeDef* handle);

#endif //OM_PAL_PORT_H_
