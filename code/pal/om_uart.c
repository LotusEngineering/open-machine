#include "om_uart.h"

void om_uart_init(OmUart* self)
{
    self->client = NULL;
    self->tx_done_event = NULL;
    self->rx_data_event = NULL;
    self->error_event = NULL;
}


void om_uart_attach(OmUart* self,
                            OmActor * client,
                            OmEvent const * tx_done_event,
                            OmEvent const * rx_data_event,
                            OmEvent const * error_event)
{
    self->client = client;
    self->tx_done_event = tx_done_event;
    self->rx_data_event = rx_data_event;
    self->error_event = error_event;
}
