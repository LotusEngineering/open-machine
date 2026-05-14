#include "om_trace_svc.h"
#include "om_event.h"
#include "om_hsm.h"
#include "om_trace.h"
#include "om_timer.h"

// Declare Init trans
OmStateResult om_trace_svc_init_trans(OmTraceSvc *self);

// Declare the states
OM_STATE_DECLARE(OmTraceSvc, om_trace_svc_super, OM_TOP_STATE);

void om_trace_svc_init(OmTraceSvc* self, 
                       OmTracePrintf printf,
                       OmTrace* trace,
                       uint32_t trace_rate_msec,
                       OmActorAttr* actor_attr,
                       OmTraceAttr* trace_attr)
{
    om_actor_init(&self->base,
                  OM_INIT_CAST(om_trace_svc_init_trans),
                  actor_attr,
                  trace_attr);

    self->printf = printf;
    self->trace = *trace;
    self->trace_rate_msec = trace_rate_msec;
    
    om_timer_init(&self->timer, 
            OM_EVT_TRACE_SVC_TICK, 
              OM_NAME_OF(OM_EVT_TRACE_SVC_TICK), 
             &self->base);

}

OmStateResult om_trace_svc_init_trans(OmTraceSvc *self)
{
    OmStateResult result = OM_TRANS(om_trace_svc_super);
    return result;
}

OM_STATE_DEFINE(OmTraceSvc, om_trace_svc_super)
{
    OmStateResult result = OM_RES_IGNORED;
    switch (event->signal)
    {
    case OM_EVT_ENTER:
        // Start trace timer
        om_timer_start(&self->timer, 
                        OM_TM_PERIODIC, 
                        self->trace_rate_msec);
        result = OM_RES_HANDLED;
        break;

    case OM_EVT_TRACE_SVC_TICK:
        {
            OmTraceLogEntry trace_record;
            // while(om_trace_read(&self->trace, &trace_record))
            // {
            //     self->printf("%lu:%s\n", (uint32_t)trace_record.timestamp_usec, trace_record.message);
            // }
            if(om_trace_is_full(&self->trace))
            {
                self->printf("WARNING!! Trace Buffer is Full!\n");
            }
            if(om_trace_read(&self->trace, &trace_record))
            {
                self->printf("%lu:%s\n", (uint32_t)trace_record.timestamp_usec, trace_record.message);
            }
        result = OM_RES_HANDLED;
        }
        break;
    case OM_EVT_EXIT:
        // Stop trace timer
        om_timer_stop(&self->timer);
        result = OM_RES_HANDLED;
        break;
    }

    return result;
}