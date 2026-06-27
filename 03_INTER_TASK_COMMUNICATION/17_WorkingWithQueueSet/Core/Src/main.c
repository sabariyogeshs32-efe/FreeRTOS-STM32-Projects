#include "main.h"
#include "cmsis_os.h"
#include <stdio.h>
#include <string.h>


UART_HandleTypeDef huart2;


void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);

int __io_putchar(int ch);

/*Creating Enum for DataSourceId... */
typedef enum
{
	temp_sensor,
	humidity_sensor,
	pressure_sensor

}DataSource_t;

/*Creating a struct for value and source buffer... */
typedef struct
{
	uint8_t ucvalue;
	DataSource_t DataSourceId;
}Data_t;

/*Creating a array of structure elements to send those all data... */
static const Data_t xStructToSend[3] = {

		{39,temp_sensor},
		{67,humidity_sensor},
		{87,pressure_sensor}

};

/*Totally 4Tasks needed 3 for sending unique data and 1 for receiving all those 3... */
void vTempSensorSenderTask(void* pvParameters);
void vHumiditySensorSenderTask(void* pvParameters);
void vPressureSensorSenderTask(void* pvParameters);

void vAllDataReceiverTask(void* pvParameters);

/*Task Handlers */
TaskHandle_t temp_sensor_handle,humidity_sensor_handle,pressure_sensor_handle,all_receiver_handle;

/*A Queue needed for data Buffer... */
/*Queue Handler */
QueueHandle_t queue1;
QueueHandle_t queue2;
QueueHandle_t queue3;

/*QueueSet Handler */
QueueSetHandle_t queueset1;


int main(void)
{


  HAL_Init();


  SystemClock_Config();


  MX_GPIO_Init();

  MX_USART2_UART_Init();

  /*Create Tasks and Queues... */
  xTaskCreate(vTempSensorSenderTask,
		  	  "Temperature Sensor",
			  100,
			  NULL,
			  1,
			  &temp_sensor_handle
			  );
  xTaskCreate(vHumiditySensorSenderTask,
		  	  "Humidity Sensor",
			  100,
			  NULL,
			  1,
			  &humidity_sensor_handle
			  );
  xTaskCreate(vPressureSensorSenderTask,
  		  	  "Pressure Sensor",
  			  100,
  			  NULL,
  			  1,
			  &pressure_sensor_handle
  			  );

    xTaskCreate(vAllDataReceiverTask,
  		  	  "Humidity Sensor",
  			  100,
  			  NULL,
  			  3,
			  &all_receiver_handle
			  );

    /*Create Queue... */
    queue1 = xQueueCreate(10,sizeof(Data_t));
    queue2 = xQueueCreate(10,sizeof(Data_t));
    queue3 = xQueueCreate(10,sizeof(Data_t));

    /*Creating the queueset... */
    queueset1 = xQueueCreateSet(30);

    /*Adding the queues to the created queueset... */
    xQueueAddToSet(queue1,queueset1);
    xQueueAddToSet(queue2,queueset1);
    xQueueAddToSet(queue3,queueset1);


    vTaskStartScheduler();

  while(1)
  {
	  //HAL_UART_Transmit(&huart2,(uint8_t *)msg,10,0xFFFF);
	 // printf("Hello_Sabari\n\r");

	  /*Sample printing... */
	  for(int i=0; i<3; i++)
	  {
	  printf("%d SensorId: %d\n",xStructToSend[i].ucvalue,xStructToSend[i].DataSourceId);

	  }
  }

}

/*Tasks Definition... */

