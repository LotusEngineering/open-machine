#include "om_gpio.h"

void om_gpio_init(OmGpio* self)
{
    self->client = NULL;
    self->rising_edge_event = NULL;
    self->falling_edge_event = NULL;
}


void om_gpio_attach(OmGpio* self,
                            OmActor * client,
                            OmEvent const * rising_edge_event,
                            OmEvent const * falling_edge_event)
{
    self->client = client;
    self->rising_edge_event = rising_edge_event;
    self->falling_edge_event = falling_edge_event;
}
