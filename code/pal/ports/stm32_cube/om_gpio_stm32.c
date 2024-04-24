#include "om_assert.h"

#include "om_gpio.h"
#include "om_pal_port.h"

OM_ASSERT_SET_FILE_NAME("om_gpio_stm32.c");

#define OM_GPIO_STM32_MAX_INST 32

static OmGpio* om_gpio_instance_table[OM_GPIO_STM32_MAX_INST];
static int om_gpio_instance_count = 0;


void om_gpio_stm32_init(OmGpio* self, GPIO_TypeDef* handle, uint16_t event_pin_mask)
{
    OM_ASSERT(om_gpio_instance_count <= OM_GPIO_STM32_MAX_INST);

    // Add instance to table for callback event lookup
    om_gpio_instance_table[om_gpio_instance_count] = self;

    // Increase instance count
    om_gpio_instance_count++;
    OM_ASSERT(om_gpio_instance_count <= OM_GPIO_STM32_MAX_INST);

    // Init common members
    om_gpio_init(self);

    self->port.handle = handle;
    self->port.event_pin_mask = event_pin_mask;
}

bool om_gpio_read(OmGpio* self, uint32_t pin_mask)
{
    return HAL_GPIO_ReadPin(self->port.handle, (uint16_t)pin_mask) == GPIO_PIN_SET;
}

void om_gpio_write(OmGpio* self, uint32_t pin_mask, bool value)
{
    if(value)
    {
        HAL_GPIO_WritePin(self->port.handle, (uint16_t)pin_mask, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(self->port.handle, (uint16_t)pin_mask, GPIO_PIN_RESET);
    }
}

void om_gpio_toggle(OmGpio* self, uint32_t pin_mask)
{
    HAL_GPIO_TogglePin(self->port.handle, (uint16_t)pin_mask);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    // Find matching instance
    for(int idx = 0; idx < om_gpio_instance_count; idx++)
    {
        if( (om_gpio_instance_table[idx]->port.event_pin_mask & GPIO_Pin)  &&
            (om_gpio_instance_table[idx]->client != NULL) )
        {
            if (om_gpio_read(om_gpio_instance_table[idx],
                om_gpio_instance_table[idx]->port.event_pin_mask ))
            {
                if(om_gpio_instance_table[idx]->rising_edge_event != NULL)
                {
                    OMA_MSG(om_gpio_instance_table[idx]->client, 
                            om_gpio_instance_table[idx]->rising_edge_event);
                }
            }
            else
            {
                if(om_gpio_instance_table[idx]->falling_edge_event != NULL)
                {
                    OMA_MSG(om_gpio_instance_table[idx]->client, 
                            om_gpio_instance_table[idx]->falling_edge_event);
                }
            }
        } 
    }

}