#ifndef _LCD_MENU_
#define _LCD_MENU_

#include<stdio.h>
#include<stdlib.h>
#include"main.h"
#include"i2c-lcd.h"
#include<string.h>


typedef enum
{
	Down,
	Up,
	NoChange
}Pointer_state;

typedef enum							//LCD_OPTION_PAGE_1
{
	Page_0,
	Page_1,
	Page1_SetPoint,
	Page1_Testing,
	Page1_Calbration_sensor,
	Page1_WifiConfig,
	Page1_Back,
	Page1_Nothing
}Page1;

typedef enum							//LCD_OPTION_PAGE_2
{
	Page2_ph,
	Page2_ph_thr,
	Page2_tds,
	Page2_tds_thr,
	Page2_pump_1,
	Page2_pump_2,
	Page2_pump_3,
	Page2_pump_4,
	Page2_calib_ph,
	Page2_calib_tds,
	Page2_start,
	Page2_end,
	Page2_Back,
	Page2_Nothing
}Page2;
typedef enum
{
	Sph,
	Sph_phr,
	Stds,
	Stds_phr,
	none,

}Value_Setype;

void Rotary_init();
int  Rotary_volum();
void Push_Slect();
void Pointer_Status(uint32_t volume);
void Pointer_2_Status(uint32_t line);
void LCD_Menu_2_1();
void LCD_Menu_2_2();
void LCD_Menu_2_3(uint8_t isCalib);
void LCD_Menu_2_4();
void LCD_Menu_1();
void LCD_Normal_Mode();
void LCD_Display();

#endif
