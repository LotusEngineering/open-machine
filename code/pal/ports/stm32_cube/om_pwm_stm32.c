#include "om_pwm.h"
#include "om_pal_port.h"
#include "om_assert.h"

OM_ASSERT_SET_FILE_NAME("om_pwm_stm32.c");

void om_pwm_stm32_init(OmPwm* self, TIM_HandleTypeDef *htim, uint32_t channel)
{
    self->port.htim = htim;
    self->port.channel = channel;
}


void om_pwm_start(OmPwm* self, uint32_t percent_x100)
{
    om_pwm_set_duty(self, percent_x100);
    HAL_TIM_PWM_Start(self->port.htim, self->port.channel);
}

void om_pwm_set_duty(OmPwm* self, uint32_t percent_x100)
{
    uint32_t ccr_value = (percent_x100 * self->port.htim->Init.Period) / 10000;
    switch(self->port.channel)
    {
        case TIM_CHANNEL_1:
            self->port.htim->Instance->CCR1 = ccr_value;
        break;
        case TIM_CHANNEL_2:
            self->port.htim->Instance->CCR2 = ccr_value;
        break;
        case TIM_CHANNEL_3:
            self->port.htim->Instance->CCR3 = ccr_value;
        break;
        case TIM_CHANNEL_4:
            self->port.htim->Instance->CCR4 = ccr_value;
        break;
        case TIM_CHANNEL_5:
            self->port.htim->Instance->CCR5 = ccr_value;
        break;
        case TIM_CHANNEL_6:
            self->port.htim->Instance->CCR6 = ccr_value;
        break;
        default:
            OM_ERROR();
        break;
    }

}

void om_pwm_stop(OmPwm* self)
{
    HAL_TIM_PWM_Stop(self->port.htim, self->port.channel);
}
