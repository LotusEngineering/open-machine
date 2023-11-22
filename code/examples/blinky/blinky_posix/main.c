#include <stdio.h>
#include <unistd.h>
#include "om_trace.h"
#include "om_timer.h"
#include "application.h"
#include <time.h>
#include <errno.h>    

/* msleep(): Sleep for the requested number of milliseconds. */
int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

#define OM_TRACE_TICK_MSEC 10
int main()
{
    application_start(0);
    printf("\n\n\n");
    printf("***************************************\n");
    printf("*** Blinky Example on POSIX         ***\n");
    printf("***************************************\n");

    
    /* Infinite loop */
    OmTraceLogEntry trace_record;
    
    for(;;)
    {
#if 0
        if(om_trace_is_full(&application_trace))
        {
            printf("WARNING!! Blinky Trace is Full!\n");
        }
        if(om_trace_read(&application_trace, &trace_record))
        {
           printf("%u:%s\n", (uint32_t)trace_record.timestamp_usec, trace_record.message);
        }
#endif        
        msleep(OM_TRACE_TICK_MSEC);
        om_trace_tick(&application_trace, OM_TRACE_TICK_MSEC * 1000);
        om_timer_tick(OM_TRACE_TICK_MSEC);

    }

}