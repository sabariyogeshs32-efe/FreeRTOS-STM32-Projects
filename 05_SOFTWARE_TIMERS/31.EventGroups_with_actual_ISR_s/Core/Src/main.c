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

#define TASK_0_BIT			(1U<<0)
#define TASK_1_BIT			(1U<<1)

EventGroupHandle_t xEventGroupTest_handle;



TaskHandle_t xEventBitReading_task_handle;

static void xEventBitReading_task(void* pvParameters);

void USART2_IRQHandler(void);
void EXTI15_10_IRQHandler(void);


int main(void)
{

  HAL_Init();


  SystemClock_Config();


  MX_GPIO_Init();

  USART2_UART_RX_Init();

  /*Enable the Interrupt in USART while receiving */
  USART2->CR1 |= (1U<<5);

  /*Set priority of uartrx interrupt and enable NVIC */
  NVIC_SetPriority(USART2_IRQn,7);

  NVIC_EnableIRQ(USART2_IRQn);

  /*As same as in uart Do it for PC13 EXTI13 */
  exti_pc13_init();


  /*Lets initialize userLED for indication */
  RCC->AHB1ENR |= (1U<<0);

  GPIOA->MODER &= ~(1U<<11);
  GPIOA->MODER |=  (1U<<10);



//  printf("System Initializing... \r\n");

  xEventGroupTest_handle = xEventGroupCreate();


  xTaskCreate(xEventBitReading_task,
  		  	  "Even Bit Setter Task",
  			  256,
  			  NULL,
  			  1,
			  &xEventBitReading_task_handle
  			  );

  vTaskStartScheduler();

  while (1)
  {

  }

}
/*Define those two ISRs Interrupt service routines */

BaseType_t test1;

char data;

/*1.ISR for UART_RX */
void USART2_IRQHandler(void)
{
	EventBits_t Bit0_EG = TASK_0_BIT;

	BaseType_t pxHigherPriorityTaskWoken = pdFALSE;

	if(USART2->SR & (1U<<5))
	{
		test1 = xEventGroupSetBitsFromISR(xEventGroupTest_handle,Bit0_EG,&pxHigherPriorityTaskWoken);
		data = USART2->DR;

	}

	portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
}

BaseType_t test2;


/*2.ISR for EXTI13 */
void EXTI15_10_IRQHandler(void)
{
	EventBits_t Bit1_EG = TASK_1_BIT;

	BaseType_t pxHigherPriorityTaskWoken = pdFALSE;

	if(EXTI->PR & (1U<<13))
	{

		test2 = xEventGroupSetBitsFromISR(xEventGroupTest_handle,Bit1_EG,&pxHigherPriorityTaskWoken);
		EXTI->PR |= (1U<<13);

	}

	portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
}





EventBits_t xEventGroupStatus;


static void xEventBitReading_task(void* pvParameters)
{

	const EventBits_t uxBitsToWaitFor = (TASK_0_BIT | TASK_1_BIT);

	const BaseType_t xClearOnExit = pdTRUE;

	const BaseType_t xWaitForAllBits = pdTRUE;

	while(1)
	{

		xEventGroupStatus = xEventGroupWaitBits(xEventGroupTest_handle,uxBitsToWaitFor,xClearOnExit,xWaitForAllBits,portMAX_DELAY);

		GPIOA->ODR ^= (1U<<5);

		vTaskDelay(pdMS_TO_TICKS(200));


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
