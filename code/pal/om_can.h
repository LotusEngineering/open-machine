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


typedef struct Filter_t
{
    CanID ID;
    CanID Mask;
}Filter;

typedef struct OmCan_t
{
    OmActor* client;
    OmEvent* tx_complete_event;
    OmEvent* rx_data_event;
    OmEvent* error_event;
    OmCanPort port;
}OmCan;



void om_can_init(OmCan* self);

void om_can_open(OmCan* self);

void om_can_attach(OmCan* self,
                    OmActor* client,
                    OmEvent const * tx_complete_event,
                    OmEvent const * rx_data_event,
                    OmEvent const * error_event);


void om_can_close(OmCan* self);

void om_can_set_filters(OmCan* self, Filter const * const pFilterList, int numFilters);

bool om_can_send(OmCan* self, OmCanFrame const * const pFrame);

#endif //OM_CAN_H_