#ifndef  __QUEUE_H
#define  __QUEUE_H
#include "stm32f10x.h"                  // Device header

#define QUEUE_SIZE   128

typedef enum
{
	MSG_TEMP_HUMI,
	MSG_LED,
	MSG_STATION,
	MSG_FAN,
	MSG_BUZZER,
	MSG_NOW
}MSGType;

typedef struct
{
	MSGType type;
	
	union
	{
		struct
		{
			float temp;
			float humi;
			float smoke;
		}th;
		
		uint8_t led;
		uint8_t station;
		uint8_t fan;
		uint8_t buzzer;
	}data;
	
}Message;

void Queue_Init(void);

uint8_t Queue_Push(Message *msg);
uint8_t Queue_Pop(Message *msg);
#endif
