/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "blinky.h"
#include "om.h"
#include "usart.h"



/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
#define OM_TRACE_TICK_MSEC 100
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for om_trace_timer */
osTimerId_t om_trace_timerHandle;
const osTimerAttr_t om_trace_timer_attributes = {
  .name = "om_trace_timer"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
// Printf Support
//#if defined(__GNUC__)
int _write(int fd, char * ptr, int len)
{
  HAL_UART_Transmit(&huart3, (uint8_t *) ptr, len, HAL_MAX_DELAY);
  return len;
}
//#endif

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void om_trace_timer_cb(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* creation of om_trace_timer */
  om_trace_timerHandle = osTimerNew(om_trace_timer_cb, osTimerPeriodic, NULL, &om_trace_timer_attributes);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  blinky_actors_start();

  // Start Trace timer
  osTimerStart(om_trace_timerHandle, OM_TRACE_TICK_MSEC);
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  printf("\n\n\n");
  printf("***************************************\n");
  printf("*** Blinky        - FreeRTOS        ***\n");
  printf("***************************************\n");

  
  /* Infinite loop */
  OmTraceLogEntry trace_record;
  for(;;)
  {
    if(om_trace_read(&blinky_trace, &trace_record))
    {
      printf("%lu:%s\n", (uint32_t)trace_record.timestamp_usec, trace_record.message);
//      HAL_UART_Transmit(&huart3, (uint8_t *) trace_record.message, strnlen(trace_record.message, OM_TRACE_MAX_MESSAGE_LENGTH), HAL_MAX_DELAY);
//      HAL_UART_Transmit(&huart3, (uint8_t *) "\n", 1, HAL_MAX_DELAY);
//      printf("\n"); // Fun fact, seems it doesn't go out unless you put a LF
    }
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* om_trace_timer_cb function */
void om_trace_timer_cb(void *argument)
{
  /* USER CODE BEGIN om_trace_timer_cb */
  om_trace_tick(&blinky_trace, OM_TRACE_TICK_MSEC);
  /* USER CODE END om_trace_timer_cb */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

