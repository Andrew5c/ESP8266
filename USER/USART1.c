
//串口1接蓝牙，接收另一个单片机发送过的信号。

#include "main.h"

extern volatile unsigned char receive_paper_flag ;
//串口中接收外部蓝牙发送过来的纸的剩余量保存在这个缓冲区，定时发送到网页上
extern volatile unsigned int box_1_paper_left ;


unsigned char Tx_Buffer[50] = {0};
unsigned char Tx_Counter = 0;
unsigned char Tx_Counter_Temp = 0;

unsigned char Rx_Buffer[50] = {0};		//接受缓冲
unsigned char Rx_Counter = 0;          //接受计数变量

unsigned char CmdRx_Buffer[20] = {0};

void bluetooth_init(uint32_t baud)
{
	Usart1_GPIO_Init();
	Usart1_Configuration(baud);
	
}
/*----------------------------------
**函数名称：Usart_GPIO_Init
**功能描述：串口使用引脚初始化
**参数说明：无
**作者：Andrew
**日期：2018.1.24
-----------------------------------*/
void Usart1_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 				//配置串口接收端口挂接到9端口
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	   		//复用推挽输出
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   	//配置端口速度为50M
  	GPIO_Init(GPIOA, &GPIO_InitStructure);				   	//根据参数初始化GPIOA寄存器	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//浮空输入(复位状态);
  	GPIO_Init(GPIOA, &GPIO_InitStructure);				   	//根据参数初始化GPIOA寄存器	
}


/*----------------------------------
**函数名称：Usart_Configuration
**功能描述：串口参数配置，包括中断向量的配置
**参数说明：BaudRate：需要配置的波特率
**作者：Andrew
**日期：2018.1.24
-----------------------------------*/
void Usart1_Configuration(uint32_t BaudRate)
{
	USART_InitTypeDef USART_InitStructure;							    	//定义一个串口结构体
	NVIC_InitTypeDef  NVIC_InitStructure;									//中断向量结构体
	
	USART_DeInit(USART1); 														//复位串口
	USART_InitStructure.USART_BaudRate            =BaudRate ;	  			//波特率自己设置，蓝牙设置了9600
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b; 	//传输过程中使用8位数据
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;	 	//在帧结尾传输1位停止位
	USART_InitStructure.USART_Parity              = USART_Parity_No ;	 	//奇偶失能
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//硬件流失能
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx; //接收和发送模式
	USART_Init(USART1, &USART_InitStructure);								//根据参数初始化串口寄存器
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);							//使能串口中断接收
	USART_Cmd(USART1, ENABLE);     											//使能串口外设
	
	//如下语句解决第1个字节无法正确发送出去的问题 
	USART_ClearFlag(USART1, USART_FLAG_TC);     /* 清发送外城标志，Transmission Complete flag */
	
	
	//配置串口中断向量
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //中断分组1
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;			//设定中断源为
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//中断占优先级为0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//副优先级为0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能中断
	NVIC_Init(&NVIC_InitStructure);							   	//根据参数初始化中断寄存器
	
}


/*----------------------------------
**函数名称：USART1_IRQHandler
**功能描述：串口中断服务函数
**参数说明：无
**作者：Andrew
**日期：2018.1.24
-----------------------------------*/
void USART1_IRQHandler()
{
	//unsigned int i = 0;
	
	if (USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)	//异常中断处理
    {
       USART_ReceiveData(USART1);										//丢弃数据
		 USART_ClearFlag(USART1, USART_FLAG_ORE);
    }
	 
	 /*发送*/
	if(USART_GetITStatus(USART1,USART_IT_TXE) != RESET)
	{
		USART1->DR = Tx_Buffer[Tx_Counter_Temp++];    	//向DR寄存器写数据的时候，串口就会自动发送
			
		if(Tx_Counter == Tx_Counter_Temp)					//数据包发送完成
		{
		   USART_ITConfig(USART1,USART_IT_TXE,DISABLE);	//关闭TXE中断
			Tx_Counter = 0;		//发送完成计数清零
			Tx_Counter_Temp = 0;
		}
	}
	 
	 /*接受*/
	if(USART_GetITStatus(USART1,USART_IT_RXNE) != RESET)	//读取接收中断标志位USART_IT_RXNE 
	{
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);	//清楚中断标志位
		
		Rx_Buffer[Rx_Counter++] = USART_ReceiveData(USART1);//接收数据到缓冲区，这个函数会自动清除中断标志位
		
		//检测到从机发来数据，结束标志
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
**函数名称：fputc
**功能描述：printf重定向函数
**参数说明：无
**作者：Andrew
**日期：2018.1.24
-----------------------------------*/
//重定义fputc函数，方便使用printf
int fputc(int Data, FILE *f)
{   
	while(!USART_GetFlagStatus(USART1,USART_FLAG_TXE));	  //USART_GetFlagStatus：得到发送状态位
														  //USART_FLAG_TXE:发送寄存器为空 1：为空；0：忙状态
	USART_SendData(USART1,Data);						  //发送一个字符
	
	return Data;										  //返回一个值
}


void USART1_Put_Char(unsigned char DataToSend)  
{  
  Tx_Buffer[Tx_Counter++] = DataToSend;    
  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);    
}  

//主机蓝牙发送信息给从机蓝牙，16进制发送
void Send_Bluetooth(const char send[])
{
	unsigned char i = 0;
	
	//计算数组元素数，需要加上1，
	unsigned char n = sizeof((const char *)send)/sizeof(send[0]) + 1;

	for(i = 0; i<n; i++)
	{
		USART1_Put_Char(*(send + i));
	}
		
}


