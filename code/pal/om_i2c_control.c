#include "om_i2c_control.h"

void om_i2c_control_init(OmI2C* self)
{
    self->client = NULL;
    self->ok_event = NULL;
    self->error_event = NULL;
}


void om_i2c_control_attach(OmI2C* self,
                            OmActor * client,
                            OmEvent const * ok_event,
                            OmEvent const * error_event)
{
    self->client = client;
    self->ok_event = ok_event;
    self->error_event = error_event;
}
