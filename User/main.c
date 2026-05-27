#include "stm32f10x.h"  // Device header
#include "Delay.h"
#include "OLED.h"
#include "SHT30.h"
#include "BtSerial.h"
#include "MQTT.h"
#include "string.h"
#include "LED.h"
#include "queue.h"
#include "MQ2.h"
#include "Key.h"
#include "menu.h"
#include "PWM.h"
#include "BUZZER.h"

uint8_t SHT30;
uint8_t Keynum;

uint8_t led_last = 1;
uint8_t led1_state = 0;//警报灯
uint8_t fan_last = 1;
uint8_t fan_state = 0;//风扇情况1动0停
uint8_t buzzer_last = 1;
uint8_t buzzer_state = 0;//蜂鸣器
uint8_t station_last = 1;
uint8_t station_state = 0;//1手动0自动

float smoke;
uint8_t temp;
uint8_t humi;

uint8_t T_temp = 24;
uint8_t T_humi = 70;
int16_t T_rate = 20;

char line[256];
char copy[256];

int esp_wifi = 1;//WiFi连接状态标志位
int esp_busy = 0;//忙碌标志位
int esp_mqtt = 1;//MQTT连接标志位
int mqtt_msg_flag;//接收云端消息标志位

volatile uint32_t sys_tick = 0;


void SysTick_Init(void)
{
    SysTick_Config(SystemCoreClock /1000);
}

void SysTick_Handler(void)
{
	sys_tick++;
}

uint32_t millis(void)
{
	return sys_tick;
}

void MQTT_Publish(void)
{
	  
    static	uint32_t last_time1 = 0;
	// static  uint32_t last_time2 = 0;
	  if(millis() - last_time1 >= 2000)
	  {
			last_time1 = millis();
			Message p1;
			p1.type = MSG_TEMP_HUMI;
		  p1.data.th.humi = humi;
			p1.data.th.temp = temp;
			p1.data.th.smoke = smoke;
			Queue_Push(&p1);
		}
		
//		if(millis() - last_time2 >= 4000) //修改方案，结构体只赋值一个变量，并且只发送一个变量到云端字节为150，
//			{                                 //如果一起发送字节将到220，可能会丢失，思路，定义一个全局变量结构体，
//				last_time2 = millis();           //当什么发生改变时,在定义局部变量结构体，只赋值一个变了的变量，再进行单变量发送。待解决
//			Message p1;
//			p1.type = MSG_NOW;
//			p1.data.led = 1;
		//	p1.data.buzzer = 0;
		//	p1.data.fan = 0;
	  //	p1.data.station = 0;
		//	Queue_Push(&p1);
		//}
//		  Message p1;
//			p1.type = MSG_NOW;
//			p1.data.led = 1;
//			p1.data.buzzer = 1;
//			p1.data.fan = 1;
//	  	p1.data.station = 1;
		if(led1_state != led_last )
		{
			led_last = led1_state;
			Message p2;
			p2.type = MSG_LED;
			p2.data.led = led1_state;
//			p2.data.buzzer = buzzer_state;
//			p2.data.fan = fan_state;
//			p2.data.station = station_state;
			Queue_Push(&p2);
		}
    if(fan_state != fan_last )
		{
			fan_last = fan_state;
			Message p3;
			p3.type = MSG_FAN;
//			p3.data.led = led1_state;
//			p3.data.buzzer = buzzer_state;
			p3.data.fan = fan_state;
//			p3.data.station = station_state;
			Queue_Push(&p3);
		}  
     if(station_state != station_last )
		{
			station_last = station_state;
			Message p4;
			p4.type = MSG_STATION;
//		  p4.data.led = led1_state;
//			p4.data.buzzer = buzzer_state;
//			p4.data.fan = fan_state;
			p4.data.station = station_state;
			Queue_Push(&p4);
		}
    if(buzzer_state != buzzer_last )
		{
			buzzer_last = buzzer_state;
			Message p5;
			p5.type = MSG_BUZZER;
//			p5.data.led = led1_state;
			p5.data.buzzer = buzzer_state;
//			p5.data.fan = fan_state;
//			p5.data.station = station_state;
			Queue_Push(&p5);
		}
}

