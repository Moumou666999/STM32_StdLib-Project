#include "stm32f10x.h"                  // Device header
#ifndef __MENU_H
#define __MENU_H

typedef enum
{
	PAGE_TEMP = 0,
	PAGE_WIFI,
	PAGE_MQTT,
	PAGE_INF0,
	PAGE_SETTING,
}Menu_t;

typedef enum
{
	MODE_VIEW = 0,//浏览模式
	MODE_EDIT,//编辑模式
}MenuMode_t;

extern uint8_t hang;
extern uint8_t page;
extern Menu_t Menu_Select;
extern Menu_t Current_Page;
void Menu_KeyScan(uint8_t KeyNum);

void ShowMenu(void);
void MenuEnter(uint8_t *T_temp, uint8_t *T_humi,int16_t *T_rate, uint8_t *temp, uint8_t *humi ,float *smoke,uint8_t KeyNum, uint8_t SHT30 ,uint8_t wifi, uint8_t mqtt);
void Temp_Menu(uint8_t *temp, uint8_t *humi,int16_t *rate, uint8_t KeyNum);
#endif
