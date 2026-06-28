#include "stm32f446xx.h"




void exti_pc13_init(void)
{
	/*disable the global interrupts */
	__disable_irq();

	/*Enable clk for the GPIOC for PC13 to use for interrupt */
	RCC->AHB1ENR |= (1U<<2);

	/*Set the PC13 as input for button press input */
	GPIOC->MODER &= ~(1U<<26);
	GPIOC->MODER &= ~(1U<<27);

	/*Enable clk for SYSCONFIG since it act as select line for MUX of EXTI */
	RCC->APB2ENR |= (1U<<14);

	/*Clear Port Selection for EXTI13 */
	SYSCFG->EXTICR[3] &= ~(0xF0);


	/*Select PC in EXTI13 using the select line */
	SYSCFG->EXTICR[3] |= 0x20;

	/*Unmask the EXTI13 */
	EXTI->IMR |= 0x2000;

	/*Set the Trigger Point Rising or Falling Edge */
	EXTI->RTSR |= (1U<<13);

	/*Set priority for EXTI13 */
	NVIC_SetPriority(EXTI15_10_IRQn,6);

	/*Enable the EXTI in NVIC */
	NVIC_EnableIRQ(EXTI15_10_IRQn);

	/*Enable the global interrupts */
	__enable_irq();

}

void gpio_init(void)
{
	/*Enable clk for the GPIOC for PC13 to use for interrupt */
		RCC->AHB1ENR |= (1U<<2);

		/*Set the PC13 as input for button press input */
		GPIOC->MODER &= ~(1U<<26);
		GPIOC->MODER &= ~(1U<<27);
}

uint32_t digital_read(void)
{
	if(GPIOC->IDR & (1U<<13))
	{
		return 1;
	}

	else
		{
		return 0;

		}

}