void MQTT_Station(void)
{
	if(BtSerial_ReadLine(line,sizeof(line),10))
	{
		if(strstr(line,"OK"))
		{
			
			esp_busy = 0;
		}
		else if(strstr(line,"busy"))
		{
			esp_busy = 1;
		}
		else if(strstr(line, "+MQTTSUBRECV"))
			{
				mqtt_msg_flag = 1;
				strcpy(copy, line);
			}
			else if(strstr(line, "WIFI GOT IP"))
			{
				esp_wifi = 1;
			}
			else if(strstr(line, "+MQTTDISCONNECTED"))
			{
				esp_mqtt = 2;
			}
			else if(strstr(line, "WIFI DISCONNECT"))
			{
				esp_wifi = 2;
			}
			else if(strstr(line, "+MQTTCONNECTED"))
			{
				esp_mqtt = 1;
			}
	}
}

void MQTT_Tast(void)
{
	 Message a1;
	 if(esp_busy == 0)
		  {
			  if(Queue_Pop(&a1))
				{
					esp_busy = 1;
				if(a1.type == MSG_NOW)
				{
				//	//MQTT_PublishStatusData(a1.data.led, a1.data.fan,  a1.data.buzzer);
					//MQTT_PublishStatusDataLed(a1.data.led);
					//MQTT_PublishStatusDataBuzzer(a1.data.buzzer);
					//MQTT_PublishStatusDataStation(a1.data.station);
					//MQTT_PublishStatusDataFan(a1.data.fan);
				}
				
				else if(a1.type == MSG_LED )
				{
					MQTT_PublishStatusDataLed(a1.data.led);
				}
				
				else if(a1.type == MSG_FAN)
				{
					MQTT_PublishStatusDataFan(a1.data.fan);
				}
				
				else if(a1.type == MSG_STATION)
				{
					MQTT_PublishStatusDataStation(a1.data.station);
				}
				
				else if(a1.type == MSG_BUZZER)
				{
					MQTT_PublishStatusDataBuzzer(a1.data.buzzer);
				}

				else if(a1.type == MSG_TEMP_HUMI)
				{
					 MQTT_PublishSensorData(a1.data.th.smoke , a1.data.th.temp, a1.data.th.humi);
				}
		  }
			}
		  if(mqtt_msg_flag == 1)
		  {
        mqtt_msg_flag = 0;
        ProcessMQTTMessage(copy,&led_last,&fan_last,&buzzer_last);      
		  }	
}

uint8_t SHT30_GetData(void)
{
	static uint32_t lasttime = 0;
	if(millis() - lasttime >= 1000)
	{
		lasttime = millis();
		SHT30_Measure(&temp, &humi);
		return 1;
	}
	return 0;
}

int main(void)
{
	TIM2_DelayInit();
	LED_Init();
	OLED_Init();
  SHT30_Init();
  BtSerial_Init();
	Buzzer_Init();
	MQ2_Init();
	PWM_Init();
	Key_Init();
  MQTT_Init();
	SysTick_Init();
	//LED1_ON();
  //Buzzer_ON();
	ShowMenu();
	OLED_Update();
	while(1)
	{
		MQTT_Publish();
		MQTT_Station();
		MQTT_Tast();
		OLED_Update();
		Keynum = Key_GetNum();
		SHT30 = SHT30_GetData();
		smoke = MQ2_GetPPM();
		if(Keynum || SHT30 || esp_wifi || esp_mqtt)
		{
			Menu_KeyScan(Keynum);
			MenuEnter(&T_temp, &T_humi, &T_rate,&temp, &humi, &smoke,Keynum, SHT30, esp_wifi,esp_mqtt);
			Temp_Menu(&T_temp, &T_humi, &T_rate, Keynum);
			OLED_Update();
		}
		if(station_state == 0)
		{
			if(temp >= T_temp)
			{
				PWM_SetSpeed(T_rate);
				fan_state = 1;
			}
			else if(temp <= T_temp-2)
			{
				PWM_SetSpeed(0);
				fan_state = 0;
			}
			if(temp >= T_temp+2 || humi >= T_humi  || smoke >= 2000)
			{
				Buzzer_ON();//蜂鸣器响
				buzzer_state = 1;
				LED1_ON();
				led1_state = 1;
			}
			else if(temp <= T_temp - 2  && humi <= T_humi && smoke <= 500)
			{
				Buzzer_OFF();//蜂鸣器不响
				buzzer_state = 0;
				LED1_OFF();
				led1_state = 0;
			}
			
		}

			if(station_state == 1)
			{
				if(fan_state==1)
				{
					PWM_SetSpeed(T_rate);
				}
				else
				{
					PWM_SetSpeed(0);
				}
				if(buzzer_state == 1)
				{
					Buzzer_ON();//蜂鸣器响
				}
				else
				{
					Buzzer_OFF();
				}
				if(led1_state == 1)
				{
					LED1_ON();
				}
				else
				{
					LED1_OFF();
				}
			}
		}
		
	} 
       
