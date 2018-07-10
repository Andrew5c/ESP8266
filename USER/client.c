
//ESP8266�ڵ�Ƭ������Ϊ�ͻ��ˣ���������������ݡ�

#include "main.h"


//��Ҫwifi�û�������������
#define WIFI_NAME      "123"
#define WIFI_PASSWORD  "123456789"

#define TCP_SERVER_IP   "192.168.43.95"		//������Ϊ�������ĵ�ַ
#define TCP_SERVER_PORT "1234"					//�����Զ���˿ں�

#define TCP_LEWEI_IP  "tcp.lewei50.com"		//�������������
#define TCP_LEWEI_PORT "9960"						//����˿ں�

extern unsigned char CmdRx_Buffer[20];

volatile uint8_t ucTcpClosedFlag = 0;

struct  STRUCT_USARTx_Fram strEsp8266_Fram_Record = { 0 };

//��¼ע����Ϣ
char userkey[100] = {"{\"method\": \"update\",\"gatewayNo\": \"01\",\"userkey\": \"71a1337f22ee404db399bb9894dfdf04\"}&^!"};
//����������
char value_common[100] = {"{\"method\": \"upload\",\"data\":[{\"Name\":\"P1\",\"Value\":\"100\"},{\"Name\":\"P2\",\"Value\":\"50\"}]}&^!"};

//����ҳ�����������ҳ��ʱ���������ᷢ�����������Ϣѯ�ʵ�ǰ��������״̬
char receive_get_sensor[] = {"{\"method\";\"send\",\"gatewayNo\":\"01\",\"userkey\":\"71a1337f22ee404db399bb9894dfdf04\",\"f\":\"getAllSensors\"}&^!"};

//���ص�ǰ���п�������״̬
char response_all_sensor_state_0[] = {"{\"method\": \"response\",\"result\": {\"successful\": true,\"message\": \"BEEP!\",\"data\":[{\"id\":\"BEEP\",\"value\":\"0\"},{\"id\":\"LED1\",\"value\":\"0\"},{\"id\":\"LED2\",\"value\":\"0\"}] }}&^!"};

//���ݿ��ƵĲ�ͬ��ִ���������ز�ͬ����Ϣ
char response_sensor_state_BEEP_1[] = {"{\"method\": \"response\",\"result\": {\"successful\": true,\"message\": \"BEEP\",\"data\":[{\"id\":\"BEEP\",\"value\":\"1\"}] }}&^!"};
char response_sensor_state_LED1_1[] = {"{\"method\": \"response\",\"result\": {\"successful\": true,\"message\": \"BEEP\",\"data\":[{\"id\":\"LED1\",\"value\":\"1\"}] }}&^!"};
char response_sensor_state_LED2_1[] = {"{\"method\": \"response\",\"result\": {\"successful\": true,\"message\": \"BEEP\",\"data\":[{\"id\":\"LED2\",\"value\":\"1\"}] }}&^!"};

char response_sensor_state_BEEP_0[] = {"{\"method\": \"response\",\"result\": {\"successful\": true,\"message\": \"BEEP\",\"data\":[{\"id\":\"BEEP\",\"value\":\"0\"}] }}&^!"};
char response_sensor_state_LED1_0[] = {"{\"method\": \"response\",\"result\": {\"successful\": true,\"message\": \"BEEP\",\"data\":[{\"id\":\"LED1\",\"value\":\"0\"}] }}&^!"};
char response_sensor_state_LED2_0[] = {"{\"method\": \"response\",\"result\": {\"successful\": true,\"message\": \"BEEP\",\"data\":[{\"id\":\"LED2\",\"value\":\"0\"}] }}&^!"};


