#include "om_uart.h"
#include "om_pool.h"

void om_uart_init(OmUart* self)
{
    self->client = NULL;
    self->rx_data_sig = OM_EVT_USER;
}


OmUartDataEvent* om_uart_data_event_new_(OmUart* uart, uint8_t* data, size_t data_size)
{
    OmUartDataEvent* event = OM_POOL_EVENT_NEW(OmUartDataEvent, OM_EVT_UART_RX_DATA); 
    event->data = data;
    event->data_size = data_size;
    return event;
}

OmUartErrorEvent* om_uart_error_event_new_(OmUart* uart, uint32_t error_code, const char* error_message)
{
    OmUartErrorEvent* event = OM_POOL_EVENT_NEW(OmUartErrorEvent, OM_EVT_UART_ERROR); 
    event->uart = uart;
    event->error_code = error_code;
    event->error_message = error_message;
    return event;
}
