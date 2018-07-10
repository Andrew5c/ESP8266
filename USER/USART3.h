#ifndef __USART3_H
#define __USART3_H

#include <stm32f10x.h>


extern unsigned char USART3_CMD_Rx_Buffer[20] ;

//º¯ÊýÉùÃ÷
void Usart3_GPIO_Init(void);
void Usart3_Configuration(uint32_t BaudRate); 

void Bluetooth_USART3_Init(uint32_t baud);

#endif

