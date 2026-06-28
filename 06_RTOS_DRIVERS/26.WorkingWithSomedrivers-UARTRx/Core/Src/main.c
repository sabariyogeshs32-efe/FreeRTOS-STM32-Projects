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



TaskHandle_t polling_task_handle,data_handler_task_handle;


QueueHandle_t xDataHandlerQueue_handle;

void data_handler_task(void *pvParameters);
void polling_task(void *pvParameters);





int main(void)
{

  HAL_Init();


  SystemClock_Config();


  MX_GPIO_Init();

  xTaskCreate(polling_task,
		  	  "Polling Task",
			  256,
			  NULL,
			  1,
			  &polling_task_handle
			  );

  xTaskCreate(data_handler_task,
  		  	  "Data Handler Task",
  			  256,
  			  NULL,
  			  1,
  			  &data_handler_task_handle
  			  );


  xDataHandlerQueue_handle = xQueueCreate(10,sizeof(uint8_t));

  vTaskStartScheduler();

  while (1)
  {


  }

}

uint8_t rcData;
uint8_t rcData1;
void polling_task(void *pvParameters)
{
	USART2_UART_RX_Init();
	while(1)
	{

		rcData = uart_read();

		xQueueSend(xDataHandlerQueue_handle,&rcData,0);

	}
}


void data_handler_task(void *pvParameters)
{
//	USART2_UART_TX_Init();
	while(1)
	{
		xQueueReceive(xDataHandlerQueue_handle,&rcData1,portMAX_DELAY);

//		printf("The received Data From the UARTRx is : %c \r\n",rcData1);

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


