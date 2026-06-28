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

uint32_t analg_read;

uint32_t dig_read;

osSemaphoreId_t xTestSemaphore;

osThreadId_t btn_task,analog_task,middle_task;

void btn_read_task(void *pdParameters);
void middle_work_task(void *pdParameters);




int main(void)
{

  HAL_Init();


  SystemClock_Config();


  MX_GPIO_Init();
  USART2_UART_TX_Init();


  RCC->AHB1ENR |= (1U<<0);
  GPIOA->MODER |= (1U<<10);
  GPIOA->MODER &= ~(1U<<11);

  osKernelInitialize();


  xTestSemaphore = osSemaphoreNew(10,0,NULL);

  osThreadAttr_t btnAtt = {
		  .priority = osPriorityAboveNormal,
		  .name = "Button Task",
		  .stack_size = 1024		//1024Bytes
  };
  btn_task = osThreadNew(btn_read_task,NULL,&btnAtt);

  osThreadAttr_t MdlAttr = {
  		  .priority = osPriorityAboveNormal3,
  		  .name = "Middle Task",
  		  .stack_size = 1024		//1024Bytes
    };
  middle_task = osThreadNew(middle_work_task,NULL,&MdlAttr);



  printf("System Initializing... \r\n");
  osKernelStart();





  while (1)
  {



  }

}

void btn_read_task(void *pdParameters)
{
	  gpio_init();

	  while(1)
	  {
	      osSemaphoreAcquire(xTestSemaphore,portMAX_DELAY);


	      osKernelLock();
	      printf("Current TimeTick= %ld\r\n CONSUMER: Took Semaphore and Processed Event!\r\n",osKernelGetTickCount());
	      osKernelUnlock();

	      osKernelLock();
	      printf("Current TimeTick= %ld\r\n Current Count of CountingSemaphore= %ld\r\n",osKernelGetTickCount(),osSemaphoreGetCount(xTestSemaphore));
	      osKernelUnlock();
	      //vTaskSuspend(NULL);



	      osDelay(500);
	  }
}

void middle_work_task(void *pdParameters)
{

	vTaskDelay(100);


		osSemaphoreRelease(xTestSemaphore);

		osKernelLock();
	    printf("Current TimeTick= %ld\r\n PRODUCER1: Gave Semaphore\r\n",osKernelGetTickCount());
	    osKernelUnlock();

	   // vTaskDelay(1);
	    osSemaphoreRelease(xTestSemaphore);
	    osKernelLock();
	    printf("Current TimeTick= %ld\r\n PRODUCER2: Gave Semaphore\r\n",osKernelGetTickCount());
	    osKernelUnlock();
	    //
	    osSemaphoreRelease(xTestSemaphore);
	    osKernelLock();
	    printf("Current TimeTick= %ld\r\n PRODUCER3: Gave Semaphore\r\n",osKernelGetTickCount());
	    osKernelUnlock();

    osKernelLock();
	printf("Current TimeTick= %ld\r\n Current Count of CountingSemaphore= %ld\r\n",osKernelGetTickCount(),osSemaphoreGetCount(xTestSemaphore));
	osKernelUnlock();
	osThreadSuspend(middle_task);
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

//
//void vApplicationIdleHook(void)
//{
//	while(1)
//	{
//		//IdleTaskProfiler++;
//		printf("Current TimeTick= %ld\r\n NO TASK CURRENTLY RUNNING BY IDLE HOOK\r\n",xTaskGetTickCount());
//	}
//
//}


void Error_Handler(void)
{

  __disable_irq();
  while (1)
  {

  }

}

void exti_callback(void)
{

	printf("Button is Pressed!!! ");
	GPIOA->ODR ^= (1U<<5);

}



void EXTI15_10_IRQHandler(void)
{
	if((EXTI->PR & (1U<<13)))
	{
		EXTI->PR |= (1U<<13);

		exti_callback();

	}
}


