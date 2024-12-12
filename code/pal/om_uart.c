#include "om_uart.h"
#include "om_pool.h"

void om_uart_init(OmUart* self)
{
    self->client = NULL;
    self->tx_complete_event =  NULL;
    self->rx_data_event = NULL;
    self->error_event = NULL;
}


OmUartDataEvent* om_uart_data_event_new_(OmUart* self, uint8_t* data, size_t data_size)
{
    OmUartDataEvent* event = NULL;
    
    if (self->rx_data_event == NULL)
    {
        // Use default signal
        event = OM_POOL_EVENT_NEW(OmUartDataEvent, OM_EVT_UART_RX_DATA); 
    }
    else
    {
        event = (OmUartDataEvent*)om_pool_alloc(sizeof(OmUartDataEvent), self->rx_data_event->signal, self->rx_data_event->name);
    }

    event->uart = self;
    event->data = data;
    event->data_size = data_size;
    return event;
}

OmUartErrorEvent* om_uart_error_event_new_(OmUart* self, uint32_t error_code, const char* error_message)
{

    OmUartErrorEvent* event = NULL;
    
    if (self->rx_data_event == NULL)
    {
        // Use default signal
        event = OM_POOL_EVENT_NEW(OmUartErrorEvent, OM_EVT_UART_ERROR); 
    }
    else
    {
        event = (OmUartErrorEvent*)om_pool_alloc(sizeof(OmUartErrorEvent), self->error_event->signal, self->error_event->name);
    }

    event->uart = self;
    event->error_code = error_code;
    event->error_message = error_message;
    return event;
}
