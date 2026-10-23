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
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdint.h>
#include <math.h>
#include "semphr.h"
#include "usart.h"
#include "i2c.h"
#include "tim.h"
#include "vl53l0x_api.h"
#include "controle.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

SemaphoreHandle_t semaphore_courant;
SemaphoreHandle_t semaphore_position;
uint32_t adc_raw = 0;
volatile uint16_t consigne_position = 65; // Hauteur du masse avec fil eloigné
volatile uint16_t position = 65;
static float consigne_puissance = 0;
volatile float courant = 0;
volatile float puissance = 0;
volatile float duty = 0;
volatile float tension_fil = 0;
VL53L0X_Dev_t vl53l0x_dev; 							// Handle du vl53l0x
VL53L0X_Error status;								// Status d'execution des fonctions
VL53L0X_RangingMeasurementData_t RangingData; 		// Variable de stockage de la mesure

/* USER CODE END Variables */
/* Definitions for boucle_courant */
osThreadId_t boucle_courantHandle;
const osThreadAttr_t boucle_courant_attributes = {
  .name = "boucle_courant",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for boucle_position */
osThreadId_t boucle_positionHandle;
const osThreadAttr_t boucle_position_attributes = {
  .name = "boucle_position",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for test */
osThreadId_t testHandle;
const osThreadAttr_t test_attributes = {
  .name = "test",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void ctrl_courant(void *argument);
void ctrl_position(void *argument);
void test_task(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);

/* USER CODE BEGIN 4 */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
   /* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
   called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	HAL_TIM_Base_Stop_IT(&htim2);
	HAL_TIM_Base_Stop_IT(&htim5);
	while (1) {

	}
}
/* USER CODE END 4 */

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

	semaphore_courant = xSemaphoreCreateBinary();
	semaphore_position = xSemaphoreCreateBinary();


  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of boucle_courant */
  boucle_courantHandle = osThreadNew(ctrl_courant, NULL, &boucle_courant_attributes);

  /* creation of boucle_position */
  boucle_positionHandle = osThreadNew(ctrl_position, NULL, &boucle_position_attributes);

  /* creation of test */
  testHandle = osThreadNew(test_task, NULL, &test_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_ctrl_courant */
/**
  * @brief  Function implementing the boucle_courant thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_ctrl_courant */
void ctrl_courant(void *argument)
{
  /* USER CODE BEGIN ctrl_courant */
	float consigne_courant = 0;
	float consigne_tension = 0;
	duty = 0;

  /* Infinite loop */
  while (1) {
    xSemaphoreTake(semaphore_courant, portMAX_DELAY);
    //HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

    // I* = sqrt(P*/R) -> P* = I*^2 x R
    consigne_courant = sqrtf(consigne_puissance/R);

    consigne_tension = consigne_courant * R;

    duty = consigne_tension / VCC;

    set_pwm_duty(duty);

    puissance = courant * consigne_tension;
    tension_fil = consigne_tension;
  }
  /* USER CODE END ctrl_courant */
}

/* USER CODE BEGIN Header_ctrl_position */
/**
* @brief Function implementing the boucle_position thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ctrl_position */
void ctrl_position(void *argument)
{
  /* USER CODE BEGIN ctrl_position */
	VL53L0X_config(&vl53l0x_dev, &hi2c1, &status);

	if (status != VL53L0X_ERROR_NONE) {
		HAL_UART_Transmit(&huart2, (uint8_t*)"Error config\r\n", 14, 1000);
	}

	float position_raw = 65;

  /* Infinite loop */
  while (1) {
    xSemaphoreTake(semaphore_position, portMAX_DELAY);
    //HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

    // Sampling de l'hauter
    position_raw = VL53L0X_LeerDistanciaMM(&vl53l0x_dev, &RangingData, &status);

    // Filtrage IIR
    position = iir(position_raw);

    int16_t erreur = consigne_position - position;

    consigne_puissance = pid_position(erreur);
  }
  /* USER CODE END ctrl_position */
}

/* USER CODE BEGIN Header_test_task */
/**
* @brief Function implementing the test thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_test_task */
void test_task(void *argument)
{
  /* USER CODE BEGIN test_task */
	char cadena[32] = {0};
	int n = 0;
  /* Infinite loop */
  while (1) {
    vTaskDelay(portMAX_DELAY);
    //HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    n = snprintf(cadena, sizeof(cadena), "P = %u\n", VL53L0X_LeerDistanciaMM(&vl53l0x_dev, &RangingData, &status));
    HAL_UART_Transmit(&huart2, (uint8_t *)cadena, n, 100);
  }
  /* USER CODE END test_task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

