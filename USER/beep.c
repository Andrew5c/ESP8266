
#include "main.h"


void Beep_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	//1、使能GPIO和外部中断必须是能APIO时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	//2、GPIO初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOC, GPIO_Pin_13);
}

/*
** 功能：蜂鸣器急促的响 nbeep 声作为警报
** 说明：使用的systic中的精确延时。
	测试发现当延时150ms的时候的声音效果比较好
*/
void Beep_Warning(unsigned char nbeep)
{
	int i;
	
	for(i=0; i < nbeep; i++)
	{
		GPIO_SetBits(GPIOC,GPIO_Pin_13);
		
		Delay_ms(150);	
		
		GPIO_ResetBits(GPIOC, GPIO_Pin_13);
		
		Delay_ms(150);	
		
	}
}

void Beep_Start(void)
{
	unsigned char i = 0;
	
	for(i = 0; i<2; i++)
	{
		GPIO_SetBits(GPIOC,GPIO_Pin_13);	
		Delay_ms(50);	
		GPIO_ResetBits(GPIOC, GPIO_Pin_13);	
		Delay_ms(100);	
		
		GPIO_SetBits(GPIOC,GPIO_Pin_13);	
		Delay_ms(100);	
		GPIO_ResetBits(GPIOC, GPIO_Pin_13);	
		Delay_ms(100);	
	}
}
