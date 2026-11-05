// Copyright 2024 Lotus Engineering LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

// OmCan port for STM32 parts with the FDCAN peripheral (G0, G4, H5, H7, L5,
// U5, ...). Parts with the older bxCAN peripheral use om_can_stm32.c instead.
//
// Frames are classic CAN: up to OM_CAN_MAX_DATA_LENGTH bytes, no bit rate
// switching. Received frames that pass the filters land in Rx FIFO 0.
//
// The CubeMX generated init (MX_FDCANx_Init) configures the bit timing. The
// FDCAN interrupt line 0 (FDCANx_IT0) must be enabled in the NVIC for any of
// the callbacks below to run.

#include "om_can.h"
#include "om_pal_port_can.h"
#include "om.h"

#if defined(HAL_FDCAN_MODULE_ENABLED)

OM_ASSERT_SET_FILE_NAME("om_can_stm32_fdcan.c");

// Static events for attached clients
OM_EVENT(tx_complete_event, OM_EVT_CAN_TX_OK);


#define OM_CAN_STM32_MAX_INST 3

// Every Tx buffer, so a completion is reported whichever one the frame went out of
#define OM_CAN_FDCAN_TX_BUFFERS (FDCAN_TX_BUFFER0 | FDCAN_TX_BUFFER1 | FDCAN_TX_BUFFER2)

// Bus state changes reported to the client as OM_EVT_CAN_ERROR
#define OM_CAN_FDCAN_ERROR_STATUS_ITS (FDCAN_IT_BUS_OFF | FDCAN_IT_ERROR_PASSIVE | FDCAN_IT_ERROR_WARNING)

#define OM_CAN_FDCAN_ITS (FDCAN_IT_RX_FIFO0_NEW_MESSAGE | \
                          FDCAN_IT_RX_FIFO0_MESSAGE_LOST | \
                          FDCAN_IT_TX_COMPLETE |           \
                          OM_CAN_FDCAN_ERROR_STATUS_ITS)

static OmCan *om_can_instance_table[OM_CAN_STM32_MAX_INST];
static int om_can_instance_count = 0;


static OmCan *om_can_fdcan_find(FDCAN_HandleTypeDef const *hfdcan);
static void om_can_fdcan_set_filter_counts(FDCAN_HandleTypeDef *hfdcan, uint32_t std_count, uint32_t ext_count);
static void om_can_fdcan_post_error(OmCan *self, uint32_t error_code, const char *error_message);


void om_can_stm32_init(OmCan *self, FDCAN_HandleTypeDef *handle)
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
  if (HAL_FDCAN_ActivateNotification(self->port.handle, OM_CAN_FDCAN_ITS, OM_CAN_FDCAN_TX_BUFFERS) != HAL_OK)
  {
    OM_ERROR();
  }

  if (HAL_FDCAN_Start(self->port.handle) != HAL_OK)
  {
    OM_ERROR();
  }
}

void om_can_close(OmCan *self)
{
  if (HAL_FDCAN_Stop(self->port.handle) != HAL_OK)
  {
    OM_ERROR();
  }

  if (HAL_FDCAN_DeactivateNotification(self->port.handle, OM_CAN_FDCAN_ITS) != HAL_OK)
  {
    OM_ERROR();
  }
}

/// Filters can only be changed while the FDCAN is stopped, so call this before
/// om_can_open() or after om_can_close(). Each filter is a classic id/mask
/// pair: a frame matches when (frame_id & mask) == (id & mask). With no
/// filters every data frame is received; with any, only matching ones are.
/// Remote frames are always rejected.
void om_can_set_filters(OmCan *self, OmCanFilter const *const filter_list, int filter_count)
{
  FDCAN_HandleTypeDef *const hfdcan = self->port.handle;
  FDCAN_FilterTypeDef filter_config;
  uint32_t std_count = 0;
  uint32_t ext_count = 0;

  OM_ASSERT(hfdcan->State == HAL_FDCAN_STATE_READY);
  OM_ASSERT(filter_count >= 0);

  for (int idx = 0; idx < filter_count; ++idx)
  {
    if (filter_list[idx].is_extended)
    {
      ext_count++;
    }
    else
    {
      std_count++;
    }
  }

  // Size the filter lists before writing them - the HAL places each filter by index into its list
  om_can_fdcan_set_filter_counts(hfdcan, std_count, ext_count);

  std_count = 0;
  ext_count = 0;
  for (int idx = 0; idx < filter_count; ++idx)
  {
    filter_config.FilterType = FDCAN_FILTER_MASK;
    filter_config.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
    filter_config.FilterID1 = filter_list[idx].id;
    filter_config.FilterID2 = filter_list[idx].mask;

    if (filter_list[idx].is_extended)
    {
      filter_config.IdType = FDCAN_EXTENDED_ID;
      filter_config.FilterIndex = ext_count++;
    }
    else
    {
      filter_config.IdType = FDCAN_STANDARD_ID;
      filter_config.FilterIndex = std_count++;
    }

    if (HAL_FDCAN_ConfigFilter(hfdcan, &filter_config) != HAL_OK)
    {
      /* Filter configuration Error */
      OM_ERROR();
    }
  }

  // Without filters accept everything, matching the reset behaviour. With
  // filters, anything they do not match is dropped.
  uint32_t const non_matching = (filter_count == 0) ? FDCAN_ACCEPT_IN_RX_FIFO0 : FDCAN_REJECT;
  if (HAL_FDCAN_ConfigGlobalFilter(hfdcan,
                                   non_matching,
                                   non_matching,
                                   FDCAN_REJECT_REMOTE,
                                   FDCAN_REJECT_REMOTE) != HAL_OK)
  {
    OM_ERROR();
  }
}

