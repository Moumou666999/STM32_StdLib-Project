#ifndef __MQ2_H
#define __MQ2_H
#include "stm32f10x.h"                  // Device header
#define MQ2_READTIMES   10
void MQ2_Init(void);
uint16_t MQ2_AD_Read(void);
uint16_t MQ2_GetData(void);

float MQ2_GetPPM(void);
#endif
