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

TaskHandle_t btn_task,analog_task,gatekeep_task;

QueueHandle_t xPrintQueue;

typedef enum
{
	digital_reading,
	analog_reading
}Data_source;

typedef struct
{
	Data_source source_id;
	uint32_t value;
}Data_t;




void btn_read_task(void *pdParameters);
void analog_read_task(void *pdParameters);
void gatekeeper_task(void *pvParameters);



int main(void)
{

  HAL_Init();


  SystemClock_Config();


  MX_GPIO_Init();
  USART2_UART_TX_Init();


  RCC->AHB1ENR |= (1U<<0);
  GPIOA->MODER |= (1U<<10);
  GPIOA->MODER &= ~(1U<<11);

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
  			  1,
  			  &analog_task
  			  );
  xTaskCreate(gatekeeper_task,
		  	  "Gatekeeper Task",
			  256,
			  NULL,
			  0,
			  &gatekeep_task
			  );

  xPrintQueue = xQueueCreate(10,sizeof( Data_t ));


  printf("System Initializing... \r\n");
  vTaskStartScheduler();





  while (1)
  {



  }

}

void btn_read_task(void *pdParameters)
{
	  gpio_init();

	  Data_t data1;
	  data1.source_id = digital_reading;

	  while(1)
	  {
		 data1.value = digital_read();
		 xQueueSend(xPrintQueue,&data1,0);

		 vTaskDelay(pdMS_TO_TICKS(50));

	  }
}

void analog_read_task(void *pdParameters)
{
	  adc1_init();

	  Data_t data2;
	  data2.source_id = analog_reading;

	  while(1)
	  {
		  data2.value = adc1_read_value();
		  xQueueSend(xPrintQueue,&data2,0);

		  vTaskDelay(pdMS_TO_TICKS(50));



	  }
}

void gatekeeper_task(void *pvParameters)
{
	Data_t print_value;
	while(1)
	{
		xQueueReceive(xPrintQueue,&print_value,portMAX_DELAY);

		if(print_value.source_id == digital_reading)
		{
			printf("The Data From digital reading : %ld \r\n",print_value.value);
		}
		if(print_value.source_id == analog_reading)
		{
			printf("The Data From analog reading : %ld \r\n",print_value.value);
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


