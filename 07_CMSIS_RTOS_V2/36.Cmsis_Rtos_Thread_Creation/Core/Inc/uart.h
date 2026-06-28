#ifndef INC_UART_H_
#define INC_UART_H_

int __io_putchar(int ch);
void uart_write(int ch);
//void USART2_UART_RX_Init(void);
void USART2_UART_TX_Init(void);
void USART2_UART_RX_Init(void);
char uart_read(void);


#endif /* INC_UART_H_ */
