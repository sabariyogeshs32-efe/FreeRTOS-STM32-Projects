#include "stm32f446xx.h"

void adc1_init(void)
{
	/*Enable clk for the GPIOA which we are going to use to take analog input for our adc */
	RCC->AHB1ENR |= (1U<<0);

	/*Set the input pin in analog input using PA */
	GPIOA->MODER |= (3U<<2);		//PA1 -> ADC_CH1

	/*Enable clk for ADC1 */
	RCC->APB2ENR |= (1U<<8);

	/*SW trigger*/
	ADC1->CR2 = 0;

	/*Set the Sequence Length and Seguence number to its channel we are going to use */
	ADC1->SQR1 = 0;
	ADC1->SQR3 = (1U<<0);


	/*Disable VBAT and Enable Internal temp sensor... */
	ADC->CCR &= ~(1U<<22);
	ADC->CCR |=  (1U<<23);

	/*Enable continous mode */
	ADC1->CR2 |= (1U<<1);

	/*Enable The ADCON */
	ADC1->CR2 |= (1U<<0);

}

//void adc1_soc(void)
//{
//	/*Give Start of Conversion */
//	ADC1->CR2 |= (1U<<30);
//
//}

uint32_t adc1_read_value(void)
{
	/*Give Start of Conversion */
	ADC1->CR2 |= (1U<<30);
	/*Wait until the conversion finish by checking the EOC bit SR */
	while(!(ADC1->SR & (1U<<1))){}

	/*Return the Read value */
	return ADC1->DR;
}
