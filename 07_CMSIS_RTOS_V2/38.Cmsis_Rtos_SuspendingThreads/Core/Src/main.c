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


uint32_t BlueCount,GreenCount,RedCount,OrangeCount;

void BlueThread(void* arguments);
void GreenThread(void* arguments);
void RedThread(void* arguments);
void OrangeThread(void* arguments);

osThreadId_t BlueId,GreenId,RedId,OrangeId;


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

  osThreadAttr_t BlueThread_Attrbt = {
		  .name = "Blue Thread",
		  .priority = osPriorityAboveNormal,
		  .stack_size = 1024			//1024 Bytes - stack size
  };
  BlueId = osThreadNew(BlueThread,NULL,&BlueThread_Attrbt);

  osThreadAttr_t GreenThread_Attrbt = {
 		  .name = "Green Thread",
 		  .priority = osPriorityAboveNormal,
 		  .stack_size = 1024			//1024 Bytes - stack size
   };
  GreenId = osThreadNew(GreenThread,NULL,&GreenThread_Attrbt);

  osThreadAttr_t RedThread_Attrbt = {
  		  .name = "Red Thread",
  		  .priority = osPriorityAboveNormal,
  		  .stack_size = 1024			//1024 Bytes - stack size
    };
  RedId = osThreadNew(RedThread,NULL,&RedThread_Attrbt);

  osThreadAttr_t OrangeThread_Attrbt = {
   		  .name = "Orange Thread",
   		  .priority = osPriorityAboveNormal,
   		  .stack_size = 1024			//1024 Bytes - stack size
     };
  OrangeId = osThreadNew(OrangeThread,NULL,&OrangeThread_Attrbt);

  osKernelStart();






  while (1)
  {
	  printf("No Threads were Created yet... \r\n");

  }

}


void BlueThread(void* arguments)
{
	while(1)
	{
		BlueCount++;
	}
}


void GreenThread(void* arguments)
{
	while(1)
	{
		GreenCount++;
	}
}


void RedThread(void* arguments)
{
	while(1)
	{
		RedCount++;
	}
}


void OrangeThread(void* arguments)
{
	while(1)
	{
		OrangeCount++;

		if(OrangeCount >= 2384293)
		{
			osThreadSetPriority(OrangeId,osPriorityHigh7);
		}
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

