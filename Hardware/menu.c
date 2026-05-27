#include "stm32f10x.h"                  // Device header
#include "menu.h"
#include "Key.h"
#include "OLED.h"

MenuMode_t Menu_Mode = MODE_EDIT;//数据改变
Menu_t Menu_Select = PAGE_TEMP; //主菜单选项
Menu_t Current_Page = PAGE_TEMP; //功能详情选项

uint8_t static page = 0;//功能详情界面标志位
uint8_t static hang = 1;

void Menu_KeyScan(uint8_t KeyNum)
{
	if(KeyNum == 1 && page == 0)
	{
		Menu_Select++;
		Current_Page++;
		if(Menu_Select > PAGE_SETTING)
		{
			Menu_Select = PAGE_TEMP;
			Current_Page = PAGE_TEMP;
		}
		ShowMenu();
	}
	
	if(KeyNum == 2 && page == 0)
	{
		if(Menu_Select == 0)
		{
			Menu_Select = PAGE_SETTING;
			Current_Page = PAGE_SETTING;
		}
		else
		{
			Menu_Select--;
			Current_Page--;
		}
		ShowMenu();
	}
	
}

void MenuEnter(uint8_t *T_temp, uint8_t *T_humi, int16_t *T_rate, uint8_t *temp, uint8_t *humi ,float *smoke,uint8_t KeyNum, uint8_t SHT30, uint8_t wifi, uint8_t mqtt)
{
	if((KeyNum == 3 && page == 0) || (SHT30 && page == 1 && Current_Page == PAGE_TEMP )|| (mqtt && page == 1 && Current_Page == PAGE_MQTT) || (wifi && page == 1 && Current_Page == PAGE_WIFI))  
	{
	if(page == 0)
	{
		OLED_Clear();
	}
	switch(Current_Page)
	{	
		case PAGE_TEMP:
		{
			OLED_ShowString(0,0,"Temp:",OLED_8X16);
			OLED_ShowString(0,20,"Humi:",OLED_8X16);
			OLED_ShowString(0,40,"P:", OLED_8X16);
			OLED_ShowFloatNum(15,45,*smoke,4,2,OLED_6X8);
			OLED_ShowNum(40,0,*temp,2,OLED_8X16);
			OLED_ShowNum(40,20,*humi,2,OLED_8X16);
			if(page == 0)
			{
			OLED_ShowString(80,0,">T:",OLED_8X16);
			OLED_ShowString(80,20,"H :",OLED_8X16);
			OLED_ShowString(80,40,"V :",OLED_8X16);
			}
			OLED_ShowNum(110,0,*T_temp,2,OLED_8X16);
			OLED_ShowNum(110,20,*T_humi,2,OLED_8X16);
			 OLED_ShowSignedNum(110,45,*T_rate,2,OLED_6X8);
		}
		break;
		case PAGE_WIFI:
		{
			if(wifi == 1)
			{
				OLED_ShowString(0,30,"WIFI online", OLED_8X16);
			}
			else
			{
				OLED_ShowString(0,30,"WIFI lost", OLED_8X16);
			}
		}
		break;
		case PAGE_MQTT:
		{
			if(mqtt == 1)
			{
				OLED_ShowString(0,30,"MQTT online", OLED_8X16);
			}
			else
			{
				OLED_ShowString(0,30,"MQTT lost", OLED_8X16);
			}
		}
		break;
		case PAGE_INF0:
		{
			OLED_ShowNum(0,30,3,1,OLED_6X8);
		}
		break;
		case PAGE_SETTING:
		{
			OLED_ShowNum(0,40,4,1,OLED_6X8);
		}
		break;
	}
		page = 1;
}
	if(KeyNum == 4 && page == 1)
	{
		OLED_Clear();
		page = 0;
		ShowMenu();
	}
}

