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

SemaphoreHandle_t xMutexSemaphore;

TaskHandle_t btn_task,analog_task,middle_task;

void btn_read_task(void *pdParameters);
void analog_read_task(void *pdParameters);
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

  /*Create Mutex Semaphore */
  xMutexSemaphore = xSemaphoreCreateMutex();

  /*Create Tasks */
  xTaskCreate(btn_read_task,
		  	 "Btn Read Task",
		  	  256,
			  NULL,
			  1,
			  &btn_task
			  );

  xTaskCreate(analog_read_task,
  		  	 "Analog Read Task",
  		  	  256,
  			  NULL,
  			  5,
  			  &analog_task
  			  );
  xTaskCreate(middle_work_task,
    		  	 "Middle Work Task",
    		  	  256,
    			  NULL,
    			  2,
    			  &middle_task
    			  );

  printf("System Initializing... \r\n");
  vTaskStartScheduler();





  while (1)
  {



  }

}

void btn_read_task(void *pdParameters)
{
	  gpio_init();

	  while(1)
	  {
	      if(xSemaphoreTake(xMutexSemaphore,portMAX_DELAY) == pdPASS)

	      {

	      vTaskSuspendAll();
	      printf("Current TimeTick= %ld\r\n LOW GOT MUTEX\r\n, The Current Priority of LOW: %ld\r\n",xTaskGetTickCount(),uxTaskPriorityGet(NULL));
	      xTaskResumeAll();

	      for(volatile uint32_t i=0;i<500000;i++);

	      vTaskSuspendAll();
	      printf("Current TimeTick= %ld\r\n LOW RELEASING MUTEX\r\n The Current Priority of LOW: %ld\r\n",xTaskGetTickCount(),uxTaskPriorityGet(NULL));
	      xTaskResumeAll();

	      xSemaphoreGive(xMutexSemaphore);
	      }

	      vTaskDelay(pdMS_TO_TICKS(1000));
	  }
}

void analog_read_task(void *pdParameters)
{
	  adc1_init();

	  while(1)
	  {
	      vTaskDelay(pdMS_TO_TICKS(200));

	      vTaskSuspendAll();
	      printf("Current TimeTick= %ld\r\n HIGH WANTS MUTEX\r\n",xTaskGetTickCount());
	      xTaskResumeAll();

	      xSemaphoreTake(xMutexSemaphore,portMAX_DELAY);

	      vTaskSuspendAll();
	      printf("Current TimeTick= %ld\r\n HIGH GOT MUTEX\r\n",xTaskGetTickCount());
	      xTaskResumeAll();

	      xSemaphoreGive(xMutexSemaphore);
	  }
}

void middle_work_task(void *pdParameters)
{


	while(1)
	{
		vTaskSuspendAll();
	    printf("Current TimeTick= %ld\r\n MEDIUM RUNNING\r\n",xTaskGetTickCount());
	    xTaskResumeAll();

	    vTaskDelay(1);
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


void vpplicationIdleHook(void)
{
	while(1)
	{
		//IdleTaskProfiler++;
	}

}


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


