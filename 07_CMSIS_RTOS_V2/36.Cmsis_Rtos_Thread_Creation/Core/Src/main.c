#include "main.h"
#include "cmsis_os.h"
#include <stdio.h>
#include "stm32f446xx.h"
#include <stdint.h>
#include "uart.h"
#include "exti_pc13.h"
#include "adc.h"

void SystemClock_Config(void);
static void MX_GPIO_Init(void);

void simple_Thread1(void* arguments);
void simple_Thread2(void* arguments);

uint32_t Thread1Count, Thread2Count;

const char name1[20] = "Simple Thread 1";
const char name2[20] = "Simple Thread 2";

const osThreadAttr_t Thread1_attr = {
		.name = name1,
		.priority = osPriorityAboveNormal,
		.stack_size = 1024
};

const osThreadAttr_t Thread2_attr = {
		.name = name2,
		.priority = osPriorityHigh7,
		.stack_size = 1024
};

int main(void)
{

  HAL_Init();

  USART2_UART_TX_Init();

  SystemClock_Config();


  MX_GPIO_Init();

  RCC->AHB1ENR |= (1U<<0);
  GPIOA->MODER |= (1U<<10);
  GPIOA->MODER &= ~(1U<<11);

  osKernelInitialize();

  osThreadNew(simple_Thread1,NULL,&Thread1_attr);

  osThreadNew(simple_Thread2,NULL,&Thread2_attr);

  osKernelStart();



  while (1)
  {
	  printf("No Threads were Created yet... \r\n");

  }

}


void simple_Thread1(void* arguments)
{
	while(1)
	{
		Thread1Count++;
		osKernelLock();
		printf("Simple Thread1 has came to running state! \r\n");
//		GPIOA->ODR ^= (1U<<5);
		osKernelUnlock();
//
//		osDelay(1000);

	}
}

void simple_Thread2(void* arguments)
{
	while(1)
	{
		Thread2Count++;
		osKernelLock();
		printf("Simple Thread2 has came to running state! \r\n");
		GPIOA->ODR ^= (1U<<5);
		osKernelUnlock();
//
//		osDelay(1000);

	}
}


void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};


  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);


  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }


  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}


static void MX_GPIO_Init(void)
{

  __HAL_RCC_GPIOA_CLK_ENABLE();

}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	/*This is our Time Base remember we have selected the Timer1 for the FreeRTOS TimeBase... */

  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }

}


void vApplicationIdleHook(void)
{
	while(1)
	{
		//printf("No Threads were in ready state now... \r\n");
	}

}


void Error_Handler(void)
{

  __disable_irq();
  while (1)
  {

  }

}



//-------------------------------------------------------




/*

common api s for cmsis rtos :

osDelay();

osDelayUntil();

osKernelInitialize();

osThreadNew();

osThreadSetPriority();

osThreadGetPriority();

osThreadSuspend();

osThreadResume();

osThreadYield();

osThreadExit();

osThreadTerminate();

osKernelStart();

osMessageQueueNew();

osMessageQueueGet();

osMessageQueuePut();

osMessageQueueDelete();

osSemaphoreNew();

osSemaphoreAcquire();

osSemaphoreRelease();

osSemaphoreDelete();

osSemaphoreGetCount();

osMutexRelease();

osMutexAcquire();

osMutexNew();

osMutexDelete();

osKernelUnlock();

osKernelGetTickCount();

osTimerIsRunning();

osTimerNew();

osTimerDelete();

osTimerGetName();

osEventFlagsNew();

osEventFlagsSet();

osEventFlagsClear();

osEventFlagsGet();

osEventFlagsWait();

osEventFlagsDelete();

//  cmsis rtos task creation function get arguments for stack size in bytes while in free rtos takes in word (4Bytes...) as argument for stack size...
//
//  cmsis rtos functions uses structures as input (arguments...)
//
//



*/

