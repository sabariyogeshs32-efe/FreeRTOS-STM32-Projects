#include "main.h"
#include "cmsis_os.h"
#include <stdio.h>


UART_HandleTypeDef huart2;

#define GREEN 	 GPIO_PIN_5
#define RED		 GPIO_PIN_12
#define BLUE	 GPIO_PIN_10

/*Pointer for passing through the parameters for the task functions... */
//const uint16_t *blue_led = (uint16_t *)BLUE;
//const uint16_t *green_led = (uint16_t *)GREEN;
//const uint16_t *red_led = (uint16_t *)RED;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
//void xTaskCreate()

int __io_putchar(int ch);
void vRedLedControllerTask(void* pvParameters);
void vGreenLedControllerTask(void* pvParameters);
void vBlueLedControllerTask(void* pvParameters);

int msg[1] = {'H'};

typedef uint16_t t;
t	GreenTaskProfiler=0, BlueTaskProfiler=0, RedTaskProfiler=0;

/*Task_Handler...*/
TaskHandle_t red_handle,green_handle,blue_handle;

uint32_t green_priority;

/*What we have left is simply hardware initialization required for our FreeRTOS... JUST THE PERIPHERALS...  Nothing RTOS here...  */

int main(void)
{


   HAL_Init();


  SystemClock_Config();


  MX_GPIO_Init();
  MX_USART2_UART_Init();

  xTaskCreate(vRedLedControllerTask,
		  	  "Red Led Controller",		//Name string for the task function as our preference
			   100,							//The stack for the temporary storage that's own current execution state in register bank
			   NULL,						//the parameters to be passed(arguments) to the TaskFunction here NUll nothing passes
			   7,							//the priority of the TaskFunction let set it as 1
			   &red_handle					//TaskHandler for that task...
			   );

  xTaskCreate(vBlueLedControllerTask,
  	  		  	  "Blue Led Controller",		//Name string for the task function as our preference
  	  			   100,							//The stack for the temporary storage that's own current execution state in register bank
				   NULL,							//the parameters to be passed(arguments) to the TaskFunction here NUll nothing passes
  	  			   7,							//the priority of the TaskFunction let set it as 1
				   &blue_handle					//TaskHandler for that task...
  	  			   );

  xTaskCreate(vGreenLedControllerTask,
    	  		  	  "Green Led Controller",		//Name string for the task function as our preference
    	  			   100,							//The stack for the temporary storage that's own current execution state in register bank
					   NULL,							//the parameters to be passed(arguments) to the TaskFunction here NUll nothing passes
    	  			   7,							//the priority of the TaskFunction let set it as 1
					   &green_handle						//TaskHandler for that task...
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

uint32_t Suspend_monitor;

/*The Task Function for BlueLedController Task*/

void vRedLedControllerTask(void* pvParameters)
{



	while(1)
	{
		HAL_GPIO_TogglePin(GPIOC,RED);

		RedTaskProfiler++;
		for(int i=0;i<100000;i++){}

		Suspend_monitor++;

		if(Suspend_monitor >= 35)
		{
			vTaskSuspend(NULL);
		}


		/*Changing its own Priority... */
//		vTaskPrioritySet(NULL,1);
			//printf("vBlueLedControllerTask running \n\r");
	}
}

void vGreenLedControllerTask(void* pvParameters)
{



	while(1)
	{

		HAL_GPIO_TogglePin(GPIOA,GREEN);

		GreenTaskProfiler++;
		for(int i=0;i<100000;i++){}

		//vTaskPrioritySet(red_handle,8);
			//printf("vBlueLedControllerTask running \n\r");
	}
}

void vBlueLedControllerTask(void* pvParameters)
{



	while(1)
	{
		HAL_GPIO_TogglePin(GPIOC,BLUE);
		BlueTaskProfiler++;
		for(int i=0;i<100000;i++){}

		/*Resume the redTask */
		if(GreenTaskProfiler >= 70)
				{
					vTaskResume(red_handle);
				}

//		Suspend_monitor++;
//
//		if(Suspend_monitor >= 50)
//		{
//			vTaskSuspend(green_handle);
//		}

//		green_priority = uxTaskPriorityGet(green_handle);

//		for(int i=0;i<700000;i++){}
//
//		/*Changing the priority of the GreenLedControllerTask in runtime...*/
//		vTaskPrioritySet(green_handle,5);


			//printf("vBlueLedControllerTask running \n\r");
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
  GPIO_InitTypeDef	GPIO_InitStruct;

  /*GPIOA Enable clk access*/
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Enable Clk ti the port*/
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Reset the pins*/
  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_12 | GPIO_PIN_10,GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_RESET);

  /*Configure the port as like port_no,mode,speed,otype*/
  GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

  /*Initialize the pins*/
  HAL_GPIO_Init(GPIOC,&GPIO_InitStruct);

  /*FOR GPIOA PA5*/
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

  HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);


}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	/*This is our Time Base remember we have selected the Timer1 for the FreeRTOS TimeBase... */

  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }

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