//���������ҳ����л���ť��ʱ���������ᷢ��������Ϣ�Կ���������״̬���л�
//������淢�͵���0������ͻ��л�Ϊ1����֮�л�Ϊ0��
char receive_cmd_BEEP_1[] = {"{\"method\":\"send\",\"gatewayNo\":\"01\",\"userkey\":\"71a1337f22ee404db399bb9894dfdf04\",\"f\":\"updateSensor\",\"p1\":\"BEEP\",\"p2\":\"1\"}&^!"};
char receive_cmd_LED1_1[] = {"{\"method\":\"send\",\"gatewayNo\":\"01\",\"userkey\":\"71a1337f22ee404db399bb9894dfdf04\",\"f\":\"updateSensor\",\"p1\":\"LED1\",\"p2\":\"1\"}&^!"};
char receive_cmd_LED2_1[] = {"{\"method\":\"send\",\"gatewayNo\":\"01\",\"userkey\":\"71a1337f22ee404db399bb9894dfdf04\",\"f\":\"updateSensor\",\"p1\":\"LED2\",\"p2\":\"1\"}&^!"};

char receive_cmd_BEEP_0[] = {"{\"method\":\"send\",\"gatewayNo\":\"01\",\"userkey\":\"71a1337f22ee404db399bb9894dfdf04\",\"f\":\"updateSensor\",\"p1\":\"BEEP\",\"p2\":\"0\"}&^!"};
char receive_cmd_LED1_0[] = {"{\"method\":\"send\",\"gatewayNo\":\"01\",\"userkey\":\"71a1337f22ee404db399bb9894dfdf04\",\"f\":\"updateSensor\",\"p1\":\"LED1\",\"p2\":\"0\"}&^!"};
char receive_cmd_LED2_0[] = {"{\"method\":\"send\",\"gatewayNo\":\"01\",\"userkey\":\"71a1337f22ee404db399bb9894dfdf04\",\"f\":\"updateSensor\",\"p1\":\"LED2\",\"p2\":\"0\"}&^!"};



//��Ϊ�ͻ������ӵ����������ֵķ�����
void ESP8266_client_link_server(void)
{
//	uint8_t ucStatus;
	
//	char cStr [ 100 ] = { 0 };
		
   printf ( "\r\nConnecting...\r\n" );

	delay_ms(2000);		//��ʱ2s����ģ���Զ�����WiFi
	
	ESP8266_Cmd ( "AT+RST", "OK", "ready", 2500 );   	
	
//	ESP8266_AT_Test ();

//	ESP8266_Net_Mode_Choose ( STA );

   while ( ! ESP8266_JoinAP ( WIFI_NAME, WIFI_PASSWORD ) );
	
	printf("Connect success!");

//	ESP8266_Enable_MultipleId ( DISABLE );
	
	delay_ms(2000);
	
	//���ӷ�����
	while (! ESP8266_Link_Server( enumTCP, TCP_LEWEI_IP, TCP_LEWEI_PORT, Single_ID_0 ));
	
	//������͸ģʽSTA
	while (! ESP8266_Net_Mode_Choose(STA));
	
	//���봩͸ģʽ�շ�
	while (! ESP8266_UnvarnishSend());
	
	printf ( "\r\nSending...\r\n" );
	
	//�ȷ���һ�ε�¼��Ϣ
	//ESP8266_Send_Message(userkey,value_common);
	ESP8266_SendString(ENABLE, userkey, 0, Single_ID_0 );
	
	printf ( "Send Success!\r\n" );
	printf ("\n---------*****-----------\n");
	//�˳���û�м��������ӺͶϿ��������ܣ�ֻ�ܷ���һ�����ݣ����Ͽ����ӡ�
}

//��ʱ�������ݣ���ֹ��ֹ����
void ESP8266_Send_Message(char *_string_1, char *_string_2)
{
	delay_ms(2000);
	
	//������Ϊ�����涨�ĸ�ʽ������������͵�¼ע����Ϣ����������ҳ��ע����û���ź��Զ�������ر��
	//ESP8266_SendString ( ENABLE, "{\"method\": \"update\",\"gatewayNo\": \"01\",\"userkey\": \"71a1337f22ee404db399bb9894dfdf04\"}&^!", 0, Single_ID_0 );   //��������
	ESP8266_SendString(ENABLE, _string_1, 0, Single_ID_0 );
	
	delay_ms(2000);
	
	//�涨��ʽ���;�������
	//ESP8266_SendString ( ENABLE, "{\"method\": \"upload\",\"data\":[{\"Name\":\"P2\",\"Value\":\"GOOD\"}]}&^!", 0, Single_ID_0 );   //��������
	ESP8266_SendString(ENABLE, _string_2, 0, Single_ID_0 );
	
	delay_ms(2000);
}

