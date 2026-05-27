#include "stm32f10x.h"                  // Device header
#include "queue.h"

static Message queue[QUEUE_SIZE];
static uint8_t head = 0;
static uint8_t tail = 0;

void Queue_Init(void)
{
	head = 0;
	tail = 0;
}

uint8_t Queue_Push(Message *msg)
{
	uint8_t next_tail;
	next_tail = (tail + 1)%QUEUE_SIZE;
	
	if(next_tail != head)
	{
		queue[tail] = *msg;
	  tail = next_tail;
		return 1;
	}
	else
	{
		return 0;
	}
}

uint8_t Queue_Pop(Message *msg)
{
	if(head != tail)
	{
		*msg = queue[head];
	  head = (head + 1)%QUEUE_SIZE;
		return 1;
	}
	else
	{
		return 0;
	}
}


