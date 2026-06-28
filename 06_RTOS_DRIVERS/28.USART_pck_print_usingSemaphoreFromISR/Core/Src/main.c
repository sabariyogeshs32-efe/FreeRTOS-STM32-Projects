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

SemaphoreHandle_t xRxDoneSemphr_handle;

static int rxInProgress = 0;

void data_handler_task(void *pvParameters);

void start_rx_interrupt(uint8_t* Buffer,uint8_t len);


static uint8_t* rxBuff = NULL;
static uint8_t  rxItr = 0;
//static uint8_t  rxlen = 0;
static uint8_t rxData[5];


int main(void)
{

  HAL_Init();


  SystemClock_Config();


  MX_GPIO_Init();

  xTaskCreate(data_handler_task,
  		  	  "Data Handler Task",
  			  256,
  			  NULL,
  			  1,
  			  &data_handler_task_handle
  			  );

  xRxDoneSemphr_handle = xSemaphoreCreateBinary();


  vTaskStartScheduler();

  while (1)
  {


  }

}

void start_rx_interrupt(uint8_t* Buffer,uint8_t len)
{

	  rxItr = 0;

	  rxBuff = Buffer;

	  rxInProgress = 1;
	  /*Enable the Interrupt in USART while receiving */
	  USART2->CR1 |= (1U<<5);

	  /*Set the priority for NVIC USART2 */
	  NVIC_SetPriority(USART2_IRQn,6);

	  /*Enable the NVIC for USART2 */
	  NVIC_EnableIRQ(USART2_IRQn);


}

static char rx_status[50];

void data_handler_task(void *pvParameters)
{
	USART2_UART_RX_Init();

	for(int i=0; i<5; i++)
	{
		rxData[i] = 0;
	}

	while(1)
	{

		start_rx_interrupt(rxData,5);

		if(xSemaphoreTake(xRxDoneSemphr_handle,portMAX_DELAY) == pdPASS)
		{
			if(rxItr == 5)
			{
				sprintf(rx_status,"Received");
			}
			else
			{
				sprintf(rx_status,"length miss matched");
			}
		}
		else
		{
			sprintf(rx_status,"timeout");
		}


	}
}


uint8_t rcData;

void USART2_IRQHandler(void)
{

	portBASE_TYPE xHigherPriorityTaskWoken = pdTRUE;


	if(USART2->SR & (1U<<5))
	{
		rcData = (uint8_t)USART2->DR;



			rxBuff[rxItr++] = rcData;

			if(rxItr == 5)
			{
				rxInProgress = 0;
				xSemaphoreGiveFromISR(xRxDoneSemphr_handle,&xHigherPriorityTaskWoken);
			}

		}





	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
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