/*
 * ��������ESP8266_Cmd
 * ����  ����WF-ESP8266ģ�鷢��ATָ��
 * ����  ��cmd�������͵�ָ��
 *         reply1��reply2���ڴ���������Ӧ��ΪNULL������Ӧ������Ϊ���߼���ϵ
 *         waittime���ȴ���Ӧ��ʱ��
 * ����  : 1��ָ��ͳɹ�
 *         0��ָ���ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_Cmd ( char * cmd, char * reply1, char * reply2, u32 waittime )
{    
	strEsp8266_Fram_Record .InfBit .FramLength = 0;               //���¿�ʼ�����µ����ݰ�

	macESP8266_Usart( "%s\r\n", cmd );

	if ( ( reply1 == 0 ) && ( reply2 == 0 ) )                      //����Ҫ�������ݣ���������
		return true;
	
	delay_ms ( waittime );                 //��ʱ�ȴ�ģ��Ӧ��
	
	strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ]  = '\0';

	printf( "%s", strEsp8266_Fram_Record .Data_RX_BUF );//��ӡ��ģ�����Ӧ
  
	if ( ( reply1 != 0 ) && ( reply2 != 0 ) )
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply1 ) || 
				   ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply2 ) ); 
 	
	else if ( reply1 != 0 )
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply1 ) );
	
	else
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply2 ) );
	
}

/*
 * ��������ESP8266_Net_Mode_Choose
 * ����  ��ѡ��WF-ESP8266ģ��Ĺ���ģʽ
 * ����  ��enumMode������ģʽ
 * ����  : 1��ѡ��ɹ�
 *         0��ѡ��ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_Net_Mode_Choose ( ENUM_Net_ModeTypeDef enumMode )
{
	switch ( enumMode )
	{
		case STA:
			return ESP8266_Cmd ( "AT+CWMODE=1", "OK", "no change", 2500 ); 
		
	  case AP:
		  return ESP8266_Cmd ( "AT+CWMODE=2", "OK", "no change", 2500 ); 
		
		case STA_AP:
		  return ESP8266_Cmd ( "AT+CWMODE=3", "OK", "no change", 2500 ); 
		
	  default:
		  return false;
  }
	
}



/*
 * ��������ESP8266_JoinAP
 * ����  ��WF-ESP8266ģ�������ⲿWiFi
 * ����  ��pSSID��WiFi�����ַ���
 *       ��pPassWord��WiFi�����ַ���
 * ����  : 1�����ӳɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_JoinAP ( char * pSSID, char * pPassWord )
{
	char cCmd [120];

	sprintf ( cCmd, "AT+CWJAP=\"%s\",\"%s\"", pSSID, pPassWord );
	
	return ESP8266_Cmd ( cCmd, "OK", NULL, 8000 );
	
}


/*
 * ��������ESP8266_Enable_MultipleId
 * ����  ��WF-ESP8266ģ������������
 * ����  ��enumEnUnvarnishTx�������Ƿ������
 * ����  : 1�����óɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_Enable_MultipleId ( FunctionalState enumEnUnvarnishTx )
{
	char cStr [20];
	
	sprintf ( cStr, "AT+CIPMUX=%d", ( enumEnUnvarnishTx ? 1 : 0 ) );
	return ESP8266_Cmd ( cStr, "OK", 0, 500 );
}

/*
 * ��������ESP8266_Link_Server
 * ����  ��WF-ESP8266ģ�������ⲿ������,��ʱ8266��Ϊ�ͻ���
 * ����  ��enumE������Э��
 *       ��ip��������IP�ַ���
 *       ��ComNum���������˿��ַ���
 *       ��id��ģ�����ӷ�������ID
 * ����  : 1�����ӳɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_Link_Server ( ENUM_NetPro_TypeDef enumE, char * ip, char * ComNum, ENUM_ID_NO_TypeDef id)
{
	char cStr [100] = { 0 }, cCmd [120];

  switch ( enumE )
  {
		case enumTCP:
		  sprintf ( cStr, "\"%s\",\"%s\",%s", "TCP", ip, ComNum );
		  break;
		
		case enumUDP:
		  sprintf ( cStr, "\"%s\",\"%s\",%s", "UDP", ip, ComNum );
		  break;
		
		default:
			break;
  }

  if ( id < 5 )
    sprintf ( cCmd, "AT+CIPSTART=%d,%s", id, cStr);

  else
	 sprintf ( cCmd, "AT+CIPSTART=%s", cStr );

	return ESP8266_Cmd ( cCmd, "OK", "ALREAY CONNECT", 4000 );
	
}

/*
 * ��������ESP8266_StartOrShutServer
 * ����  ��WF-ESP8266ģ�鿪����رշ�����ģʽ����ʱ��8266��Ϊ���������������ֻ���TCP��������
 * ����  ��enumMode������/�ر�
 *       ��pPortNum���������˿ں��ַ���
 *       ��pTimeOver����������ʱʱ���ַ�������λ����
 * ����  : 1�������ɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_StartOrShutServer ( FunctionalState enumMode, char * pPortNum, char * pTimeOver )
{
	char cCmd1 [120], cCmd2 [120];

	if ( enumMode )
	{
		sprintf ( cCmd1, "AT+CIPSERVER=%d,%s", 1, pPortNum );
		
		sprintf ( cCmd2, "AT+CIPSTO=%s", pTimeOver );

		return ( ESP8266_Cmd ( cCmd1, "OK", 0, 500 ) &&
						 ESP8266_Cmd ( cCmd2, "OK", 0, 500 ) );
	}
	
	else
	{
		sprintf ( cCmd1, "AT+CIPSERVER=%d,%s", 0, pPortNum );

		return ESP8266_Cmd ( cCmd1, "OK", 0, 500 );
	}
	
}


/*
 * ��������ESP8266_UnvarnishSend
 * ����  ������WF-ESP8266ģ�����͸������
 * ����  ����
 * ����  : 1�����óɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_UnvarnishSend ( void )
{
	if ( ! ESP8266_Cmd ( "AT+CIPMODE=1", "OK", 0, 500 ) )
		return false;
	
	return 
	  ESP8266_Cmd ( "AT+CIPSEND", "OK", ">", 500 );
	
}


/*
 * ��������ESP8266_SendString
 * ����  ��WF-ESP8266ģ�鷢���ַ���
 * ����  ��enumEnUnvarnishTx�������Ƿ���ʹ����͸��ģʽ
 *       ��pStr��Ҫ���͵��ַ���
 *       ��ulStrLength��Ҫ���͵��ַ������ֽ���
 *       ��ucId���ĸ�ID���͵��ַ���
 * ����  : 1�����ͳɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_SendString ( FunctionalState enumEnUnvarnishTx, char * pStr, u32 ulStrLength, ENUM_ID_NO_TypeDef ucId )
{
	char cStr [20];
	bool bRet = false;
	
		
	if ( enumEnUnvarnishTx )
	{
		macESP8266_Usart ( "%s", pStr );
		
		bRet = true;
		
	}

	else
	{
		if ( ucId < 5 )
			sprintf ( cStr, "AT+CIPSEND=%d,%d", ucId, ulStrLength + 2 );

		else
			sprintf ( cStr, "AT+CIPSEND=%d", ulStrLength + 2 );
		
		ESP8266_Cmd ( cStr, "> ", 0, 1000 );

		bRet = ESP8266_Cmd ( pStr, "SEND OK", 0, 1000 );
  }
	
	return bRet;

}