/// Queues a classic CAN data frame. Returns false if it could not be queued -
/// the Tx FIFO is full, which is normal while the bus is busy or down, or the
/// FDCAN is not open. OM_EVT_CAN_TX_OK follows once it has been sent.
bool om_can_send(OmCan *self, OmCanFrame const *const frame)
{
  FDCAN_TxHeaderTypeDef tx_header;

  OM_ASSERT(frame->data_length <= OM_CAN_MAX_DATA_LENGTH);

  tx_header.Identifier = frame->id;
  tx_header.IdType = frame->id_is_extended ? FDCAN_EXTENDED_ID : FDCAN_STANDARD_ID;
  tx_header.TxFrameType = FDCAN_DATA_FRAME;
  tx_header.DataLength = frame->data_length; // Classic CAN DLC codes 0..8 are the byte count
  tx_header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  tx_header.BitRateSwitch = FDCAN_BRS_OFF;
  tx_header.FDFormat = FDCAN_CLASSIC_CAN;
  tx_header.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
  tx_header.MessageMarker = 0;

  return HAL_FDCAN_AddMessageToTxFifoQ(self->port.handle, &tx_header, frame->data) == HAL_OK;
}

////////////////////// Private functions /////////////////

static OmCan *om_can_fdcan_find(FDCAN_HandleTypeDef const *hfdcan)
{
  for (int idx = 0; idx < om_can_instance_count; idx++)
  {
    if (om_can_instance_table[idx]->port.handle == hfdcan)
    {
      return om_can_instance_table[idx];
    }
  }
  return NULL;
}

#if defined(FDCAN_RXGFC_LSS)
// Fixed message RAM (G0, G4, H5, L5, U5): every instance always has room for
// 28 standard and 8 extended filter elements. The list lengths are just
// register fields, writable while CCE is set (READY state), so grow or shrink
// them to fit the filters being set.
#define OM_CAN_FDCAN_MAX_STD_FILTERS 28u
#define OM_CAN_FDCAN_MAX_EXT_FILTERS 8u

static void om_can_fdcan_set_filter_counts(FDCAN_HandleTypeDef *hfdcan, uint32_t std_count, uint32_t ext_count)
{
  OM_ASSERT(std_count <= OM_CAN_FDCAN_MAX_STD_FILTERS);
  OM_ASSERT(ext_count <= OM_CAN_FDCAN_MAX_EXT_FILTERS);

  hfdcan->Init.StdFiltersNbr = std_count;
  hfdcan->Init.ExtFiltersNbr = ext_count;
  MODIFY_REG(hfdcan->Instance->RXGFC,
             FDCAN_RXGFC_LSS | FDCAN_RXGFC_LSE,
             (std_count << FDCAN_RXGFC_LSS_Pos) | (ext_count << FDCAN_RXGFC_LSE_Pos));
}
#else
// Configurable message RAM (H7, MP1): HAL_FDCAN_Init() laid out the RAM from
// Init.StdFiltersNbr / ExtFiltersNbr, so the filters must fit what was
// reserved there - set those counts in CubeMX.
static void om_can_fdcan_set_filter_counts(FDCAN_HandleTypeDef *hfdcan, uint32_t std_count, uint32_t ext_count)
{
  OM_ASSERT(std_count <= hfdcan->Init.StdFiltersNbr);
  OM_ASSERT(ext_count <= hfdcan->Init.ExtFiltersNbr);
}
#endif

