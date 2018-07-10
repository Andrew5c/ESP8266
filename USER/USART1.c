
//����1��������������һ����Ƭ�����͹����źš�

#include "main.h"

extern volatile unsigned char receive_paper_flag ;
//�����н����ⲿ�������͹�����ֽ��ʣ�����������������������ʱ���͵���ҳ��
extern volatile unsigned int box_1_paper_left ;


unsigned char Tx_Buffer[50] = {0};
unsigned char Tx_Counter = 0;
unsigned char Tx_Counter_Temp = 0;

unsigned char Rx_Buffer[50] = {0};		//���ܻ���
unsigned char Rx_Counter = 0;          //���ܼ�������

unsigned char CmdRx_Buffer[20] = {0};

void bluetooth_init(uint32_t baud)
{
	Usart1_GPIO_Init();
	Usart1_Configuration(baud);
	
}
/*----------------------------------
**�������ƣ�Usart_GPIO_Init
**��������������ʹ�����ų�ʼ��
**����˵������
**���ߣ�Andrew
**���ڣ�2018.1.24
-----------------------------------*/
void Usart1_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 				//���ô��ڽ��ն˿ڹҽӵ�9�˿�
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	   		//�����������
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   	//���ö˿��ٶ�Ϊ50M
  	GPIO_Init(GPIOA, &GPIO_InitStructure);				   	//���ݲ�����ʼ��GPIOA�Ĵ���	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//��������(��λ״̬);
  	GPIO_Init(GPIOA, &GPIO_InitStructure);				   	//���ݲ�����ʼ��GPIOA�Ĵ���	
}


/*----------------------------------
**�������ƣ�Usart_Configuration
**�������������ڲ������ã������ж�����������
**����˵����BaudRate����Ҫ���õĲ�����
**���ߣ�Andrew
**���ڣ�2018.1.24
-----------------------------------*/
void Usart1_Configuration(uint32_t BaudRate)
{
	USART_InitTypeDef USART_InitStructure;							    	//����һ�����ڽṹ��
	NVIC_InitTypeDef  NVIC_InitStructure;									//�ж������ṹ��
	
	USART_DeInit(USART1); 														//��λ����
	USART_InitStructure.USART_BaudRate            =BaudRate ;	  			//�������Լ����ã�����������9600
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b; 	//���������ʹ��8λ����
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;	 	//��֡��β����1λֹͣλ
	USART_InitStructure.USART_Parity              = USART_Parity_No ;	 	//��żʧ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//Ӳ����ʧ��
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx; //���պͷ���ģʽ
	USART_Init(USART1, &USART_InitStructure);								//���ݲ�����ʼ�����ڼĴ���
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);							//ʹ�ܴ����жϽ���
	USART_Cmd(USART1, ENABLE);     											//ʹ�ܴ�������
	
	//�����������1���ֽ��޷���ȷ���ͳ�ȥ������ 
	USART_ClearFlag(USART1, USART_FLAG_TC);     /* �巢����Ǳ�־��Transmission Complete flag */
	
	
	//���ô����ж�����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //�жϷ���1
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;			//�趨�ж�ԴΪ
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//�ж�ռ���ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//�����ȼ�Ϊ0
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
void USART1_IRQHandler()
{
	//unsigned int i = 0;
	
	if (USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)	//�쳣�жϴ���
    {
       USART_ReceiveData(USART1);										//��������
		 USART_ClearFlag(USART1, USART_FLAG_ORE);
    }
	 
	 /*����*/
	if(USART_GetITStatus(USART1,USART_IT_TXE) != RESET)
	{
		USART1->DR = Tx_Buffer[Tx_Counter_Temp++];    	//��DR�Ĵ���д���ݵ�ʱ�򣬴��ھͻ��Զ�����
			
		if(Tx_Counter == Tx_Counter_Temp)					//���ݰ��������
		{
		   USART_ITConfig(USART1,USART_IT_TXE,DISABLE);	//�ر�TXE�ж�
			Tx_Counter = 0;		//������ɼ�������
			Tx_Counter_Temp = 0;
		}
	}
	 
	 /*����*/
	if(USART_GetITStatus(USART1,USART_IT_RXNE) != RESET)	//��ȡ�����жϱ�־λUSART_IT_RXNE 
	{
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);	//����жϱ�־λ
		
		Rx_Buffer[Rx_Counter++] = USART_ReceiveData(USART1);//�������ݵ�������������������Զ�����жϱ�־λ
		
		//��⵽�ӻ��������ݣ�������־
		if(Rx_Buffer[Rx_Counter-1] == 0xEE)
		{
			receive_paper_flag = 1;
			if(Rx_Buffer[Rx_Counter-2] == 0x07)
			{
				box_1_paper_left = 75;
				Beep_Warning(1);
				Rx_Counter = 0;
				memset(Rx_Buffer, 0 ,sizeof(Rx_Buffer));
			}
			else if(Rx_Buffer[Rx_Counter-2] == 0x05)
			{
				box_1_paper_left = 50;
				Beep_Warning(2);
				Rx_Counter = 0;
				memset(Rx_Buffer, 0 ,sizeof(Rx_Buffer));
			}
			else
			{
				box_1_paper_left = 30;
				Rx_Counter = 0;
				memset(Rx_Buffer, 0 ,sizeof(Rx_Buffer));
			}
			
		}
	}

}


/*----------------------------------
**�������ƣ�fputc
**����������printf�ض�����
**����˵������
**���ߣ�Andrew
**���ڣ�2018.1.24
-----------------------------------*/
//�ض���fputc����������ʹ��printf
int fputc(int Data, FILE *f)
{   
	while(!USART_GetFlagStatus(USART1,USART_FLAG_TXE));	  //USART_GetFlagStatus���õ�����״̬λ
														  //USART_FLAG_TXE:���ͼĴ���Ϊ�� 1��Ϊ�գ�0��æ״̬
	USART_SendData(USART1,Data);						  //����һ���ַ�
	
	return Data;										  //����һ��ֵ
}


void USART1_Put_Char(unsigned char DataToSend)  
{  
  Tx_Buffer[Tx_Counter++] = DataToSend;    
  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);    
}  

//��������������Ϣ���ӻ�������16���Ʒ���
void Send_Bluetooth(const char send[])
{
	unsigned char i = 0;
	
	//��������Ԫ��������Ҫ����1��
	unsigned char n = sizeof((const char *)send)/sizeof(send[0]) + 1;

	for(i = 0; i<n; i++)
	{
		USART1_Put_Char(*(send + i));
	}
		
}


