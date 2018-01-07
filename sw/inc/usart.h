#ifndef UART_H_
#define UART_H_

void USART_Init(unsigned int baudrate);
char Usart_Rx(void);
void Usart_Tx(unsigned char);
void Usart_puts(char* str);

#endif