static void om_can_fdcan_post_error(OmCan *self, uint32_t error_code, const char *error_message)
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

////////////////////// STM32 HAL callbacks /////////////////

void HAL_FDCAN_TxBufferCompleteCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t BufferIndexes)
{
  OmCan *const self = om_can_fdcan_find(hfdcan);
  if ((self == NULL) || (self->client == NULL))
  {
    return;
  }

  // Several buffers can finish in one interrupt; report each frame sent
  for (uint32_t buffers = BufferIndexes; buffers != 0u; buffers &= (buffers - 1u))
  {
    if (self->tx_complete_event != NULL)
    {
      // Use client specified event
      OMA_MSG(self->client, self->tx_complete_event);
    }
    else
    {
      // Use default event
      OMA_MSG(self->client, &tx_complete_event);
    }
  }
}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
  FDCAN_RxHeaderTypeDef rx_header;
  OmCan *const self = om_can_fdcan_find(hfdcan);

  if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != 0u)
  {
    // Drain the FIFO. Frames are still read with no client attached, so a
    // full FIFO does not hold up whatever arrives once one is.
    while (HAL_FDCAN_GetRxFifoFillLevel(hfdcan, FDCAN_RX_FIFO0) > 0u)
    {
      uint8_t data[64]; // Largest FD payload, in case an FD frame gets through
      if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rx_header, data) != HAL_OK)
      {
        break;
      }

      if ((self == NULL) || (self->client == NULL))
      {
        continue;
      }

      OmCanDataEvent *event = om_can_data_event_new_(self);

      // Classic frames are at most 8 bytes; an FD frame is cut to fit
      uint8_t const length = (rx_header.DataLength > OM_CAN_MAX_DATA_LENGTH) ? OM_CAN_MAX_DATA_LENGTH
                                                                            : (uint8_t)rx_header.DataLength;
      for (uint8_t idx = 0; idx < length; idx++)
      {
        event->frame.data[idx] = data[idx];
      }
      event->frame.data_length = length;
      event->frame.id = rx_header.Identifier;
      event->frame.id_is_extended = (rx_header.IdType == FDCAN_EXTENDED_ID);

      // Send CAN data to client
      OMA_MSG(self->client, event);
    }
  }

  if (((RxFifo0ITs & FDCAN_IT_RX_FIFO0_MESSAGE_LOST) != 0u) &&
      (self != NULL) && (self->client != NULL))
  {
    om_can_fdcan_post_error(self, FDCAN_IT_RX_FIFO0_MESSAGE_LOST, "FDCAN Rx FIFO 0 overrun");
  }
}

void HAL_FDCAN_ErrorStatusCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t ErrorStatusITs)
{
  OmCan *const self = om_can_fdcan_find(hfdcan);
  uint32_t const status = hfdcan->Instance->PSR;

  // Bus-off leaves the FDCAN in initialisation mode. Clearing INIT starts the
  // recovery sequence (128 x 11 recessive bits), the equivalent of bxCAN's
  // automatic bus-off management.
  if (((ErrorStatusITs & FDCAN_IT_BUS_OFF) != 0u) && ((status & FDCAN_PSR_BO) != 0u))
  {
    CLEAR_BIT(hfdcan->Instance->CCCR, FDCAN_CCCR_INIT);
  }

  if ((self == NULL) || (self->client == NULL))
  {
    return;
  }

  // Each interrupt marks a change, entering or leaving the state, so the
  // status register says which
  if ((ErrorStatusITs & FDCAN_IT_BUS_OFF) != 0u)
  {
    om_can_fdcan_post_error(self, FDCAN_IT_BUS_OFF,
                            ((status & FDCAN_PSR_BO) != 0u) ? "FDCAN bus off" : "FDCAN bus off cleared");
  }
  if ((ErrorStatusITs & FDCAN_IT_ERROR_PASSIVE) != 0u)
  {
    om_can_fdcan_post_error(self, FDCAN_IT_ERROR_PASSIVE,
                            ((status & FDCAN_PSR_EP) != 0u) ? "FDCAN error passive" : "FDCAN error active");
  }
  if ((ErrorStatusITs & FDCAN_IT_ERROR_WARNING) != 0u)
  {
    om_can_fdcan_post_error(self, FDCAN_IT_ERROR_WARNING,
                            ((status & FDCAN_PSR_EW) != 0u) ? "FDCAN error warning" : "FDCAN error warning cleared");
  }
}

#endif // HAL_FDCAN_MODULE_ENABLED
