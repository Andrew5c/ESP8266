
#include "main.h"


void Beep_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	//1��ʹ��GPIO���ⲿ�жϱ�������APIOʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	//2��GPIO��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOC, GPIO_Pin_13);
}

/*
** ���ܣ����������ٵ��� nbeep ����Ϊ����
** ˵����ʹ�õ�systic�еľ�ȷ��ʱ��
	���Է��ֵ���ʱ150ms��ʱ�������Ч���ȽϺ�
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