void Temp_Menu(uint8_t *T_temp, uint8_t *T_humi,int16_t *T_rate, uint8_t KeyNum)
{
	if(page == 1 && Current_Page == PAGE_TEMP)
	{
		if(Menu_Mode == MODE_VIEW)
		{
		if(KeyNum == 1)
		{
			hang++;
//			if(hang == 1)
//			{
//			  OLED_ShowString(70,0,">T:",OLED_8X16);
//			  OLED_ShowString(70,20,"H :",OLED_8X16);
//				OLED_ShowString(70,40,"V :",OLED_8X16);
//			}
			if(hang == 2)
			{
				OLED_ShowString(80,0,"T :",OLED_8X16);
			  OLED_ShowString(80,20,">H:",OLED_8X16);
				OLED_ShowString(80,40,"V :",OLED_8X16);
			}
			else if(hang == 3)
			{
			  OLED_ShowString(80,0,"T :",OLED_8X16);
			  OLED_ShowString(80,20,"H :",OLED_8X16);
				OLED_ShowString(80,40,">V:",OLED_8X16);
			}
			else if(hang == 4)
			{
				hang = 1;
				OLED_ShowString(80,0,">T:",OLED_8X16);
			  OLED_ShowString(80,20,"H :",OLED_8X16);
				OLED_ShowString(80,40,"V :",OLED_8X16);
			}
		}
		if(KeyNum == 2)
		{
			hang--;
			if(hang == 1)
			{
			  OLED_ShowString(80,0,">T:",OLED_8X16);
			  OLED_ShowString(80,20,"H :",OLED_8X16);
				OLED_ShowString(80,40,"V :",OLED_8X16);
			}
			else if(hang == 2)
			{
				OLED_ShowString(80,0,"T :",OLED_8X16);
			  OLED_ShowString(80,20,">H:",OLED_8X16);
				OLED_ShowString(80,40,"V :",OLED_8X16);
			}
//			else if(hang == 3)
//			{
//			  OLED_ShowString(70,0,"T :",OLED_8X16);
//			  OLED_ShowString(70,20,"H :",OLED_8X16);
//				OLED_ShowString(70,40,">V:",OLED_8X16);			
//			}
			else if(hang == 0)
			{
				hang = 3;
				OLED_ShowString(80,0,"T :",OLED_8X16);
			  OLED_ShowString(80,20,"H :",OLED_8X16);
				OLED_ShowString(80,40,">V:",OLED_8X16);

			}
		}
	}
		if(Menu_Mode == MODE_EDIT)
		{
			if(KeyNum == 1)
			{
				if(hang == 1)
				{
					(*T_temp)++;
					OLED_ShowNum(110,0,*T_temp,2,OLED_8X16);
				}
				if(hang == 2)
				{
					(*T_humi)++;
				  OLED_ShowNum(110,20,*T_humi,2,OLED_8X16);
				}
				if(hang == 3)
				{
					(*T_rate)+=10;
					if((*T_rate)> 60)
					{
						(*T_rate) = -60;
					}
					  OLED_ShowSignedNum(110,45,*T_rate,2,OLED_6X8);
				}
			}
			if(KeyNum == 2)
			{
				if(hang == 1)
				{
					(*T_temp)--;
					OLED_ShowNum(110,0,*T_temp,2,OLED_8X16);
				}
				if(hang == 2)
				{
					(*T_humi)--;
				  OLED_ShowNum(110,20,*T_humi,2,OLED_8X16);
				}
				if(hang == 3)
				{
					(*T_rate)-=10;
					if((*T_rate)< -60)
					{
						(*T_rate) = 60;
					}
					 OLED_ShowSignedNum(110,45,*T_rate,2,OLED_6X8);
				}
			}
		}
		
		if(KeyNum == 3)
		{
			if(Menu_Mode == MODE_VIEW)
			{
				Menu_Mode = MODE_EDIT;
				OLED_ShowString(0,60,"**",OLED_6X8);
			}
			else
			{
				Menu_Mode = MODE_VIEW;
				OLED_ShowString(0,60,">>",OLED_6X8);
			}
		}
		
		if(KeyNum == 4)
		{
			Menu_Mode = MODE_VIEW;
			OLED_ShowString(0,0,"Temp:",OLED_8X16);
			OLED_ShowString(0,20,"Humi:",OLED_8X16);
		  OLED_ShowString(70,0,"T:",OLED_8X16);
			OLED_ShowString(70,20,"H:",OLED_8X16);
		}
	}
}

void ShowMenu(void)
{
	OLED_ShowString(0,0,Menu_Select == PAGE_TEMP ? ">TEMP" : " TEMP",OLED_8X16);
	OLED_ShowString(0,20,Menu_Select == PAGE_WIFI ? ">WIFI" : " WIFI", OLED_8X16);
	OLED_ShowString(0,40,Menu_Select == PAGE_MQTT ? ">MQTT" : " MQTT", OLED_8X16);
	OLED_ShowString(50,0,Menu_Select == PAGE_INF0 ? ">INF0" : " INF0", OLED_8X16);
	OLED_ShowString(50,20,Menu_Select == PAGE_SETTING ? ">SET" : " SET", OLED_8X16);
}

