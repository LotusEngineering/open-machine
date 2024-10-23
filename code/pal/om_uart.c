#include "om_uart.h"
#include "om_pool.h"

void om_uart_init(OmUart* self)
{
    self->client = NULL;
    self->rx_data_sig = OM_EVT_USER;
}


OmUartDataEvent* om_uart_data_event_new_(OmUart* uart, OmSignal rx_signal, uint8_t* data, size_t data_size)
{
    OmUartDataEvent* event = OM_POOL_EVENT_NEW(OmUartDataEvent, rx_signal); // This will cause the event to show "rx_signal" in the trace
    event->data = data;
    event->data_size = data_size;
    return event;
}