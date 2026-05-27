#ifndef __ESP8266_H
#define __ESP8266_H
#include "stdint.h"
void ESP8266_SendSting(const char *String);

uint8_t ESP8266_SendWait(const char *cmd,const char *ack,uint32_t timeout_ms);

uint8_t ESP8266_WaitFor(const char *ack,uint32_t timeout_ms);

uint8_t ESP8266_DisableEcho(uint32_t timeout_ms);

uint8_t ESP8266_SendCmdOK(const char *cmd,uint32_t timeout_ms);
uint8_t ESP8266_Init(void);
uint8_t ESP8266_ConnectWIFI(char *ssid, char *pwd);
uint8_t ESP8266_ConnectServer(char *ip,uint16_t port);

uint16_t ESP8266_SendData(uint8_t *data, uint16_t len);
uint8_t ESP8266_CloseConnection(void);

#endif
