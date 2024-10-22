// Copyright 2024 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.
#ifndef OM_GPIO_H_
#define OM_GPIO_H_

#include <stdbool.h>
#include <stdint.h>

#include "om_event.h"
#include "om_actor.h"
#include "om_pal_port_gpio.h" // Should define any bit masks required by application layer


typedef struct OmGpio_t
{
    OmActor* client;
    OmEvent const * rising_edge_event;
    OmEvent const * falling_edge_event;
    OmGpioPort port;
}OmGpio;

void om_gpio_init(OmGpio* self);

void om_gpio_attach(OmGpio* self,
                        OmActor* client,
                        OmEvent const * rising_edge_event,
                        OmEvent const * falling_edge_event);

bool om_gpio_read(OmGpio* self, uint32_t pin_mask);

void om_gpio_write(OmGpio* self, uint32_t pin_mask, bool value);

void om_gpio_toggle(OmGpio* self, uint32_t pin_mask);

#endif//OM_GPIO_H_