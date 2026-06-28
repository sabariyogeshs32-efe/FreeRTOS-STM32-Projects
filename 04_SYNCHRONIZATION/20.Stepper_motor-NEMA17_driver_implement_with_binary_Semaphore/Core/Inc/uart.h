#ifndef INC_UART_H_
#define INC_UART_H_

int __io_putchar(int ch);
void uart_write(int ch);
void PrintF(const char *x);

//void USART2_UART_RX_Init(void);
void USART2_UART_TX_Init(void);


#endif /* INC_UART_H_ */
