#ifndef __PWM_H
#define __PWM_H
#include "stm32f10x.h" 

void PWM_Init(void);

void PWM_SetCompare3(uint16_t Compare);

void PWM_SetSpeed(int8_t Speed);

#endif
