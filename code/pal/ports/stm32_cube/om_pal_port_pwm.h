// Copyright 2024 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.
#ifndef OM_PAL_PORT_PWM_H_
#define OM_PAL_PORT_PWM_H_

#include <stdint.h>

// The processor specific hal is provided in the generated "main.h"
#include "main.h" 


typedef struct 
{
    TIM_HandleTypeDef *htim;
    uint32_t channel;
}OmPwmPort;

// Forward declare
typedef struct OmPwm_t OmPwm;

void om_pwm_stm32_init(OmPwm* self, TIM_HandleTypeDef *htim, uint32_t channel);

#endif //OM_PAL_PORT_PWM_H_
