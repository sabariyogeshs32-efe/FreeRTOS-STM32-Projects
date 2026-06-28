#include "stm32f446xx.h"
#include "stdint.h"
#include "cmsis_os.h"

/*
 * STEP ->PC6
 * DIR ->PC8
 * */


uint32_t current_ARR;
uint32_t event_occured;
uint32_t target_ARR;
uint32_t acceleration_step;
uint32_t event_max_value;


TaskHandle_t motor_task_handler;

SemaphoreHandle_t xBinSemaphore;

void motor_control_task(void* pvParameters);


int main(void)
{

	/*Clk access to PC */
	RCC->AHB1ENR |= (1U<<2);

	/*Set PC6 pin in Alternate Function mode */
	GPIOC->MODER |= (1U<<13);
	GPIOC->MODER &= ~(1U<<12);

	/*Set PC8 pin in General purpose output mode */
	GPIOC->MODER &= ~(1U<<17);
	GPIOC->MODER |= (1U<<16);

	/*Set the PC6 as TIM3_CH1 in AFR */
	GPIOC->AFR[0] &= ~(1U<<27);
	GPIOC->AFR[0] &= ~(1U<<26);
	GPIOC->AFR[0] |=  (1U<<25);
	GPIOC->AFR[0] &= ~(1U<<24);


	/*Set the OTYPE to output push-pull mode */
	GPIOC->OTYPER &= ~(1U<<6);
	GPIOC->OTYPER &= ~(1U<<8);

	/*Set the PUPDR to no PULL UP/PULL DOWN */
	GPIOC->PUPDR &= ~(1U<<13);
	GPIOC->PUPDR &= ~(1U<<12);

	GPIOC->PUPDR &= ~(1U<<17);
	GPIOC->PUPDR &= ~(1U<<16);

	/*Set the SPEED to HIGH SPEED bcz the DRV8825 Requires High speed to recognize that signal!!! */
	GPIOC->OSPEEDR |= (1U<<13);
	GPIOC->OSPEEDR |= (1U<<12);

	GPIOC->OSPEEDR |= (1U<<17);
	GPIOC->OSPEEDR |= (1U<<16);

	/*DIR = 1 for Clockwise */
	GPIOC->BSRR = (1U<<8);

	/*Enable clk for the TIM3 */
	RCC->APB1ENR |= (1U<<1);

	/*To Make Sure CEN is disabled */
	TIM3->CR1 &= ~(1U<<0);

	/*Set the Prescaler value in PSC register to prescale the clock frequency */
	TIM3->PSC = (16 - 1);

	/*Enable the Auto-reload Preload buffer in ARR in CR1 */
	TIM3->CR1 |= (1U<<7);

//	/*Set the Auto Reload value in ARR */
//	TIM3->ARR = current_ARR;

	/*Enable Toggle mode in the CCMR1 register */
	TIM3->CCMR1 &= ~(1U<<6);
	TIM3->CCMR1 |=  (1U<<5);
	TIM3->CCMR1 |=  (1U<<4);

	/*To Make Sure that the CH1 is strictly configured as output */
	TIM3->CCMR1 &= ~(1U<<0);
	TIM3->CCMR1 &= ~(1U<<1);


	/*TIM cap/comp output enable */
	TIM3->CCER |= (1U<<0);

//	/*Enable the Timer */
//	TIM3->CR1 |= (1U<<0);

	/*Enable the interrupt of the timer */
	TIM3->DIER |= (1U<<0);

	/*Reduce the TIM_interrupt priority so we can use xSemaphoreGiveFromISR(); */
	NVIC_SetPriority(TIM3_IRQn,5);

	/*Enable the IRQ in NVIC */
	NVIC_EnableIRQ(TIM3_IRQn);

	xTaskCreate(
				motor_control_task,
				"Motor control task",
				256,
				NULL,
				1,
				&motor_task_handler
				);


	xBinSemaphore = xSemaphoreCreateBinary();


	vTaskStartScheduler();


	while(1){}





}

void motor_control_task(void* pvParameters)
{

	while(1)
	{

	 current_ARR = 5000;
	 event_occured = 0;
	 target_ARR = 500;
	 acceleration_step = 10;
	 event_max_value = 12000;

	 /*Set the Auto Reload value in ARR */
	 TIM3->ARR = current_ARR;

	 /*Rest COUNT reg */
	 TIM3->CNT = 0;

	 /*Enable the Timer */
	 TIM3->CR1 |= (1U<<0);

	 xSemaphoreTake(xBinSemaphore,portMAX_DELAY);

	 /*Lets Wait for 2seconds and flip direction and loop again */
	 vTaskDelay(pdMS_TO_TICKS(2000));

	 GPIOC->ODR ^= (1U<<8);

	}


}

void TIM3_IRQHandler(void)
{

	/*Disable the update interrupt flag */
	TIM3->SR &= ~(1U<<0);

	event_occured++;

	if(event_occured >= event_max_value)
	{

		TIM3->CR1 &= ~(1U<<0);

		/*Declare the FreeRTOS context switching flag */
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;

		xSemaphoreGiveFromISR(xBinSemaphore,&xHigherPriorityTaskWoken);


		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
	else
	{
		if(event_occured%2 == 0)
		{
			if(current_ARR > target_ARR)
			{

				current_ARR -= acceleration_step;

				/*Reload the current_ARR into ARR */
				TIM3->ARR = current_ARR;
			}
		}
	}

}






