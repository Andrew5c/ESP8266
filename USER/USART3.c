
//串口1接蓝牙，接收另一个单片机发送过的信号。

#include "main.h"


//unsigned char Tx_Buffer[50] = {0};
//unsigned char Tx_Counter = 0;
//unsigned char Tx_Counter_Temp = 0;


unsigned char USART3_Rx_Buffer[50] = {0};		//接受缓冲
unsigned char USART3_Rx_Counter = 0;          //接受计数变量
unsigned char USART3_CMD_Rx_Buffer[20] = {0};


void Bluetooth_USART3_Init(uint32_t baud)
{
	Usart3_GPIO_Init();
	Usart3_Configuration(baud);
	
}
/*----------------------------------
**函数名称：Usart_GPIO_Init
**功能描述：串口使用引脚初始化
**参数说明：无
**作者：Andrew
**日期：2018.1.24
-----------------------------------*/
void Usart3_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 				//配置串口接收端口挂接到9端口
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	   		//复用推挽输出
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   	//配置端口速度为50M
  	GPIO_Init(GPIOB, &GPIO_InitStructure);				   	//根据参数初始化GPIOA寄存器	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; 
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//浮空输入(复位状态);
  	GPIO_Init(GPIOB, &GPIO_InitStructure);				   	//根据参数初始化GPIOA寄存器	
}


/*----------------------------------
**函数名称：Usart_Configuration
**功能描述：串口参数配置，包括中断向量的配置
**参数说明：BaudRate：需要配置的波特率
**作者：Andrew
**日期：2018.1.24
-----------------------------------*/
void Usart3_Configuration(uint32_t BaudRate)
{
	USART_InitTypeDef USART_InitStructure;							    	//定义一个串口结构体
	NVIC_InitTypeDef  NVIC_InitStructure;									//中断向量结构体
	
	USART_DeInit(USART3); 														//复位串口
	USART_InitStructure.USART_BaudRate            =BaudRate ;	  			//波特率自己设置，蓝牙设置了9600
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b; 	//传输过程中使用8位数据
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;	 	//在帧结尾传输1位停止位
	USART_InitStructure.USART_Parity              = USART_Parity_No ;	 	//奇偶失能
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//硬件流失能
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx; //接收和发送模式
	USART_Init(USART3, &USART_InitStructure);								//根据参数初始化串口寄存器
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);							//使能串口中断接收
	USART_Cmd(USART3, ENABLE);     											//使能串口外设
	
	//如下语句解决第1个字节无法正确发送出去的问题 
	USART_ClearFlag(USART3, USART_FLAG_TC);     /* 清发送外城标志，Transmission Complete flag */
	
	
	//配置串口中断向量
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //中断分组1
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;			//设定中断源为
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//中断占优先级为0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			//副优先级为0
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
void USART3_IRQHandler()
{
	unsigned int i = 0;
	
	if (USART_GetFlagStatus(USART3, USART_FLAG_ORE) != RESET)	//异常中断处理
    {
       USART_ReceiveData(USART3);										//丢弃数据
		 USART_ClearFlag(USART3, USART_FLAG_ORE);
    }
	 
	 /*发送*/
//	if(USART_GetITStatus(USART3,USART_IT_TXE) != RESET)
//	{
//		USART1->DR = Tx_Buffer[Tx_Counter_Temp++];    	//向DR寄存器写数据的时候，串口就会自动发送
//			
//		if(Tx_Counter == Tx_Counter_Temp)					//数据包发送完成
//		{
//		   USART_ITConfig(USART1,USART_IT_TXE,DISABLE);	//关闭TXE中断
//			Tx_Counter = 0;		//发送完成计数清零
//			Tx_Counter_Temp = 0;
//		}
//	}
	 
	 /*接受AAFB00036X\n*/
	if(USART_GetITStatus(USART3,USART_IT_RXNE) != RESET)	//读取接收中断标志位USART_IT_RXNE 
	{
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);	//清楚中断标志位
				
		USART3_Rx_Buffer[USART3_Rx_Counter++] = USART_ReceiveData(USART3);//接收数据到缓冲区，这个函数会自动清除中断标志位
		
		if(USART3_Rx_Buffer[USART3_Rx_Counter-1] == 00)//检测最后的结束标志
		{
			for(i = 0;i<USART3_Rx_Counter;i++)
			{
				USART3_CMD_Rx_Buffer[i] = USART3_Rx_Buffer[i];  //拷贝到命令缓冲区
			}
			USART3_CMD_Rx_Buffer[USART3_Rx_Counter] = 0;		//'\0'
			USART3_Rx_Counter = 0;
		}
	}

}


