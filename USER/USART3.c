
//����1��������������һ����Ƭ�����͹����źš�

#include "main.h"


//unsigned char Tx_Buffer[50] = {0};
//unsigned char Tx_Counter = 0;
//unsigned char Tx_Counter_Temp = 0;


unsigned char USART3_Rx_Buffer[50] = {0};		//���ܻ���
unsigned char USART3_Rx_Counter = 0;          //���ܼ�������
unsigned char USART3_CMD_Rx_Buffer[20] = {0};


void Bluetooth_USART3_Init(uint32_t baud)
{
	Usart3_GPIO_Init();
	Usart3_Configuration(baud);
	
}
/*----------------------------------
**�������ƣ�Usart_GPIO_Init
**��������������ʹ�����ų�ʼ��
**����˵������
**���ߣ�Andrew
**���ڣ�2018.1.24
-----------------------------------*/
void Usart3_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 				//���ô��ڽ��ն˿ڹҽӵ�9�˿�
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	   		//�����������
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   	//���ö˿��ٶ�Ϊ50M
  	GPIO_Init(GPIOB, &GPIO_InitStructure);				   	//���ݲ�����ʼ��GPIOA�Ĵ���	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; 
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//��������(��λ״̬);
  	GPIO_Init(GPIOB, &GPIO_InitStructure);				   	//���ݲ�����ʼ��GPIOA�Ĵ���	
}


/*----------------------------------
**�������ƣ�Usart_Configuration
**�������������ڲ������ã������ж�����������
**����˵����BaudRate����Ҫ���õĲ�����
**���ߣ�Andrew
**���ڣ�2018.1.24
-----------------------------------*/
void Usart3_Configuration(uint32_t BaudRate)
{
	USART_InitTypeDef USART_InitStructure;							    	//����һ�����ڽṹ��
	NVIC_InitTypeDef  NVIC_InitStructure;									//�ж������ṹ��
	
	USART_DeInit(USART3); 														//��λ����
	USART_InitStructure.USART_BaudRate            =BaudRate ;	  			//�������Լ����ã�����������9600
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b; 	//���������ʹ��8λ����
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;	 	//��֡��β����1λֹͣλ
	USART_InitStructure.USART_Parity              = USART_Parity_No ;	 	//��żʧ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//Ӳ����ʧ��
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx; //���պͷ���ģʽ
	USART_Init(USART3, &USART_InitStructure);								//���ݲ�����ʼ�����ڼĴ���
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);							//ʹ�ܴ����жϽ���
	USART_Cmd(USART3, ENABLE);     											//ʹ�ܴ�������
	
	//�����������1���ֽ��޷���ȷ���ͳ�ȥ������ 
	USART_ClearFlag(USART3, USART_FLAG_TC);     /* �巢����Ǳ�־��Transmission Complete flag */
	
	
	//���ô����ж�����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //�жϷ���1
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;			//�趨�ж�ԴΪ
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//�ж�ռ���ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			//�����ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ʹ���ж�
	NVIC_Init(&NVIC_InitStructure);							   	//���ݲ�����ʼ���жϼĴ���
	
}


/*----------------------------------
**�������ƣ�USART1_IRQHandler
**���������������жϷ�����
**����˵������
**���ߣ�Andrew
**���ڣ�2018.1.24
-----------------------------------*/
void USART3_IRQHandler()
{
	unsigned int i = 0;
	
	if (USART_GetFlagStatus(USART3, USART_FLAG_ORE) != RESET)	//�쳣�жϴ���
    {
       USART_ReceiveData(USART3);										//��������
		 USART_ClearFlag(USART3, USART_FLAG_ORE);
    }
	 
	 /*����*/
//	if(USART_GetITStatus(USART3,USART_IT_TXE) != RESET)
//	{
//		USART1->DR = Tx_Buffer[Tx_Counter_Temp++];    	//��DR�Ĵ���д���ݵ�ʱ�򣬴��ھͻ��Զ�����
//			
//		if(Tx_Counter == Tx_Counter_Temp)					//���ݰ��������
//		{
//		   USART_ITConfig(USART1,USART_IT_TXE,DISABLE);	//�ر�TXE�ж�
//			Tx_Counter = 0;		//������ɼ�������
//			Tx_Counter_Temp = 0;
//		}
//	}
	 
	 /*����AAFB00036X\n*/
	if(USART_GetITStatus(USART3,USART_IT_RXNE) != RESET)	//��ȡ�����жϱ�־λUSART_IT_RXNE 
	{
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);	//����жϱ�־λ
				
		USART3_Rx_Buffer[USART3_Rx_Counter++] = USART_ReceiveData(USART3);//�������ݵ�������������������Զ�����жϱ�־λ
		
		if(USART3_Rx_Buffer[USART3_Rx_Counter-1] == 00)//������Ľ�����־
		{
			for(i = 0;i<USART3_Rx_Counter;i++)
			{
				USART3_CMD_Rx_Buffer[i] = USART3_Rx_Buffer[i];  //�������������
			}
			USART3_CMD_Rx_Buffer[USART3_Rx_Counter] = 0;		//'\0'
			USART3_Rx_Counter = 0;
		}
	}

}


