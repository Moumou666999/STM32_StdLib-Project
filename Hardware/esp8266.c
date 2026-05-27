/*这个文件中需要实现AT指令发送，查找接收到的消息是否应答，应答成功或者失败
*/
#include "stm32f10x.h"                  // Device header
#include "BtSerial.h"
#include "Delay.h"
#include <stdio.h>
#include <string.h>
#include "MQTT.h"
#define ESP8266_RETRY_TIMES        3
#define ESP8266_DEFAULT_TIMEOUT_MS 5000

void ESP8266_SendSting(const char *String)
{
	BtSerial_SendString(String);
}

uint8_t ESP8266_SendWait(const char *cmd,const char *ack,uint32_t timeout_ms)
{
	uint32_t elapsed = 0;
	BtSerial_SendString(cmd);
	
	while(elapsed < timeout_ms)
	{
		char line[256];
		if(BtSerial_ReadLine(line,sizeof(line),100))
		{
			if(strstr(line,"MQTTSUBRECV"))
			{
				
				continue;
			}
			if(strstr(line, "busy"))
			{
				continue;
			}
			if(strstr(line ,ack) != NULL)
			{
				return 1;
			}
		}
		else 
		{
			Delay_ms(10);
			elapsed += 10;
		}
	}
	return 0;
}

uint8_t ESP8266_WaitFor(const char *ack,uint32_t timeout_ms)
{
	uint32_t elapsed = 0;
	while(elapsed < timeout_ms)
	{
		char line[256];
		if(BtSerial_ReadLine(line,sizeof(line),timeout_ms - elapsed))
		{
			if(strstr(line ,ack) != NULL)
			{
				return 1;
			}
		}
		else
		{
			Delay_ms(10);
			elapsed +=10;
		}
	}
	return 0;
}

uint8_t ESP8266_DisableEcho(uint32_t timeout_ms)
{
	return ESP8266_SendWait("ATE0\r\n","OK",timeout_ms);
}

uint8_t ESP8266_SendCmdOK(const char *cmd,uint32_t timeout_ms)
{
	return ESP8266_SendWait(cmd, "OK", timeout_ms);
}

uint8_t ESP8266_Init(void)
{
	uint8_t i;
	BtSerial_Init();
	
	ESP8266_DisableEcho(2000);
	
	for(i = 0; i < ESP8266_RETRY_TIMES; i++)
	{
		if(ESP8266_SendCmdOK("AT\r\n", ESP8266_DEFAULT_TIMEOUT_MS))
		{
			return 1;
		}
		Delay_ms(1000);
	}
	return 0;
}

uint8_t ESP8266_ConnectWIFI(char *ssid, char *pwd)
{
	char cmd[128];
	if(!ESP8266_SendCmdOK("AT+CWMODE=1\r\n", 2000)) return 0;
	sprintf(cmd,"AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, pwd);
	if(!ESP8266_SendWait(cmd, "WIFI GOT IP",15000)) return 0;
	return 1;
}

uint8_t ESP8266_ConnectServer(char *ip,uint16_t port)
{
	char cmd[128];
	if(!ESP8266_SendCmdOK("AT+CIPMUX=0\r\n",2000)) return 0;
	sprintf(cmd,"AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", ip, port);
	if(!ESP8266_SendWait(cmd,"CONNECT",10000)) return 0;
	return 1;
}

uint16_t ESP8266_SendData(uint8_t *data, uint16_t len)
{
	char cmd[32];
	sprintf(cmd, "AT+CIPSEND=%d\r\n",len);
	if(!ESP8266_SendWait(cmd, ">", 2000)) return 0;
	BtSerial_SendArray(data, len);
	if(ESP8266_WaitFor("SEND OK", 5000)) return len;
	return 0;
}

uint8_t ESP8266_CloseConnection(void)
{
	return ESP8266_SendWait("AT+CIPCLOSE\r\n", "CLOSED", 3000);
}

