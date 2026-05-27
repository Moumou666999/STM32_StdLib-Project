#include "stm32f10x.h"                  // Device header
#include "Delay.h"

void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

uint8_t Key_Down(void)
{
	static uint8_t Key = 1;
	if(Key && GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 1)
	{
		Delay_ms(10);
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 1)
		{
			Key = 0;
			return 1;
		}
	}
		else
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) == 0)
		{
			Key = 1;
		}
		return 0;
}

uint8_t Key_Up(void)
{
	static uint8_t Key = 1;
	if(Key && GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 1)
	{
		Delay_ms(10);
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 1)
		{
			Key = 0;
			return 1;
		}
	}
		else
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 0)
		{
			Key = 1;
		}
		return 0;
}

uint8_t Key_OK(void)
{
	static uint8_t Key = 1;
	if(Key && GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 1)
	{
		Delay_ms(10);
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 1)
		{
			Key = 0;
			return 1;
		}
	}
		else
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0)
		{
			Key = 1;
		}
		return 0;
}

uint8_t Key_Back(void)
{
	static uint8_t Key = 1;
	if(Key && GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == 1)
	{
		Delay_ms(10);
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == 1)
		{
			Key = 0;
			return 1;
		}
	}
		else
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == 0)
		{
			Key = 1;
		}
		return 0;
}

uint8_t Key_GetNum(void)
{
	if(Key_Down())
	{
		return 1;
	}
	if(Key_Back())
	{
		return 4;
	}
	if(Key_OK())
	{
		return 3;
	}
	if(Key_Up())
	{
		return 2;
	}
	return 0;
}
