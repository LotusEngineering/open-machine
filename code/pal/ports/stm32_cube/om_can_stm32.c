// Copyright 2024 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

// OmCan port for STM32 parts with the bxCAN peripheral. Parts with FDCAN use
// om_can_stm32_fdcan.c instead.
//
// Errors are reported to the client as OM_EVT_CAN_ERROR from the CAN status
// change/error interrupt (CANx_SCE), which must be enabled in the NVIC along
// with the Rx FIFO 0 and Tx interrupts.

#include "om_can.h"
#include "om_pal_port_can.h"
#include "om.h"

OM_ASSERT_SET_FILE_NAME("om_can_stm32.c");

// Static events for attached clients
OM_EVENT(tx_complete_event, OM_EVT_CAN_TX_OK);


#define OM_CAN_STM32_MAX_INST 6

// Bus state changes and Rx overruns. Protocol errors (CAN_IT_LAST_ERROR_CODE)
// are left off: on a bus with no other node every retransmission is an ACK
// error, which would flood the client.
#define OM_CAN_STM32_ERROR_ITS (CAN_IT_ERROR_WARNING | \
                                CAN_IT_ERROR_PASSIVE | \
                                CAN_IT_BUSOFF |        \
                                CAN_IT_ERROR |         \
                                CAN_IT_RX_FIFO0_OVERRUN)

#define OM_CAN_STM32_ITS (CAN_IT_RX_FIFO0_MSG_PENDING | \
                          CAN_IT_TX_MAILBOX_EMPTY |     \
                          OM_CAN_STM32_ERROR_ITS)

// HAL error codes reported to the client, each as its own OM_EVT_CAN_ERROR.
// Tx errors only occur with automatic retransmission off (NART).
static const struct
{
  uint32_t error_code;
  const char *error_message;
} om_can_stm32_errors[] = {
    {HAL_CAN_ERROR_EWG, "CAN error warning"},
    {HAL_CAN_ERROR_EPV, "CAN error passive"},
    {HAL_CAN_ERROR_BOF, "CAN bus off"},
    {HAL_CAN_ERROR_RX_FOV0, "CAN Rx FIFO 0 overrun"},
    {HAL_CAN_ERROR_TX_ALST0, "CAN Tx mailbox 0 arbitration lost"},
    {HAL_CAN_ERROR_TX_TERR0, "CAN Tx mailbox 0 transmit error"},
    {HAL_CAN_ERROR_TX_ALST1, "CAN Tx mailbox 1 arbitration lost"},
    {HAL_CAN_ERROR_TX_TERR1, "CAN Tx mailbox 1 transmit error"},
    {HAL_CAN_ERROR_TX_ALST2, "CAN Tx mailbox 2 arbitration lost"},
    {HAL_CAN_ERROR_TX_TERR2, "CAN Tx mailbox 2 transmit error"},
};

static OmCan *om_can_instance_table[OM_CAN_STM32_MAX_INST];
static int om_can_instance_count = 0;

static void om_can_stm32_post_error(OmCan *self, uint32_t error_code, const char *error_message);

void om_can_stm32_init(OmCan *self, CAN_HandleTypeDef *handle)
{
  OM_ASSERT(om_can_instance_count < OM_CAN_STM32_MAX_INST);

  // Add instance to table for callback event lookup
  om_can_instance_table[om_can_instance_count] = self;

  // Increase instance count
  om_can_instance_count++;

  // Call base class init
  om_can_init(self);

  self->port.handle = handle;
}

void om_can_open(OmCan *self)
{
  if (HAL_CAN_Start(self->port.handle) != HAL_OK)
  {
    OM_ERROR();
  }

  if (HAL_CAN_ActivateNotification(self->port.handle, OM_CAN_STM32_ITS) != HAL_OK)
  {
    OM_ERROR();
  }
}

void om_can_close(OmCan *self)
{
  if (HAL_CAN_Stop(self->port.handle) != HAL_OK)
  {
    OM_ERROR();
  }

  if (HAL_CAN_DeactivateNotification(self->port.handle, OM_CAN_STM32_ITS) != HAL_OK)
  {
    OM_ERROR();
  }
}

void om_can_set_filters(OmCan *self, OmCanFilter const *const filter_list, int filter_count)
{
  static CAN_FilterTypeDef filter_config;

  // Currently only allowing 3 filters since we are using filter banks 0..2 based on index below
  OM_ASSERT(filter_count <= 3);

  for (int idx = 0; idx < filter_count; ++idx)
  {

    filter_config.FilterActivation = ENABLE;
    filter_config.FilterFIFOAssignment = CAN_FILTER_FIFO0;
    filter_config.FilterScale = CAN_FILTERSCALE_32BIT;
    filter_config.FilterMode = CAN_FILTERMODE_IDMASK;
    filter_config.FilterBank = idx;

    uint32_t filter_id = filter_list[idx].id;
    uint32_t filter_mask = filter_list[idx].mask;
    if (filter_list[idx].is_extended)
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
      OM_ERROR();
    }
  }
}

