// Copyright 2024 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "om_can.h"
#include "om_pal_port_can.h"
#include "om.h"

OM_ASSERT_SET_FILE_NAME("om_can_stm32.c");

#define OM_CAN_STM32_MAX_INST 6

// Static events for attached clients
OM_EVENT(tx_complete_event, OM_EVT_CAN_TX_OK);

static OmCan *om_can_instance_table[OM_CAN_STM32_MAX_INST];
static int om_can_instance_count = 0;

void om_can_stm32_init(OmCan *self, CAN_HandleTypeDef *handle)
{
    OM_ASSERT(om_can_instance_count <= OM_CAN_STM32_MAX_INST);

    // Add instance to table for callback event lookup
    om_can_instance_table[om_can_instance_count] = self;

    // Increase instance count
    om_can_instance_count++;

    // Call base class init
    om_can_init(self);

    self->port.handle = handle;
}

void om_can_open(OmCan* self)
{
    if (HAL_CAN_Start(self->port.handle) != HAL_OK)
    {
      OM_ERROR();
    }
  
    if (HAL_CAN_ActivateNotification(self->port.handle, CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_TX_MAILBOX_EMPTY) != HAL_OK)
    {
      OM_ERROR();
    }
}

void om_can_close(OmCan* self)
{
  if (HAL_CAN_Stop(self->port.handle) != HAL_OK)
  {
    OM_ERROR();
  }

  if (HAL_CAN_DeactivateNotification(self->port.handle, CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_TX_MAILBOX_EMPTY) != HAL_OK)
  {
    OM_ERROR();
  }

}

void om_can_set_filters(OmCan* self, OmCanFilter const * const pFilterList, int numFilters)
{
  static CAN_FilterTypeDef filter_config;
  
  // Currently only allowing 3 filters since we are using filter banks 0..2 based on index below
  OM_ASSERT(numFilters <= 3);

  for(int idx = 0; idx < numFilters; ++idx)
  {

    filter_config.FilterActivation = ENABLE;
    filter_config.FilterFIFOAssignment = CAN_FILTER_FIFO0;
    filter_config.FilterScale = CAN_FILTERSCALE_32BIT;
    filter_config.FilterMode = CAN_FILTERMODE_IDMASK;
    filter_config.FilterBank = idx;

    uint32_t filter_id = pFilterList[idx].id;
    uint32_t filter_mask = pFilterList[idx].mask;
    if (pFilterList[idx].is_extended)
    {
      filter_config.FilterIdHigh = (filter_id << 3) >> 16;
      filter_config.FilterIdLow = (filter_id << 3);
      filter_config.FilterMaskIdHigh = (filter_mask << 3) >> 16;
      filter_config.FilterMaskIdLow = (filter_mask << 3);
    }
    else
    {
      filter_config.FilterIdHigh = (filter_id << 5);
      filter_config.FilterIdLow = 0;
      filter_config.FilterMaskIdHigh = (filter_mask << 5);
      filter_config.FilterMaskIdLow = 0;
    }
  
    if (HAL_CAN_ConfigFilter(self->port.handle, &filter_config) != HAL_OK)
    {
      /* Filter configuration Error */
      OmError();
    }
  

  }
}

bool om_can_send(OmCan* self, OmCanFrame const * const pFrame)
{

}
