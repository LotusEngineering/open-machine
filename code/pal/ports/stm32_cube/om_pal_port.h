// Copyright 2024 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.
#ifndef OM_PAL_PORT_H_
#define OM_PAL_PORT_H_

// The processor specific hal is provided in the generated "main.h"
#include "main.h" 



// I2C /////////////////////////////////////////////////////////////////////////

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


// GPIO ////////////////////////////////////////////////////////////////////////

typedef struct 
{
    GPIO_TypeDef* handle;
    uint16_t event_pin_mask;
}OmGpioPort;

// Forward declare
typedef struct OmGpio_t OmGpio;

/// @brief Port specific init function
/// @param self 
/// @param handle 
/// @param event_pin_mask pin mask that triggers rising or falling edge event
void om_i2c_gpio_stm32_init(OmGpio* self, GPIO_TypeDef* handle, uint16_t event_pin_mask);

#endif //OM_PAL_PORT_H_
