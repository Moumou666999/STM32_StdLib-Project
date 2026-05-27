#ifndef __MQTT_H
#define __MQTT_H
#include "stdint.h"
uint8_t WaitFor(const char *ack, uint32_t timeout_ms);

void MQTT_PublishSensorData(float smoke, int temp, int humi);
void MQTT_Init(void);
void ProcessMQTTMessage(char *line,uint8_t *led1_last, uint8_t *fan_last , uint8_t *buzzer_last);
void MQTT_PublishStatusData(uint8_t led1, uint8_t fan, uint8_t buzzer);
void MQTT_PublishStatusDataLed(uint8_t led1);

void MQTT_PublishStatusDataFan(uint8_t fan);
void MQTT_PublishStatusDataStation(uint8_t Station);

void MQTT_PublishStatusDataBuzzer(uint8_t buzzer);
#endif
