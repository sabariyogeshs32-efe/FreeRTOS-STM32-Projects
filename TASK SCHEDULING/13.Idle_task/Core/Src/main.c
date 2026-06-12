#include "main.h"
#include "cmsis_os.h"
#include <stdio.h>


UART_HandleTypeDef huart2;


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
//void xTaskCreate()

int __io_putchar(int ch);
void vBlueLedControllerTask(void* pvParameters);
void vRedLedControllerTask(void* pvParameters);
void vGreenLedControllerTask(void* pvParameters);

/*macro for the milliSecond to ticks for Gelay*/
//const TickType_t _50ms = pdMS_TO_TICKS(50);

int msg[1] = {'H'};

const TickType_t _250ms = pdMS_TO_TICKS(250);
TickType_t xLastWakeTime;

typedef uint32_t t;
t	GreenTaskProfiler, BlueTaskProfiler, RedTaskProfiler, IdleTaskProfiler;
//int ch[1] = {'H'};

/*What we have left is simply hardware initialization required for our FreeRTOS... JUST THE PERIPHERALS...  Nothing RTOS here...  */

int main(void)
{


  HAL_Init();


  SystemClock_Config();


  MX_GPIO_Init();
  MX_USART2_UART_Init();

  xTaskCreate(vBlueLedControllerTask,
		  	  "Blue Led Controller",		//Name string for the task function as our preference
			   100,							//The stack for the temporary storage that's own current execution state in register bank
			   NULL,						//the parameters to be passed(arguments) to the TaskFunction here NUll nothing passes
			   4,							//the priority of the TaskFunction let set it as 1
			   NULL							//TaskHandler for that task...
			   );

  xTaskCreate(vRedLedControllerTask,
  		  	  "Red Led Controller",		//Name string for the task function as our preference
  			   100,							//The stack for the temporary storage that's own current execution state in register bank
  			   NULL,						//the parameters to be passed(arguments) to the TaskFunction here NUll nothing passes
  			   4,							//the priority of the TaskFunction let set it as 1
  			   NULL							//TaskHandler for that task...
  			   );

  xTaskCreate(vGreenLedControllerTask,
  		  	  "Green Led Controller",		//Name string for the task function as our preference
  			   100,							//The stack for the temporary storage that's own current execution state in register bank
  			   NULL,						//the parameters to be passed(arguments) to the TaskFunction here NUll nothing passes
  			   4,							//the priority of the TaskFunction let set it as 1
  			   NULL							//TaskHandler for that task...
  			   );

  /*Then we have to start the taskscheduler by using the taskstartscheduler*/

  vTaskStartScheduler();

 /*Lets delete all the stuffs related to the CMSIS RTOS and we will create the functions of the FreeRTOS on by own self...*/



  while (1)
  {

	  HAL_UART_Transmit(&huart2,(uint8_t *)msg,10,0xFFFF);
	  printf("Hello_Sabari\n\r");

  }

}

/*The Task Function for BlueLedController Task*/

void vBlueLedControllerTask(void* pvParameters)
{


	while(1)
	{
		BlueTaskProfiler++;
		xLastWakeTime = xTaskGetTickCount();

		for(int i=0;i<100000;i++){}

		vTaskDelayUntil(& xLastWakeTime,_250ms);
		//vTaskDelay(_250ms);

	}
}

void vRedLedControllerTask(void* pvParameters)
{
	while(1)
	{
		RedTaskProfiler++;
		xLastWakeTime = xTaskGetTickCount();

		for(int i=0;i<100000;i++){}


//		vTaskDelay(_250ms);
		vTaskDelayUntil(& xLastWakeTime,_250ms);

	}
}

void vGreenLedControllerTask(void* pvParameters)
{
	while(1)
	{
		GreenTaskProfiler++;
		xLastWakeTime = xTaskGetTickCount();

		for(int i=0;i<100000;i++){}

		//vTaskDelay(_250ms);
		vTaskDelayUntil(& xLastWakeTime,_250ms);

	}
}

int __io_putchar(int ch)
{
	HAL_UART_Transmit(&huart2,(uint8_t *)&ch,1,0xFFFF);
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

void vApplicationIdleHook(void)
{
	//Do Something
	IdleTaskProfiler++;

}


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
