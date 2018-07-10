#ifndef __USART1_H
#define __USART1_H

#include <stm32f10x.h>

//º¯ÊýÉùÃ÷
void Usart1_GPIO_Init(void);
void Usart1_Configuration(uint32_t BaudRate); 

void bluetooth_init(uint32_t baud);
void USART1_Put_Char(unsigned char DataToSend);
void Send_Bluetooth(const char send[]);
	
#endif