bool om_can_send(OmCan *self, OmCanFrame const *const frame)
{
  static CAN_TxHeaderTypeDef TxHeader;
  uint32_t TxMailbox;

  if (frame->id_is_extended)
  {
    TxHeader.ExtId = frame->id;
    TxHeader.IDE = CAN_ID_EXT;
  }
  else
  {
    TxHeader.StdId = frame->id;
    TxHeader.IDE = CAN_ID_STD;
  }
  TxHeader.RTR = CAN_RTR_DATA;
  TxHeader.DLC = frame->data_length;
  TxHeader.TransmitGlobalTime = DISABLE;

#if 0 // If the network has issues this will return an error, but we don't want to assert if that happens

  HAL_CAN_AddTxMessage(self->port.handle, &TxHeader, frame->data, &TxMailbox);
#else
  HAL_StatusTypeDef status = HAL_CAN_AddTxMessage(self->port.handle, &TxHeader, frame->data, &TxMailbox);
  if (status != HAL_OK)
  {
    return false;
  }
  else
  {
    return true;
  }
#endif
}

////////////////////// Private funcitons and STM32 HAL callbacks /////////////////
static void om_can_stm32_post_error(OmCan *self, uint32_t error_code, const char *error_message)
{
  OmCanErrorEvent *event = NULL;

  if (self->error_event == NULL)
  {
    // Use default signal
    event = OM_POOL_EVENT_NEW(OmCanErrorEvent, OM_EVT_CAN_ERROR);
  }
  else
  {
    // Use client specified signal
    event = (OmCanErrorEvent *)om_pool_alloc(sizeof(OmCanErrorEvent), self->error_event->signal, self->error_event->name);
  }

  event->can = self;
  event->error_code = error_code;
  event->error_message = error_message;

  OMA_MSG(self->client, event);
}

void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
  // The HAL accumulates error codes in the handle; take them and start afresh
  // so the next interrupt only reports what is new
  uint32_t const error_codes = HAL_CAN_GetError(hcan);
  HAL_CAN_ResetError(hcan);

  // Find matching instance
  for (int idx = 0; idx < om_can_instance_count; idx++)
  {
    if ((om_can_instance_table[idx]->port.handle == hcan) &&
        (om_can_instance_table[idx]->client != NULL))
    {
      for (size_t err = 0; err < sizeof(om_can_stm32_errors) / sizeof(om_can_stm32_errors[0]); err++)
      {
        if ((error_codes & om_can_stm32_errors[err].error_code) != 0u)
        {
          om_can_stm32_post_error(om_can_instance_table[idx],
                                  om_can_stm32_errors[err].error_code,
                                  om_can_stm32_errors[err].error_message);
        }
      }
    }
  }
}

void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan)
{
    // Find matching base instance and send TX Complete event
    for(int idx = 0; idx < om_can_instance_count; idx++)
    {
        if( (om_can_instance_table[idx]->port.handle == hcan) &&
            (om_can_instance_table[idx]->client != NULL) )
        {
            if(om_can_instance_table[idx]->tx_complete_event != NULL)
            {
                // Use client specified event
                OMA_MSG(om_can_instance_table[idx]->client, om_can_instance_table[idx]->tx_complete_event);                  
            }
            else
            {
                // Use default event
                OMA_MSG(om_can_instance_table[idx]->client, &tx_complete_event);                  
            }
        } 
    }
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  static CAN_RxHeaderTypeDef rx_header;

  // Find matching instance
  for (int idx = 0; idx < om_can_instance_count; idx++)
  {
    if ((om_can_instance_table[idx]->port.handle == hcan) &&
        (om_can_instance_table[idx]->client != NULL))
    {
      OmCanDataEvent *event = om_can_data_event_new_(om_can_instance_table[idx]);
      HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, event->frame.data);
      event->frame.data_length = rx_header.DLC;

      if (rx_header.IDE == CAN_ID_EXT)
      {
        event->frame.id = rx_header.ExtId;
        event->frame.id_is_extended = true;
      }
      else
      {
        event->frame.id = rx_header.StdId;
        event->frame.id_is_extended = false;
      }

      // Send CAN data to client
      OMA_MSG(om_can_instance_table[idx]->client, event);
    }
  }
}
