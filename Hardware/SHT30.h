#ifndef __SHT30
#define __SHT30
#include <stdint.h>

void SHT30_W_SDA(uint8_t BitValue);
void SHT30_W_SCL(uint8_t BitValue);

uint8_t SHT30_R_SDA(void);
void SHT30_Init(void);

void SHT30_Start(void);
void SHT30_Stop(void);

void SHT30_SendByte(uint8_t Byte);

uint8_t SHT30_ReceiveByte(void);
void SHT30_SendAck(uint8_t Byte);
uint8_t SHT30_ReceiveAck(void);

void SHT30_SendCommand(uint8_t addr, uint8_t cmd);

uint8_t SHT30_ReceiveData(uint8_t *Data);

uint8_t SHT30_CRC8(const uint8_t *Data, uint8_t len);

uint8_t SHT30_Measure(uint8_t *temp, uint8_t *humi);

uint8_t aaa(void);
#endif
