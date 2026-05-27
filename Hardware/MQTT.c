#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "Delay.h"
#include "esp8266.h"
#include "BtSerial.h"
#include <string.h>
#include "OLED.h"
#include "MQTT.h"

#define WIFI_SSID      "lalala666"
#define WIFI_PWD       "t3629dm5"

#define MQTT_USER_CFG  "AT+MQTTUSERCFG=0,1,\"STM32-sensor\",\"R4EANcG43p\",\"version=2018-10-31&res=products%2FR4EANcG43p%2Fdevices%2FSTM32-sensor&et=1808292700&method=sha1&sign=TSDblrb56y624F8noOmzufKexdU%3D\",0,0,\"\"\r\n"
#define MQTT_CONN_CFG  "AT+MQTTCONN=0,\"mqtts.heclouds.com\",1883,1\r\n"

#define MQTT_SUB_REPLY "\"$sys/R4EANcG43p/STM32-sensor/thing/property/post/reply\",1\r\n"
#define MQTT_SUB_SET   "\"$sys/R4EANcG43p/STM32-sensor/thing/property/set\",1\r\n"

#define MQTT_PUB_TOPIC "$sys/R4EANcG43p/STM32-sensor/thing/property/post"
#define MQTT_SET_REPLY "$sys/R4EANcG43p/STM32-sensor/thing/property/set_reply"



uint8_t WaitFor(const char *ack, uint32_t timeout_ms)
{
	uint32_t elapsed = 0;
	while(elapsed < timeout_ms)
	{
		char line[256];
		if(BtSerial_ReadLine(line, sizeof(line),100))
		{
			if(strstr(line, ack)) return 1;
		}
		else
		{
			Delay_ms(10);
			elapsed += 10;
		}
	}
	return 0;
}

void MQTT_PublishStatusData(uint8_t led1, uint8_t fan, uint8_t buzzer)
{
	char json[500];
	sprintf(json, 
        "{\\\"id\\\":\\\"123\\\"\\,\\\"version\\\":\\\"1.0\\\"\\,\\\"params\\\":{" 
	      "\\\"buzzer\\\":{\\\"value\\\":%s}\\," 
	      "\\\"fan\\\":{\\\"value\\\":%s}\\,"
        "\\\"LED1\\\":{\\\"value\\\":%s}}}", 
      	buzzer ? "true" : "false",
	      fan ? "true" : "false",
        led1 ? "true" : "false"); 
	char cmd[700];
	sprintf(cmd, "AT+MQTTPUB=0,\"%s\",\"%s\",0,0\r\n", MQTT_PUB_TOPIC, json); 
//	OLED_ShowNum(0,0,strlen(cmd),4,OLED_6X8);
//	OLED_Update();
    BtSerial_SendString(cmd); 
    Delay_ms(500); 
}

void MQTT_PublishStatusDataLed(uint8_t led1)
{
	char json[500];
	sprintf(json, 
        "{\\\"id\\\":\\\"123\\\"\\,\\\"version\\\":\\\"1.0\\\"\\,\\\"params\\\":{" 
        "\\\"LED1\\\":{\\\"value\\\":%s}}}", 
        led1 ? "true" : "false"); 
	char cmd[300];
	sprintf(cmd, "AT+MQTTPUB=0,\"%s\",\"%s\",0,0\r\n", MQTT_PUB_TOPIC, json); 
//		OLED_ShowNum(0,0,strlen(cmd),4,OLED_6X8);
//	OLED_Update();
    BtSerial_SendString(cmd); 
    Delay_ms(500); 
}

void MQTT_PublishStatusDataFan(uint8_t fan)
{
	char json[500];
	sprintf(json, 
        "{\\\"id\\\":\\\"123\\\"\\,\\\"version\\\":\\\"1.0\\\"\\,\\\"params\\\":{" 
        "\\\"fan\\\":{\\\"value\\\":%s}}}", 
        fan ? "true" : "false"); 
	char cmd[300];
	sprintf(cmd, "AT+MQTTPUB=0,\"%s\",\"%s\",0,0\r\n", MQTT_PUB_TOPIC, json); 
    BtSerial_SendString(cmd); 
    Delay_ms(500); 
}

void MQTT_PublishStatusDataStation(uint8_t Station)
{
	char json[500];
	sprintf(json, 
        "{\\\"id\\\":\\\"123\\\"\\,\\\"version\\\":\\\"1.0\\\"\\,\\\"params\\\":{" 
        "\\\"station\\\":{\\\"value\\\":%s}}}", 
        Station ? "true" : "false"); 
	char cmd[300];
	sprintf(cmd, "AT+MQTTPUB=0,\"%s\",\"%s\",0,0\r\n", MQTT_PUB_TOPIC, json); 
    BtSerial_SendString(cmd); 
    Delay_ms(500); 
}

