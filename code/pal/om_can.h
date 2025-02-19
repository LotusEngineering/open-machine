#ifndef OM_CAN_H_
#define OM_CAN_H_

#include <stdint.h>

#include "om_event.h"
#include "om_actor.h"
#include "om_pal_port_can.h"
#include "om_pool.h"


#define OM_CAN_MAX_DATA_LENGTH 8
typedef uint32_t CanID;

typedef struct OmCanFrame_t
{
    CanID    ID;
    uint8_t  Data[OM_CAN_MAX_DATA_LENGTH];
    uint8_t  DataLength;
}OmCanFrame;


/// @brief Event that contains CAN frame
typedef struct OmCanDataEvent_t
{
    OmPoolEvent base;
    OmCan* can; ///< CAN instance 
    OmCanFrame frame;
}OmCanDataEvent;

/// @brief Event for CAN error sent with OM_EVT_CAN_ERROR
typedef struct OmCanErrorEvent_t
{
    OmPoolEvent base;
    OmCan* can; ///< CAN instance 
    uint32_t error_code;
    const char* error_message;
}OmCanErrorEvent;


typedef struct Filter_t
{
    CanID ID;
    CanID Mask;
}Filter;

typedef struct OmCan_t
{
    OmActor* client;
    OmEvent* tx_complete_event;
    OmEvent* rx_data_event; ///< Set to
    OmEvent* error_event;
    OmCanPort port;
}OmCan;



void om_can_init(OmCan* self);

void om_can_open(OmCan* self);


/// @brief Attaches an actor to the CAN and starts reception
/// @param self 
/// @param client 
/// @param tx_complete_event Transmit complete event or NULL for default OM_EVT_CAN_TX_OK
/// @param rx_data_event Receive data or NULL for default OM_EVT_CAN_RX_DATA
/// @param error_event Error event or NULL for default OM_EVT_CAN_ERROR
void om_can_attach(OmCan* self,
                    OmActor* client,
                    OmEvent const * tx_complete_event,
                    OmEvent const * rx_data_event,
                    OmEvent const * error_event);


void om_can_close(OmCan* self);

void om_can_set_filters(OmCan* self, Filter const * const pFilterList, int numFilters);

bool om_can_send(OmCan* self, OmCanFrame const * const pFrame);

#endif //OM_CAN_H_