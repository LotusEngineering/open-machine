#include "om_i2c.h"

void om_i2c_init(OmI2C* self)
{
    self->client = NULL;
}


void om_i2c_attach(OmI2C* self,
                    OmActor * client,
                    OmEvent const * read_ok_event,
                    OmEvent const * write_ok_event,
                    OmEvent const * error_event)

{
    self->client = client;
    self->read_ok_event = read_ok_event;
    self->write_ok_event = write_ok_event;
    self->error_event = error_event;
}
