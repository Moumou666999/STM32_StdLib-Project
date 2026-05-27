#ifndef __BTSERIAL_H
#define __BTSERIAL_H
#include <stdio.h>
#include "stdint.h"

extern char Serial_RxPacket[];
extern uint8_t Serial_RxFalg;	

void BtSerial_Init(void);

void BtSerial_SendByte(uint8_t Byte);

void BtSerial_SendArray(uint8_t *Array,uint16_t length);

void BtSerial_SendString(const char *String);

void Serial_printf(char *format,...);

void BtSerial_Clear(void);

uint8_t BtSerial_ReadLine(char *line,uint16_t max_len,uint32_t time_out);

static uint8_t BtSerial_GetChar(uint8_t *data);

static void BtSerial_PutChar(uint8_t data);
#endif

