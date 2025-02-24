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

OmCanDataEvent* om_can_data_event_new_(OmCan* self)
{
    OmCanDataEvent* event = NULL;
    
    if (self->rx_data_event == NULL)
    {
        // Use default signal
        event = OM_POOL_EVENT_NEW(OmCanDataEvent,  OM_EVT_CAN_RX_DATA); 
    }
    else
    {
        // Use client specifed signal
        event = (OmCanDataEvent*)om_pool_alloc(sizeof(OmCanDataEvent), self->rx_data_event->signal, self->rx_data_event->name);
    }

    event->can = self;
    // Note receiving frame data is port specific and must be done after calling this function 
    // event->frame = ?;
 
    return event;

}