void vTempSensorSenderTask(void* pvParameters)
{
	BaseType_t qstatus;
	while(1)
	{
		qstatus = xQueueSend(queue1, &xStructToSend[0],portMAX_DELAY);

		if(qstatus == pdPASS)
		{
			//printf("The Data Has been Successfully sent... ");
		}
		else
		{
			printf("Error! occured While Sending... ");
		}
		vTaskDelay(pdMS_TO_TICKS(5000));
	}
}
void vHumiditySensorSenderTask(void* pvParameters)
{
	BaseType_t qstatus;
	while(1)
	{

		qstatus = xQueueSend(queue2, &xStructToSend[1],portMAX_DELAY);

		if(qstatus == pdPASS)
		{
			//printf("The Data Has been Successfully sent... ");
		}
		else
		{
			printf("Error! occured While Sending... ");
		}
		vTaskDelay(pdMS_TO_TICKS(8000));
	}
}
void vPressureSensorSenderTask(void* pvParameters)
{
	BaseType_t qstatus;
	while(1)
	{


		qstatus = xQueueSend(queue3, &xStructToSend[2],portMAX_DELAY);

		if(qstatus == pdPASS)
		{
			//printf("The Data Has been Successfully sent... ");
		}
		else
		{
			printf("Error! occured While Sending... ");
		}
		vTaskDelay(pdMS_TO_TICKS(10000));
	}
}


void vAllDataReceiverTask(void* pvParameters)
{
	Data_t received_data;
	BaseType_t qstatus;
	while(1)
	{



//			printf("The Sensor Id: %d, Sensor Value: %d \r\n",received_data.DataSourceId,received_data.ucvalue);

			QueueSetMemberHandle_t active_queue;

			active_queue = xQueueSelectFromSet(queueset1,portMAX_DELAY);

			qstatus = xQueueReceive(active_queue, &received_data,portMAX_DELAY);
							if(qstatus == pdPASS)
									{
								if(received_data.DataSourceId == temp_sensor)
											{
												printf("Tick=%lu Temp=%d\r\n",(unsigned long)xTaskGetTickCount(),received_data.ucvalue);
												//printf("The Temperature Sensor Value: %d \r\n",received_data.ucvalue);
											}

											else if(received_data.DataSourceId == humidity_sensor)
											{
												printf("Tick=%lu Humi=%d\r\n",(unsigned long)xTaskGetTickCount(),received_data.ucvalue);
											}
											else if(received_data.DataSourceId == pressure_sensor)
											{
												printf("Tick=%lu press=%d\r\n",(unsigned long)xTaskGetTickCount(),received_data.ucvalue);
											}
											else
											{
												printf("Nothing has been Received!!! ");
											}
									}
							else
									{
										printf("Error! occured While Sending... ");
									}

//			if(active_queue == queue1)
//			{
//				qstatus = xQueueReceive(queue1, &received_data,portMAX_DELAY);
//				if(qstatus == pdPASS)
//						{
//							printf("The Data Has been Successfully Received From the Queue!!! \r\n");
//							printf("The Temperature Sensor Value: %d \r\n",received_data.ucvalue);
//						}
//				else
//						{
//							printf("Error! occured While Sending... ");
//						}
//			}
//
//			if(active_queue == queue2)
//			{
//				qstatus = xQueueReceive(queue2, &received_data,portMAX_DELAY);
//				if(qstatus == pdPASS)
//						{
//							printf("The Data Has been Successfully Received From the Queue!!! \r\n");
//							printf("The Humidity Sensor Value: %d \r\n",received_data.ucvalue);
//						}
//				else
//						{
//							printf("Error! occured While Sending... ");
//						}
//
//			}
//			if(active_queue == queue3)
//			{
//				qstatus = xQueueReceive(queue3, &received_data,portMAX_DELAY);
//				if(qstatus == pdPASS)
//						{
//							printf("The Data Has been Successfully Received From the Queue!!! \r\n");
//							printf("The Pressure Sensor Value: %d \r\n",received_data.ucvalue);
//						}
//				else
//						{
//							printf("Error! occured While Sending... ");
//						}
//
//			}
//			else
//			{
//				printf("Nothing has been Received!!! ");
//			}


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
