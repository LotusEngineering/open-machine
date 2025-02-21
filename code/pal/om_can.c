#include "om_can.h"
#include "om.h"

OM_ASSERT_SET_FILE_NAME("om_can.c");

void om_can_init(OmCan* self)
{
    self->client = NULL;
    self->tx_complete_event = NULL;
    self->rx_data_event = NULL;
    self->error_event = NULL;
}


void om_can_attach(OmCan* self,
                    OmActor* client,
                    OmEvent const * tx_complete_event,
                    OmEvent const * rx_data_event,
                    OmEvent const * error_event)
{
    // Sorry only one client at a time
    OM_ASSERT(self->client == NULL);
    self->client = client;
    self->tx_complete_event = tx_complete_event;
    self->rx_data_event = rx_data_event;
    self->error_event = error_event;
}