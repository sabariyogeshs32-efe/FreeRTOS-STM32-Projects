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



TaskHandle_t xEventBitSetting_task_handle,xEventBitReading_task_handle;

static void xEventBitSetting_task(void* pvParameters);
static void xEventBitReading_task(void* pvParameters);


int main(void)
{

  HAL_Init();


  SystemClock_Config();


  MX_GPIO_Init();

  USART2_UART_TX_Init();

  printf("System Initializing... \r\n");

  xEventGroupTest_handle = xEventGroupCreate();

  xTaskCreate(xEventBitSetting_task,
		  	  "Even Bit Setter Task",
			  256,
			  NULL,
			  1,
			  &xEventBitSetting_task_handle
			  );

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

static void xEventBitSetting_task(void* pvParameters)
{
	while(1)
	{
		vTaskDelay(pdMS_TO_TICKS(500));

		printf("EventBitSetting task about to set the Bit for Bit 0 in EventGroup and the current Tick is: %ld \r\n",xTaskGetTickCount());

		xEventGroupSetBits(xEventGroupTest_handle,TASK_0_BIT);

		vTaskDelay(pdMS_TO_TICKS(500));

		printf("EventBitSetting task about to set the Bit for Bit 1 in EventGroup and the current Tick is: %ld \r\n",xTaskGetTickCount());

		xEventGroupSetBits(xEventGroupTest_handle,TASK_1_BIT);

		vTaskDelay(pdMS_TO_TICKS(5000));

	}
}

EventBits_t xEventGroupStatus;
EventBits_t xEventoWait = (TASK_0_BIT | TASK_1_BIT);

static void xEventBitReading_task(void* pvParameters)
{

	while(1)
	{

		xEventGroupStatus = xEventGroupWaitBits(xEventGroupTest_handle,xEventoWait,pdTRUE,pdTRUE,portMAX_DELAY);

		if((xEventGroupStatus & TASK_0_BIT) != 0)
		{
			printf("The EventBit_0 was set and successfully read! and the current Tick is: %ld \r\n",xTaskGetTickCount());
		}

		if((xEventGroupStatus & TASK_1_BIT) != 0)
		{
			printf("The EventBit_1 was set and successfully read! and the current Tick is: %ld \r\n",xTaskGetTickCount());
		}

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