void MQTT_PublishStatusDataBuzzer(uint8_t buzzer)
{
	char json[500];
	sprintf(json, 
        "{\\\"id\\\":\\\"123\\\"\\,\\\"version\\\":\\\"1.0\\\"\\,\\\"params\\\":{" 
        "\\\"buzzer\\\":{\\\"value\\\":%s}}}", 
        buzzer ? "true" : "false"); 
	char cmd[300];
	sprintf(cmd, "AT+MQTTPUB=0,\"%s\",\"%s\",0,0\r\n", MQTT_PUB_TOPIC, json); 
    BtSerial_SendString(cmd); 
    Delay_ms(500); 
}

void MQTT_PublishSensorData(float smoke, int temp, int humi)
{
	char json[400];
	sprintf(json, 
	"{\\\"id\\\": \\\"123\\\"\\,\\\"version\\\":\\\"1.0\\\"\\,\\\"params\\\":{"
	"\\\"MQ2\\\": {\\\"value\\\": %.2f}\\,"
	"\\\"temp\\\": {\\\"value\\\": %d}\\,"
	"\\\"humi\\\": {\\\"value\\\": %d}}}",
	smoke, temp, humi);
	
	char cmd[600];
	sprintf(cmd, "AT+MQTTPUB=0,\"%s\",\"%s\",0,0\r\n", MQTT_PUB_TOPIC, json);
	BtSerial_SendString(cmd);
	Delay_ms(500);
}

void MQTT_Init(void)
{
	OLED_ShowString(0,0,"System Init", OLED_8X16);
	OLED_ShowString(0,20,"Starting...", OLED_8X16);
	OLED_Update();
	Delay_ms(500);
	
	OLED_Clear();
	OLED_ShowString(0,0,"Factory Reset", OLED_8X16);
	OLED_ShowString(0,20,"AT+RESTORE", OLED_8X16);
	OLED_Update();
	BtSerial_SendString("AT+RESTORE\r\n");
	if(!WaitFor("ready",1500))
	{
		OLED_ShowString(0,40,"Reset Fail", OLED_8X16);
		while(1);
	}
	OLED_ShowString(0,40,"OK",OLED_8X16);
	OLED_Update();
	Delay_ms(500);
	
	OLED_Clear();
	OLED_ShowString(0,0,"AT Test", OLED_8X16);
	OLED_ShowString(0,20,"Sending...", OLED_8X16);
	OLED_Update();
  if(!ESP8266_SendCmdOK("AT\r\n", 3000))
	{
		OLED_ShowString(0,40,"AT Test Fail", OLED_8X16);
		while(1);
	}
  OLED_ShowString(0, 40,"OK", OLED_8X16);
	OLED_Update();
  Delay_ms(500);
  
	OLED_Clear();
	OLED_ShowString(0,0,"Disable Echo" , OLED_8X16);
	OLED_ShowString(0,20,"ATE0", OLED_8X16);
	ESP8266_DisableEcho(3000);
	OLED_Update();
	Delay_ms(500);
	
	OLED_Clear();
	OLED_ShowString(0,0,"WIFI Mode", OLED_8X16);
	OLED_ShowString(0,20,"Station", OLED_8X16);
	ESP8266_SendCmdOK("AT+CWMODE=1\r\n", 2000);
	OLED_Update();
	Delay_ms(500);
	
	OLED_Clear();
	OLED_ShowString(0,0,"WIFI Connect", OLED_8X16);
	OLED_ShowString(0,20,WIFI_SSID, OLED_8X16);
	OLED_Update();
	if(!ESP8266_ConnectWIFI(WIFI_SSID, WIFI_PWD))
	{
		OLED_ShowString(0,40,"WIFI Fail", OLED_8X16);
		while(1);
	}
	OLED_ShowString(0,40,"OK", OLED_8X16);
	OLED_Update();
	Delay_ms(1000);
	
	OLED_Clear();
	OLED_ShowString(0,0,"MQTT Config", OLED_8X16);
	OLED_ShowString(0,20,"User Params", OLED_8X16);
	OLED_Update();
	if(!ESP8266_SendCmdOK(MQTT_USER_CFG, 5000))
	{
		OLED_ShowString(0,40,"MQTT User Fail" ,OLED_8X16);
		while(1);
	}
	OLED_ShowString(0,40,"OK", OLED_8X16);
	OLED_Update();
	Delay_ms(500);
	
	OLED_Clear();
	OLED_ShowString(0,0,"MQTT Connect",OLED_8X16);
	OLED_ShowString(0,20,"mqtts.hecluds.com", OLED_8X16);
	OLED_Update();
	if(!ESP8266_SendCmdOK(MQTT_CONN_CFG, 10000))
	{
		OLED_ShowString(0,40,"MQTT Conn Fail",OLED_8X16);
		while(1);
	}
	OLED_ShowString(0,40,"OK",OLED_8X16);
	OLED_Update();
	Delay_ms(500);
	
	OLED_Clear();
	OLED_ShowString(0,0,"MQTT Subscrible" ,OLED_8X16);
	OLED_ShowString(0,20,"post/reply", OLED_8X16);
	OLED_Update();
	{
		char cmd[200];
		sprintf(cmd, "AT+MQTTSUB=0,%s",MQTT_SUB_REPLY);
		if(!ESP8266_SendCmdOK(cmd, 10000))
		{
			OLED_ShowString(0,40,"SUB1 Fail", OLED_8X16);
			while(1);
		}
		OLED_ShowString(0,40,"OK" ,OLED_8X16);
		Delay_ms(1500);
	}
	OLED_Update();
	
	OLED_Clear();
	OLED_ShowString(0,0,"MQTT Subscribe" ,OLED_8X16);
	OLED_ShowString(0,20,"property/set",OLED_8X16);
	OLED_Update();
	{
		char cmd[200];
		sprintf(cmd, "AT+MQTTSUB=0,%s", MQTT_SUB_SET);
		if(!ESP8266_SendCmdOK(cmd, 10000))
		{
			OLED_ShowString(0,40,"SUB2 Fail", OLED_8X16);
			while(1);
		}
		OLED_ShowString(0,40,"OK", OLED_8X16);
		Delay_ms(1500);
		
	}
	OLED_Update();
	Delay_ms(500);
	OLED_Clear();
	OLED_Update();
}

