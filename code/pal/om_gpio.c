#include "om_gpio.h"


// Static events for attached clients that don't provide custom events
OM_EVENT(om_gpio_high_event, OM_EVT_GPIO_HIGH_EVENT);
OM_EVENT(om_gpio_low_event, OM_EVT_GPIO_LOW_EVENT);


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
    
    if (rising_edge_event != NULL)
    {
        self->rising_edge_event = rising_edge_event;
    }
    else
    {
        self->rising_edge_event = &om_gpio_high_event;
    }

    if (falling_edge_event != NULL)
    {
        self->falling_edge_event = falling_edge_event;
    }
    else
    {
        self->falling_edge_event = &om_gpio_low_event;
    }

}
