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

osEventFlagsId_t EventFlag_Id;

osThreadId_t EveReadId,EveSetId1,EveSetId2;

static void xEvenFlagReading_Thread(void* arguments);
static void xEvenFlagSetting_Thread1(void* arguments);
static void xEvenFlagSetting_Thread2(void* arguments);




int main(void)
{

  HAL_Init();


  SystemClock_Config();


  MX_GPIO_Init();

  USART2_UART_TX_Init();

  /*Lets initialize userLED for indication */
  RCC->AHB1ENR |= (1U<<0);

  GPIOA->MODER &= ~(1U<<11);
  GPIOA->MODER |=  (1U<<10);

  osKernelInitialize();


//  printf("System Initializing... \r\n");

  EventFlag_Id = osEventFlagsNew(NULL);


  osThreadAttr_t EventFlgReaderAttr = {
		  .name = "Event Flag Reader Thread",
		  .stack_size = 1024,
		  .priority = osPriorityNormal
  };

  EveReadId = osThreadNew(xEvenFlagReading_Thread,NULL,&EventFlgReaderAttr);

  osThreadAttr_t EventFlgSettingAttr1 = {
  		  .name = "Event Flag Setter 1 Thread",
  		  .stack_size = 1024,
  		  .priority = osPriorityNormal
    };

  EveSetId1 = osThreadNew(xEvenFlagSetting_Thread1,NULL,&EventFlgSettingAttr1);

   osThreadAttr_t EventFlgSettingAttr2 = {
   		  .name = "Event Flag Setter 2 Thread",
   		  .stack_size = 1024,
   		  .priority = osPriorityNormal
     };

   EveSetId2 = osThreadNew(xEvenFlagSetting_Thread2,NULL,&EventFlgSettingAttr2);

  osKernelStart();

  while (1)
  {

  }

}





uint32_t xEventGroupStatus;


static void xEvenFlagReading_Thread(void* arguments)
{

	const uint32_t uxBitsToWaitFor = (TASK_0_BIT | TASK_1_BIT);


	while(1)
	{

		xEventGroupStatus = osEventFlagsWait(EventFlag_Id,uxBitsToWaitFor,0,portMAX_DELAY);

		GPIOA->ODR ^= (1U<<5);

		osDelay(pdMS_TO_TICKS(200));


	}
}


static void xEvenFlagSetting_Thread1(void* arguments)
{


	while(1)
	{

		osDelay(8000);
		printf("Bit0 is Successfully Set! current tick=%ld \r\n",osKernelGetTickCount());
		osEventFlagsSet(EventFlag_Id,TASK_0_BIT);


		osDelay(pdMS_TO_TICKS(200));


	}
}


static void xEvenFlagSetting_Thread2(void* arguments)
{



	while(1)
	{

		osDelay(12000);
		printf("Bit1 is Successfully Set! current tick=%ld \r\n",osKernelGetTickCount());
		osEventFlagsSet(EventFlag_Id,TASK_1_BIT);

		osDelay(pdMS_TO_TICKS(200));


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
