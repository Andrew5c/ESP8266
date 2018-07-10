
#include "main.h"




//����������ݰ����������̶��ĸ�ʽд���������
char paper_state[100] = {0};
volatile unsigned char receive_paper_flag = 0;
//�����н����ⲿ�������͹�����ֽ��ʣ�����������������������ʱ���͵���ҳ��
volatile unsigned int box_1_paper_left = 0;

/*
** JDY��������ģ��Ҫ��������16λ��ʽ���ͣ���ô��Ҫ��ԭʼ��8λ���������ϲ���16λ�������ڽ��з���
** ԭʼ����Ϊ��AAFB0003EE
** ��˼�����ַ���Ϊ03��ģ�鷢������EE������Ϊ��ֽ�źţ�����Ϊ�����ź�
** ����ģ��Ĺ㲥����Ϊ456������һ��
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
	
	//ֱ�������Ʒ��������ȷ���һ������
	ESP8266_client_link_server();
	
	Time_2_Init(10);//��ʱ10ms

	strEsp8266_Fram_Record .InfBit .FramLength = 0; 
	
	//��ʾ���ӳɹ����������
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
** ��ʱ����һ�����ݣ��Ա���������������ӣ����ݾ�������ǲ�ֽ��ʣ����
*/
static void Keep_Linking(void)
{
	if(receive_paper_flag == 1)
	{
		receive_paper_flag = 0;
		//������ܵ��ӻ����͹����Ĳ�ֽ������Ϣ�����ϴ��������
		sprintf(paper_state,"{\"method\": \"upload\",\"data\":[{\"Name\":\"P1\",\"Value\":\"%d\"}]}&^!", box_1_paper_left);
		ESP8266_SendString(ENABLE, paper_state, 0, Single_ID_0 );
		Delay_ms(500);
	}

	//ÿһ�η��͵�¼��Ϣ����ҳ����ظ�һ����Ϣ
	//ESP8266_Send_Message(userkey,paper_state);
	ESP8266_SendString(ENABLE, userkey, 0, Single_ID_0 );
	
	//��¼֮�������ظ���ҳ��ǰ���д�������״̬��ȷ�����Զ���л�
	ESP8266_SendString(ENABLE, response_all_sensor_state_0, 0, Single_ID_0 );
	//�ȴ���ҳ�ظ�
	Delay_ms(1000);
	//�����ַ�����������������
	strEsp8266_Fram_Record .InfBit .FramLength = 0; 
	
	receive_flag = 0;
	//������ʾ
	printf ("\n Keep Linking...\n");
		
}

/*
** ���յ���ҳ���͹���������
*/
static void Reply_Web(void)
{
	//����ȷ�Ĵ�ӡ����ҳ���ص���Ϣ
	printf("%s\n",receive_buf);
	
	//�����ҳ������ƶˣ����������ظ�
	if(strstr(receive_buf,"getAllSensors"))
	{
		printf("receive ask message...\n");
		
		//������������źţ���ʾ��ǰִ������״̬��0
		ESP8266_SendString(ENABLE, response_all_sensor_state_0, 0, Single_ID_0 );
		
		printf ( "\n prepare for reveiving CMD...\r\n" );
		
		memset(receive_buf,0,sizeof(receive_buf));		//��������
	}
	
	//�����⵽��ҳ����״̬ת����Ϣ������ظ�������ת��ĳִ������״̬
	
	//����һ
	else if(strstr(receive_buf,receive_cmd_BEEP_1) || strstr(receive_buf,receive_cmd_BEEP_0))
	{
		if(strstr(receive_buf,receive_cmd_BEEP_1))
			ESP8266_SendString(ENABLE, response_sensor_state_BEEP_1, 0, Single_ID_0 );
		else
			ESP8266_SendString(ENABLE, response_sensor_state_BEEP_0, 0, Single_ID_0 );
		
		memset(receive_buf,0,sizeof(receive_buf));		//��������
		
		//ִ������������
		Beep_Warning(2);
		//��1�Ų�ֽ�з��ͳ�ֽ�źš�66��ʾ��ֽ
		Send_Bluetooth(master_to_bluetooth1);
	}
	
	//������
	else if(strstr(receive_buf,receive_cmd_LED1_1) || strstr(receive_buf,receive_cmd_LED1_0))
	{
		if(strstr(receive_buf,receive_cmd_LED1_1))
			ESP8266_SendString(ENABLE, response_sensor_state_LED1_1, 0, Single_ID_0 );
		else
			ESP8266_SendString(ENABLE, response_sensor_state_LED1_0, 0, Single_ID_0 );

		//��������
		memset(receive_buf,0,sizeof(receive_buf));	
	
		//ִ������������
		Beep_Warning(3);
		//��2�Ų�ֽ�з��ͳ�ֽ�źš�66��ʾ��ֽ
		Send_Bluetooth(master_to_bluetooth3);
	}
	
	//������
	else if(strstr(receive_buf,receive_cmd_LED2_1) || strstr(receive_buf,receive_cmd_LED2_0))
	{
		if(strstr(receive_buf,receive_cmd_LED2_1))
			ESP8266_SendString(ENABLE, response_sensor_state_LED2_1, 0, Single_ID_0 );
		else
			ESP8266_SendString(ENABLE, response_sensor_state_LED2_0, 0, Single_ID_0 );
		
		//��������
		memset(receive_buf,0,sizeof(receive_buf));	
	
		//ִ������������
		Beep_Warning(4);
		//��3�Ų�ֽ�з��ͳ�ֽ�źš�66��ʾ��ֽ
		Send_Bluetooth(master_to_bluetooth4);
	}

}