void ProcessMQTTMessage(char *line,uint8_t *led1_last, uint8_t *fan_last , uint8_t *buzzer_last)
{
	
	if(strstr(line, "+MQTTSUBRECV") == NULL) return;
	
	if(strstr(line, "property/set") == NULL) return;
	
	char *payload = strchr(line ,'{');
	if(!payload) return;
	
	char id[16] = "123";
	char *p = strstr(payload, "\"id\":\"");
	if(p){
		p += 6;
		char *q = strchr(p, '\"');
		if(q){
			int len = q - p;
			if(len < sizeof(id)){
				strncpy(id, p, len);
				id[len] = '\0';
			}
		}
	}
	
	int led1 = -1;
	int fan = -1;
	int buzzer = -1;
	int station = -1;
	
	if(strstr(payload, "\"LED1\":true")) led1 = 1;
	else if (strstr(payload, "\"LED1\":false")) led1 = 0;
	
	if(strstr(payload, "\"fan\":true")) fan = 1;
	else if (strstr(payload, "\"fan\":false")) fan = 0;
	
	if(strstr(payload, "\"buzzer\":true")) buzzer = 1;
	else if (strstr(payload, "\"buzzer\":false")) buzzer = 0;
	
	if(strstr(payload, "\"station\":true")) station = 1;
	else if (strstr(payload, "\"station\":false")) station = 0;
	
	extern int led1_state;
	extern uint8_t fan_state;
	extern uint8_t buzzer_state;
	extern uint8_t station_state;
	
	station_state = (station != -1) ? station : station_state;
	
	if(station_state == 1)
	{
		led1_state = (led1 != -1) ? led1 : led1_state;
	  fan_state = (fan != -1) ? fan : fan_state;
	  buzzer_state = (buzzer != -1) ? buzzer : buzzer_state;
		
		
	  char reply_json[128];
	  sprintf(reply_json, "{\\\"id\\\":\\\"%s\\\"\\,\\\"code\\\":200\\,\\\"msg\\\":\\\"success\\\"}", id);
	  char reply_cmd[256];
	  sprintf(reply_cmd, "AT+MQTTPUB=0,\"%s\",\"%s\",0,0\r\n", 
            MQTT_SET_REPLY, reply_json); 
	  BtSerial_SendString(reply_cmd);
	  Delay_ms(500);
	}
	
	if(station_state == 0 || station_state == 1)
	{
		if(station_state == 0)
		{
			fan_state = 0;
			buzzer_state = 0;
			led1_state = 0;
			*led1_last = 1;
			*fan_last = 1;
			*buzzer_last = 1;
			
		}
		char reply_json[128];
	sprintf(reply_json, "{\\\"id\\\":\\\"%s\\\"\\,\\\"code\\\":200\\,\\\"msg\\\":\\\"success\\\"}", id);
	char reply_cmd[256];
	sprintf(reply_cmd, "AT+MQTTPUB=0,\"%s\",\"%s\",0,0\r\n", 
            MQTT_SET_REPLY, reply_json); 
	BtSerial_SendString(reply_cmd);
	Delay_ms(500);
	}
}

