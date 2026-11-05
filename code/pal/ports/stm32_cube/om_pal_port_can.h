// Copyright 2024 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#ifndef OM_PAL_PORT_CAN_H_
#define OM_PAL_PORT_CAN_H_

#include <stdint.h>

// Forward Declare
//typedef struct CAN_HandleTypeDef_t CAN_HandleTypeDef;

// The processor specific hal is provided in the generated "main.h"
#include "main.h" 


// Forward declare
typedef struct OmCan_t OmCan;

// There are two different types of CAN devices on STM32, the FDCAN and the older CAN. The HAL provides different types for each. 
// This struct abstracts the differences away from the rest of the code.
#if defined(HAL_FDCAN_MODULE_ENABLED)
  extern FDCAN_HandleTypeDef hfdcan1;
typedef struct 
{
    FDCAN_HandleTypeDef* handle;
}OmCanPort;

/// @brief Port specific init function
/// @param self 
/// @param handle 
void om_can_stm32_init(OmCan* self, FDCAN_HandleTypeDef* handle);

#elif defined(HAL_CAN_MODULE_ENABLED)

typedef struct 
{
    CAN_HandleTypeDef* handle;
}OmCanPort;

/// @brief Port specific init function
/// @param self 
/// @param handle 
void om_can_stm32_init(OmCan* self, CAN_HandleTypeDef* handle);

#else
  #error "No CAN HAL module enabled"
#endif




#endif //OM_PAL_PORT_CAN_H_