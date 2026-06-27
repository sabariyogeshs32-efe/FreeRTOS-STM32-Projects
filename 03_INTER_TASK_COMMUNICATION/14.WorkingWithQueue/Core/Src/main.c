#include "main.h"
#include "cmsis_os.h"
#include <stdio.h>
#include <string.h>


UART_HandleTypeDef huart2;


void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);

int __io_putchar(int ch);
void vBlueLedControllerTask(void* pvParameters);
void vRedLedControllerTask(void* pvParameters);
void vGreenLedControllerTask(void* pvParameters);

/*Queue Handler Creation: */
QueueHandle_t uC_Name_queue;

/*Task Handler Creation: */
TaskHandle_t sender_handle,receiver_handle;

/*Prototype for Tasks to be created */
void SenderTask(void *pvParameters);
void ReceiverTask(void *pvParameters);

/*TimeTicks*/
TickType_t WaitDelay = pdMS_TO_TICKS(100);


int main(void)
{


  HAL_Init();


  SystemClock_Config();


  MX_GPIO_Init();
  MX_USART2_UART_Init();


  /*TaskCreation: */
   xTaskCreate(SenderTask,
		       "Sender Task",
			   100,
			   NULL,
			   1,
			   &sender_handle
			   );
   xTaskCreate(ReceiverTask,
   		       "Receiver Task",
   			   100,
   			   NULL,
   			   1,
   			   &receiver_handle
   			   );

   /*Queue Creation: */
   uC_Name_queue = xQueueCreate(8,sizeof(int32_t));

   /*Start the Task Scheduler... */
   vTaskStartScheduler();

  while (1)
  {

	  //HAL_UART_Transmit(&huart2,(uint8_t *)msg,10,0xFFFF);
	  printf("Hello_Sabari\n\r");

  }

}

/*Task Functions Definitions: */
void SenderTask(void *pvParameters)
{
	const char *Item2Send = "STM32F446RE";
	BaseType_t queue_status;

	while(1)
	{
		queue_status = xQueueSend(uC_Name_queue, &Item2Send, WaitDelay);

		/*Check whether the Transfer is successful! */
		if(queue_status == pdPASS)
		{
			//for(int i=0;i<100000;i++){}

			printf("The Data is Successfully sent to the Queue!!! \r\n");
			//for(int i=0;i<100000;i++){}

		}
		else
		{
		//	for(int i=0;i<100000;i++){}

			printf("Error occured while Sending!!! \r\n");
			//for(int i=0;i<100000;i++){}

		}
		for(int i=0;i<100000;i++){}


	}
}

void ReceiverTask(void *pvParameters)
{
	char *ReceiveBuffer;
	BaseType_t queue_status;

	while(1)
	{
		queue_status = xQueueReceive(uC_Name_queue, &ReceiveBuffer, WaitDelay);

		/*Check whether the Reception is successful!!! */
		if(queue_status == pdPASS)
		{
			printf("The Data is Successfully received from the Queue!!! \r\n");
			//for(int i=0;i<100000;i++){}

			printf("The Received Data is : %s \r\n",ReceiveBuffer);
			//for(int i=0;i<100000;i++){}

		}
		else
		{
			//for(int i=0;i<100000;i++){}

			printf("Error occured while Receiving!!! \r\n");
			//for(int i=0;i<100000;i++){}

		}
	}
}

int uart2write(int ch)
{
	while(!(USART2->SR & 0x80)){}

		USART2->DR = (ch & 0xFF);
		return ch;

}


int __io_putchar(int ch)
{
	//HAL_UART_Transmit(&huart2,(uint8_t *)&ch,1,0xFFFF);
	uart2write(ch);
	return ch;
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


static void MX_USART2_UART_Init(void)
{


  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
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

//void vApplicationIdleHook(void)
//{
//	//Do Something
//	//IdleTaskProfiler++;
//
//}


void Error_Handler(void)
{

  __disable_irq();
  while (1)
  {
  }

}

#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{

}
#endif
