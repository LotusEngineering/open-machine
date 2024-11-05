#include "om_i2c.h"

void om_i2c_init(OmI2C* self)
{
    self->client = NULL;
}


void om_i2c_attach(OmI2C* self,
                    OmActor * client)
{
    self->client = client;
}
