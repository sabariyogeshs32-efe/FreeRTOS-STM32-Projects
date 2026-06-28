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

TaskHandle_t xNotif_Reveiving_Task_handle;

void xNotif_Reveiving_Task(void* pvParameters);



int main(void)
{

  HAL_Init();


  SystemClock_Config();


  MX_GPIO_Init();

  USART2_UART_TX_Init();

//  /*Enable the Interrupt in USART while receiving */
//  USART2->CR1 |= (1U<<5);
//
//  /*Set priority of uartrx interrupt and enable NVIC */
//  NVIC_SetPriority(USART2_IRQn,7);
//
//  NVIC_EnableIRQ(USART2_IRQn);

  /*As same as in uart Do it for PC13 EXTI13 */
  exti_pc13_init();


  /*Lets initialize userLED for indication */
  RCC->AHB1ENR |= (1U<<0);

  GPIOA->MODER &= ~(1U<<11);
  GPIOA->MODER |=  (1U<<10);



  printf("System Initializing... \r\n");

  xTaskCreate(xNotif_Reveiving_Task,
		  	  "Notif Receiving Task",
			  128,
			  NULL,
			  1,
			  &xNotif_Reveiving_Task_handle
			  );





  vTaskStartScheduler();

  while (1)
  {

  }

}



void xNotif_Reveiving_Task(void* pvParameters)
{
	while(1)
	{
		if((ulTaskNotifyTake(pdFALSE,pdMS_TO_TICKS(100))) != 0)
		{
			printf("Button Pressed and notified from ISR \r\n");
			GPIOA->ODR ^= (1U<<5);
		}
	}
}

/*ISR for EXTI13 */
void EXTI15_10_IRQHandler(void)
{


	BaseType_t pxHigherPriorityTaskWoken = pdFALSE;

	if(EXTI->PR & (1U<<13))
	{
		vTaskNotifyGiveFromISR(xNotif_Reveiving_Task_handle,&pxHigherPriorityTaskWoken);

		EXTI->PR |= (1U<<13);

	}

	portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
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
