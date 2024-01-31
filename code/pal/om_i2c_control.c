#include "om_i2c_control.h"

void om_i2c_control_init(OmI2C* i2c)
{
    i2c->client = NULL;
    i2c->ok_event = NULL;
    i2c->error_event = NULL;
}


void om_i2c_control_attach(OmI2C* i2c,
                            OmActor * client,
                            OmEvent const * ok_event,
                            OmEvent const * error_event)
{
    i2c->client = client;
    i2c->ok_event = ok_event;
    i2c->error_event = error_event;
}
