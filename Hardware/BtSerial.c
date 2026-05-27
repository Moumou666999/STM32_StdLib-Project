#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>
#include "Delay.h"

#define RX_BUF_SIZE 256            //环形缓冲区最大值
static uint8_t rx_buf[RX_BUF_SIZE];//接收缓冲区
static uint16_t rx_head = 0;       //存指针
static uint16_t rx_tail = 0;       //取指针

char Serial_RxPacket[100];         //接收格式为@......\r\n的数据存放
uint8_t Serial_RxFalg = 0;	       //标志位

static void BtSerial_PutChar(uint8_t data) 
{
	uint16_t next = (rx_head + 1) % RX_BUF_SIZE;
	if(next != rx_tail)
	{
		rx_buf[rx_head] = data;
		rx_head = next;
	}
	else
	{
		
	}
}

static uint8_t BtSerial_GetChar(uint8_t *data)
{
	if(rx_head != rx_tail)
	{
		*data = rx_buf[rx_tail];
		rx_tail = (rx_tail + 1) % RX_BUF_SIZE;
		return 1;
	}
	return 0;
}

void BtSerial_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	
	USART_Init(USART1,&USART_InitStructure);
	
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	
	NVIC_PriorityGroupConfig(2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART1,ENABLE);
}

void BtSerial_SendByte(uint8_t Byte)
{
	USART_SendData(USART1,Byte);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
}


void BtSerial_SendArray(uint8_t *Array,uint16_t length)
{
	uint16_t i;
	for(i = 0;i < length;i++)
	{
		BtSerial_SendByte(Array[i]);
	}
}

int fputc(int ch,FILE *f)//pfintf重写
{
	BtSerial_SendByte(ch);
	return ch;
}

void BtSerial_SendString(const char *String)//发送字符串
{
	uint8_t i;
	for(i = 0;String[i]!='\0';i++)
	{
		BtSerial_SendByte(String[i]);
	}
}

void Serial_printf(char *format,...)
{
	char String[100];
	va_list arg;
	vsprintf(String,format,arg);
	va_end(arg);
	BtSerial_SendString(String);//改这个串口就可以在不同串口用这个,函数名记得改；
}

uint8_t BtSerial_ReadLine(char *line,uint16_t max_len,uint32_t time_out)
{
	uint16_t idx = 0;
	uint32_t elapsed = 0;
	while(elapsed < time_out)
	{
		uint8_t data;
		if(BtSerial_GetChar(&data))
		{
			if(data == '\r')
			{
				continue;
			}
			else if(data == '\n')
			{if(idx == 0)
				{
					continue;
				}
				line[idx] = '\0';
				return 1;
			}
			else
			{
				if(idx < max_len - 1)
				{
					line[idx++] = data;
				}
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

void USART1_IRQHandler(void)
{
	static uint8_t RxState = 0;
	static uint8_t pRxState = 0;
	
	if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET)
	{
		uint8_t data = USART_ReceiveData(USART1);
		
		BtSerial_PutChar(data);
		
		if(RxState == 0)
		{
			if(data == '@' && Serial_RxFalg == 0)
			{
				RxState = 1;
				pRxState = 0;
			}
		}
		else if(RxState == 1)
		{
			if(data == '\r')
			{
				RxState = 2;
			}
			else
			{
				Serial_RxPacket[pRxState] = data;
			  pRxState++;
			}
		}
		else if(RxState == 2)
		{
			if(data == '\n')
			{
				RxState = 0;
				Serial_RxPacket[pRxState] = '\0';
				Serial_RxFalg = 1;		
			}
		}
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	}
}

void BtSerial_Clear(void)
{
	rx_head = 0;
	rx_tail = 0;
}

