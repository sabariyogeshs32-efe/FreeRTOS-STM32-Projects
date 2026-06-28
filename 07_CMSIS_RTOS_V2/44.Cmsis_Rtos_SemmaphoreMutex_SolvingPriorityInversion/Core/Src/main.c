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


osMutexId_t osMutex_Id;

void BlueThread(void* arguments);
void GreenThread(void* arguments);
void RedThread(void* arguments);

osThreadId_t BlueId,GreenId,RedId;

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

  osMutex_Id = osMutexNew(NULL);

  osThreadAttr_t Blue_Attribute = {

		  	 	 	 .name = "Blue Thread Middle Priority ",
					 .priority = osPriorityAboveNormal,
					 .stack_size = 1024 	 	// 1024 Bytes
  };
  BlueId = osThreadNew(BlueThread,NULL,&Blue_Attribute);

  osThreadAttr_t Green_Attribute = {

  		  	 	 	 .name = "Green Thread Lowest Priority",
  					 .priority = osPriorityNormal,
  					 .stack_size = 1024 	 	// 1024 Bytes
  };
  GreenId = osThreadNew(GreenThread,NULL,&Green_Attribute);

  osThreadAttr_t Red_Attribute = {

    		  	 	  .name = "Red Thread Highest Priority",
    				  .priority = osPriorityHigh,
    				  .stack_size = 1024 	 	// 1024 Bytes
  };
  RedId = osThreadNew(RedThread,NULL,&Red_Attribute);

  printf("System Initializing... \r\n");

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

		osKernelLock();
		printf("Middle Task is Running... current Tick rate is=%ld \r\n",osKernelGetTickCount());
		osKernelUnlock();

		osDelay(10);


	}
}


void GreenThread(void* arguments)
{
	while(1)
	{
		osMutexAcquire(osMutex_Id,portMAX_DELAY);

		osKernelLock();
		printf("Low Task Acquired Semaphore... current Tick rate is=%ld \nCurrent Priority of LOW TASk=%d \r\n",osKernelGetTickCount(),osThreadGetPriority(GreenId));
		osKernelUnlock();

		for(int i=0; i<500000; i++){}


		printf("Low Task Released Semaphore... current Tick rate is=%ld \nCurrent Priority of LOW TASk=%d \r\n",osKernelGetTickCount(),osThreadGetPriority(GreenId));


		osSemaphoreRelease(osMutex_Id);




	}
}


void RedThread(void* arguments)
{
	while(1)
	{
		osDelay(200);

		osKernelLock();
		printf("High Task WANT Semaphore... current Tick rate is=%ld \r\n",osKernelGetTickCount());
		osKernelUnlock();

		osSemaphoreAcquire(osMutex_Id,portMAX_DELAY);

		osKernelLock();
		printf("High Task Acquired Semaphore... current Tick rate is=%ld \r\n",osKernelGetTickCount());
		osKernelUnlock();

		osSemaphoreRelease(osMutex_Id);

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
