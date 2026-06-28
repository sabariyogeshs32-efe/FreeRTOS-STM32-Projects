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

TimerHandle_t xOneshot_handle,xAutoreload_handle;


#define xOneshotTimer_Period	(pdMS_TO_TICKS(6000))
#define xAutoreloadTimer_Period	(pdMS_TO_TICKS(600))
void xOneShot_callback_task(TimerHandle_t xTimer);
void xAutoreload_callback_task(TimerHandle_t xTimer);

BaseType_t xTimer1Started,xTimer2Started;

int main(void)
{

  HAL_Init();


  SystemClock_Config();


  MX_GPIO_Init();
  USART2_UART_TX_Init();


  RCC->AHB1ENR |= (1U<<0);
  GPIOA->MODER |= (1U<<10);
  GPIOA->MODER &= ~(1U<<11);


  xOneshot_handle = xTimerCreate("One Shot Timer",
		  	  	  	  	  	  	 xOneshotTimer_Period,
								 pdFALSE,
								 0,
								 xOneShot_callback_task
								 );
  xAutoreload_handle = xTimerCreate("One Shot Timer",
		  	  	  	  	  	  	 xAutoreloadTimer_Period,
  								 pdTRUE,
  								 0,
								 xAutoreload_callback_task
  								 );

  xTimer1Started = xTimerStart(xOneshot_handle, 0 );
  xTimer2Started = xTimerStart(xAutoreload_handle, 0 );

  printf("System Initializing... \r\n");

  vTaskStartScheduler();




  while (1)
  {



  }

}

void xOneShot_callback_task(TimerHandle_t xTimer)
{
	TickType_t current_Tick;
	current_Tick = xTaskGetTickCount();

	printf("One Shot Callback function is called!!! Current_Tick: %ld \r\n",current_Tick);

}

void xAutoreload_callback_task(TimerHandle_t xTimer)
{
	TickType_t current_Tick;
	current_Tick = xTaskGetTickCount();

	printf("Auto reload Callback function is called!!! Current_Tick: %ld \r\n",current_Tick);

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
