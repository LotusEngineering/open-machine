#ifndef OM_PWM_H_
#define OM_PWM_H_


#include "om_event.h"
#include "om_actor.h"
#include "om_pal_port.h"

typedef struct OmPwm_t
{
    OmPwmPort port;
}OmPwm;

void om_pwm_start(OmPwm* self, uint32_t percent_x100);

void om_pwm_set_duty(OmPwm* self, uint32_t percent_x100);

void om_pwm_stop(OmPwm* self);

#endif