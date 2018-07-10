
#include "main.h"




//把上面的数据按照乐联网固定的格式写入这个数组
char paper_state[100] = {0};
volatile unsigned char receive_paper_flag = 0;
//串口中接收外部蓝牙发送过来的纸的剩余量保存在这个缓冲区，定时发送到网页上
volatile unsigned int box_1_paper_left = 0;

/*
** JDY组网蓝牙模块要求数据以16位格式发送，那么需要吧原始的8位数据两两合并成16位的数据在进行发送
** 原始数据为：AAFB0003EE
** 意思是向地址编号为03的模块发送数据EE，即作为出纸信号，又作为结束信号
** 所有模块的广播名都为456，必须一致
*/
const char master_to_bluetooth3[7] = {0XAA,0XFB,0X00,0X03,0XEE};
const char master_to_bluetooth1[7] = {0XAA,0XFB,0X00,0X01,0XEE};
const char master_to_bluetooth4[7] = {0XAA,0XFB,0X00,0X04,0XEE};


static void Keep_Linking(void);
static void Reply_Web(void);


int main()
{
	#warning "this is the master device...MADDR = 02."
	
	bluetooth_init(115200);
		
	ESP8266_init(115200);
	
	Beep_Init();
	
	printf("\r\n  ESP8266 TEST   \r\n");
	
	//直接连接云服务器，先发送一次数据
	ESP8266_client_link_server();
	
	Time_2_Init(10);//定时10ms

	strEsp8266_Fram_Record .InfBit .FramLength = 0; 
	
	//提示链接成功，进入监听
	Beep_Start();
	
	while(1)
	{		
		if(time_flag_1 >= 3000)
		{
			time_flag_1 = 0;
			Keep_Linking();
		}
		
		if(receive_flag == 1)
		{
			receive_flag = 0;
			Reply_Web();		
		}
	
	}
}


/*
** 定时发送一次数据，以保持与服务器的连接，数据具体可以是厕纸的剩余量
*/
static void Keep_Linking(void)
{
	if(receive_paper_flag == 1)
	{
		receive_paper_flag = 0;
		//如果接受到从机发送过来的厕纸余量信息，就上传这个数据
		sprintf(paper_state,"{\"method\": \"upload\",\"data\":[{\"Name\":\"P1\",\"Value\":\"%d\"}]}&^!", box_1_paper_left);
		ESP8266_SendString(ENABLE, paper_state, 0, Single_ID_0 );
		Delay_ms(500);
	}

	//每一次发送登录信息，网页都会回复一个消息
	//ESP8266_Send_Message(userkey,paper_state);
	ESP8266_SendString(ENABLE, userkey, 0, Single_ID_0 );
	
	//登录之后立即回复网页当前所有传感器的状态，确保可以多次切换
	ESP8266_SendString(ENABLE, response_all_sensor_state_0, 0, Single_ID_0 );
	//等待网页回复
	Delay_ms(1000);
	//串口字符串缓冲区计数清零
	strEsp8266_Fram_Record .InfBit .FramLength = 0; 
	
	receive_flag = 0;
	//给出提示
	printf ("\n Keep Linking...\n");
		
}

/*
** 接收到网页发送过来的命令
*/
static void Reply_Web(void)
{
	//能正确的打印出网页返回的信息
	printf("%s\n",receive_buf);
	
	//如果网页进入控制端，立即给出回复
	if(strstr(receive_buf,"getAllSensors"))
	{
		printf("receive ask message...\n");
		
		//向服务器发出信号，表示当前执行器的状态是0
		ESP8266_SendString(ENABLE, response_all_sensor_state_0, 0, Single_ID_0 );
		
		printf ( "\n prepare for reveiving CMD...\r\n" );
		
		memset(receive_buf,0,sizeof(receive_buf));		//数组清零
	}
	
	//如果检测到网页发来状态转换信息，给予回复，并且转换某执行器的状态
	
	//动作一
	else if(strstr(receive_buf,receive_cmd_BEEP_1) || strstr(receive_buf,receive_cmd_BEEP_0))
	{
		if(strstr(receive_buf,receive_cmd_BEEP_1))
			ESP8266_SendString(ENABLE, response_sensor_state_BEEP_1, 0, Single_ID_0 );
		else
			ESP8266_SendString(ENABLE, response_sensor_state_BEEP_0, 0, Single_ID_0 );
		
		memset(receive_buf,0,sizeof(receive_buf));		//数组清零
		
		//执行器动作函数
		Beep_Warning(2);
		//向1号厕纸盒发送出纸信号。66表示出纸
		Send_Bluetooth(master_to_bluetooth1);
	}
	
	//动作二
	else if(strstr(receive_buf,receive_cmd_LED1_1) || strstr(receive_buf,receive_cmd_LED1_0))
	{
		if(strstr(receive_buf,receive_cmd_LED1_1))
			ESP8266_SendString(ENABLE, response_sensor_state_LED1_1, 0, Single_ID_0 );
		else
			ESP8266_SendString(ENABLE, response_sensor_state_LED1_0, 0, Single_ID_0 );

		//数组清零
		memset(receive_buf,0,sizeof(receive_buf));	
	
		//执行器动作函数
		Beep_Warning(3);
		//向2号厕纸盒发送出纸信号。66表示出纸
		Send_Bluetooth(master_to_bluetooth3);
	}
	
	//动作三
	else if(strstr(receive_buf,receive_cmd_LED2_1) || strstr(receive_buf,receive_cmd_LED2_0))
	{
		if(strstr(receive_buf,receive_cmd_LED2_1))
			ESP8266_SendString(ENABLE, response_sensor_state_LED2_1, 0, Single_ID_0 );
		else
			ESP8266_SendString(ENABLE, response_sensor_state_LED2_0, 0, Single_ID_0 );
		
		//数组清零
		memset(receive_buf,0,sizeof(receive_buf));	
	
		//执行器动作函数
		Beep_Warning(4);
		//向3号厕纸盒发送出纸信号。66表示出纸
		Send_Bluetooth(master_to_bluetooth4);
	}

}


