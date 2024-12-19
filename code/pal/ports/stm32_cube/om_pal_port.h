// Copyright 2024 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

// This file includes all the port specific headers for the Open Machine PAL

#ifndef OM_PAL_PORT_H_
#define OM_PAL_PORT_H_

#include <stdint.h>

// The processor specific hal is provided in the generated "main.h"
#include "main.h" 


#include "om_pal_port_i2c.h"
#include "om_pal_port_gpio.h"
#include "om_pal_port_pwm.h"
#include "om_pal_port_uart.h"
#include "om_pal_port_cat.h"

#endif //OM_PAL_PORT_H_