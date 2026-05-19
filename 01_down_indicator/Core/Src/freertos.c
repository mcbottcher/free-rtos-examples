/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "queue.h"
#include "lsm303dlhc.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} AccelData_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ACCEL_QUEUE_LEN   1 // Only want this as 1 to ensure the realtimedness of the system                                                  
#define TILT_THRESHOLD    200
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
static QueueHandle_t xAccelQueue;
static TaskHandle_t xAccelTaskHandle = NULL;
/* USER CODE END Variables */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
static void vAccelReadTask(void *pvParameters)
{
    int16_t raw[3];
    AccelData_t data;                                                        
  
    for (;;)                                                                 
    {           
        /* block until the DRDY interrupt fires */
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
                                                                              
        LSM303DLHC_AccReadXYZ(raw);                                          
        data.x = raw[0];                                                     
        data.y = raw[1];                                                     
        data.z = raw[2];

        xQueueSend(xAccelQueue, &data, 0);                                   
    }
}  

static void vLedTask(void *pvParameters)
{
    static const uint16_t led_pins[8] = {
        LD3_Pin,   /* N  - PE9  */
        LD5_Pin,   /* NE - PE10 */                                           
        LD7_Pin,   /* E  - PE11 */
        LD9_Pin,   /* SE - PE12 */                                           
        LD10_Pin,  /* S  - PE13 */                                           
        LD8_Pin,   /* SW - PE14 */
        LD6_Pin,   /* W  - PE15 */                                           
        LD4_Pin,   /* NW - PE8  */                                           
    };
    static const uint16_t all_leds =                                         
        LD3_Pin | LD4_Pin | LD5_Pin | LD6_Pin |
        LD7_Pin | LD8_Pin | LD9_Pin | LD10_Pin;
                                                                              
    AccelData_t data;
                                                                              
    for (;;)    
    {
        if (xQueueReceive(xAccelQueue, &data, portMAX_DELAY) != pdTRUE)
            continue;                                                        
  
        HAL_GPIO_WritePin(GPIOE, all_leds, GPIO_PIN_RESET);                  
                
        int16_t ax   = data.x;
        int16_t ay   = data.y;
        int16_t absx = ax < 0 ? -ax : ax;                                    
        int16_t absy = ay < 0 ? -ay : ay;
        int16_t mag  = absx > absy ? absx : absy;                            
                
        if (mag < TILT_THRESHOLD)                                            
            continue;
                                                                              
        int sector;
        if      (absx <= absy / 2 && ay > 0) sector = 0; /* N  */
        else if (absy <= absx / 2 && ax > 0) sector = 2; /* E  */
        else if (absx <= absy / 2 && ay < 0) sector = 4; /* S  */            
        else if (absy <= absx / 2 && ax < 0) sector = 6; /* W  */
        else if (ax > 0 && ay > 0)           sector = 1; /* NE */            
        else if (ax > 0 && ay < 0)           sector = 3; /* SE */
        else if (ax < 0 && ay < 0)           sector = 5; /* SW */            
        else                                 sector = 7; /* NW */            
  
        HAL_GPIO_WritePin(GPIOE, led_pins[sector], GPIO_PIN_SET);            
    }           
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == MEMS_INT3_Pin)
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        vTaskNotifyGiveFromISR(xAccelTaskHandle, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

void vStartFreeRTOSTasks(void)
{
    xAccelQueue = xQueueCreate(ACCEL_QUEUE_LEN, sizeof(AccelData_t));
    xTaskCreate(vAccelReadTask, "AccelRead", 128, NULL, 2, &xAccelTaskHandle);
    xTaskCreate(vLedTask,       "LedTask",   128, NULL, 1, NULL);            
}


/* USER CODE END Application */

