// Copyright 2024 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OM_PAL_PORT_CAN_H_
#define OM_PAL_PORT_CAN_H_

#include <stdint.h>

// The processor specific hal is provided in the generated "main.h"
#include "main.h" 

typedef struct 
{
    CAN_HandleTypeDef* handle;
}OmCanPort;

// Forward declare
typedef struct OmCan_t OmCan;

/// @brief Port specific init function
/// @param self 
/// @param handle 
void om_can_stm32_init(OmCan* self, CAN_HandleTypeDef* handle);

#endif //OM_PAL_PORT_I2C_H_