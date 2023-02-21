/*
 * lcd_menu.c
 *
 *  Created on: Feb 20, 2023
 *      Author: Xuan Truong
 */
#include<lcd_menu.h>

extern TIM_HandleTypeDef htim4;
extern IWDG_HandleTypeDef hiwdg;

uint32_t last = 0, pointer_position = 0,Rpush_number = 0,button_flag = 0,Page = 0;
uint32_t lcd_pointer_1 = 0;
uint8_t count = 0;
uint8_t isPress = 0;
uint32_t rotary_first_value = 0, rotary_curent_value = 0;

extern float TDS,TDS_THR,PH,PH_THR,Temperature;
extern float TDS_SetPoint,TDS_THR_SetPoint,PH_THR_SetPoint,PH_SetPoint;
Page1 option_page_1 = Page1_Nothing;
Page2 option_page_2 = Page2_Nothing;

extern void Read_SetPoint(Save_Flash_Type tp);
extern void Save_SetPoint();

void Rotary_init()
{
	rotary_first_value = HAL_GPIO_ReadPin(GPIOE, Rotary_CLK_Pin);
}
int  Rotary_volum()
{
	rotary_curent_value = HAL_GPIO_ReadPin(GPIOE, Rotary_CLK_Pin);
	if (rotary_curent_value != rotary_first_value)
	   {
	     if (HAL_GPIO_ReadPin(GPIOE, Rotary_DT_Pin) != rotary_curent_value)
	     {
	    	 lcd_clear();
	    	 lcd_pointer_1 +=1;

	     }
	     else
	     {
	    	 lcd_clear();
	    	 lcd_pointer_1 -=1;
	     }
	   }
	rotary_first_value = rotary_curent_value;
	return lcd_pointer_1;
}
void Push_Slect()
{
	  if(HAL_GPIO_ReadPin(GPIOE, Rotary_SW_Pin) == 0)
	  		{
	  			HAL_Delay(20);
	  			if((HAL_GPIO_ReadPin(GPIOE, Rotary_SW_Pin) == 0) && (isPress == 0)) // nut nhan da bam
	  			{
	  				lcd_clear();
	  				Rpush_number++;
	  				isPress = 1;
	  				button_flag = 0;
	  			}
	  		}
	  		else{isPress = 0;}
}
void Pointer_Status(uint32_t volume)
{
	if(volume > last)
	{
		pointer_position++;
	}
	else if(volume < last)
	{
		pointer_position--;
	}
	last = volume;

	switch(pointer_position)
	{
		case 0:
			lcd_send_cmd(0x80 | 0x00); //PH
			lcd_send_string("->");
			break;
		case 2:
			lcd_send_cmd(0x80 | 0x40); //PH
			lcd_send_string("->");
			break;
		case 4:
			lcd_send_cmd(0x80 | 0x14); //PH
			lcd_send_string("->");
			break;
		case 6:
			lcd_send_cmd(0x80 | 0x54); //PH
			lcd_send_string("->");
			break;
		case 8:
			if(Rpush_number == 2||3)
			{
				lcd_send_cmd(0x80 | 0x66);
				lcd_send_string(">>");
			}
			break;

		default:
			if(pointer_position>9)
			{
				pointer_position = 0;
				lcd_send_cmd(0x80 | 0x00); //PH
				lcd_send_string("->");
			}
			break;
	}
}
void Pointer_2_Status(uint32_t line)
{
	if(line == 0)
		{
		lcd_send_cmd(0x80 | 0x0A); //PH
		lcd_send_string("<");
		}
		else if(line == 2)
		{
		lcd_send_cmd(0x80 | 0x4A); //PH
		lcd_send_string("<");
		}

		else if(line == 4)
		{
		lcd_send_cmd(0x80 | 0x1E); //PH
		lcd_send_string("<");
		}

		else if(line == 6)
		{
		lcd_send_cmd(0x80 | 0x5E); //PH
		lcd_send_string("<");
		}
}
void LCD_Menu_2_1()
{
	char buffer_string[100];
	lcd_send_cmd(0x80 | 0x02); //PH
	lcd_send_string("PH:");
	lcd_send_cmd(0x80 | 0x42); //PH
	lcd_send_string("PH_Thr:");
	lcd_send_cmd(0x80 | 0x16); //PH
	lcd_send_string("TDS:");
	lcd_send_cmd(0x80 | 0x56); //PH
	lcd_send_string("TDS_Thr:");


	lcd_send_cmd(0x80 | 0x0B); //PH_Setpoint
	sprintf(buffer_string,"%.2f",PH_SetPoint);
	lcd_send_string(buffer_string);
	memset(buffer_string,0,strlen(buffer_string));


	lcd_send_cmd(0x80 | 0x4B); //PH_THR_Setpoint
	sprintf(buffer_string,"%.2f",PH_THR_SetPoint);
	lcd_send_string(buffer_string);
	memset(buffer_string,0,strlen(buffer_string));

	lcd_send_cmd(0x80 | 0x1F); //TDS_Setpoint
	sprintf(buffer_string,"%.2f",TDS_SetPoint);
	lcd_send_string(buffer_string);
	memset(buffer_string,0,strlen(buffer_string));


	lcd_send_cmd(0x80 | 0x5F); //TDS_THR_Setpoint
	sprintf(buffer_string,"%.2f",TDS_THR_SetPoint);
	lcd_send_string(buffer_string);
	memset(buffer_string,0,strlen(buffer_string));
}
void LCD_Menu_2_2()
{
	lcd_send_cmd(0x80 | 0x02); //PH
	lcd_send_string("Pump 1:");
	lcd_send_cmd(0x80 | 0x42); //PH
	lcd_send_string("Pump 2:");
	lcd_send_cmd(0x80 | 0x16); //PH
	lcd_send_string("Pump 3:");
	lcd_send_cmd(0x80 | 0x56); //PH
	lcd_send_string("Pump 4:");
}
void LCD_Menu_2_3()
{
	lcd_send_cmd(0x80 | 0x02); //PH
	lcd_send_string("PH:");
	lcd_send_cmd(0x80 | 0x42); //PH
	lcd_send_string("TDS:");
	lcd_send_cmd(0x80 | 0x16); //PH
}
void LCD_Menu_2_4()
{
	lcd_send_cmd(0x80 | 0x02); //PH
	lcd_send_string("Go to the link:");
	lcd_send_cmd(0x80 | 0x42); //PH
	lcd_send_string("setupwifi.com.vn");
	lcd_send_cmd(0x80 | 0x16); //PH
	lcd_send_string("Start");
	lcd_send_cmd(0x80 | 0x56); //PH
	lcd_send_string("End");
}
void LCD_Menu_1()
{
	lcd_send_cmd(0x80 | 0x02);
		lcd_send_string("Set point");
		lcd_send_cmd(0x80 | 0x42);
		lcd_send_string("Testing");
		lcd_send_cmd(0x80 | 0x16);
		lcd_send_string("Calibration sensor");
		lcd_send_cmd(0x80 | 0x56);
		lcd_send_string("Wifi Config");
}
void LCD_Normal_Mode()
{
	char String[100];
	sprintf(String,"PH  :%.2f   ",PH);
	lcd_send_cmd(0x80 | 0x03); //PH
	lcd_send_string(String) ;
	memset(String,0,strlen(String));

	sprintf(String,"TDS :%.0f   ",TDS);
	lcd_send_cmd(0x80 | 0x43); //PH
	lcd_send_string(String);
	memset(String,0,strlen(String));

	sprintf(String,"TEMP:%.2f   ",Temperature);
	lcd_send_cmd(0x80 | 0x17); //PH
	lcd_send_string(String);
	memset(String,0,strlen(String));
	lcd_send_cmd(0x80 | 0x54); //PH
	lcd_send_string("PRESS TO CONFIG MODE");
}
void LCD_Display()
{
	float tamp  = 0;
	float tamp2 = 0;
	{
	Push_Slect();
	if(Rpush_number == 0)
	{
		Page = 0;
	}
	else if(Rpush_number == 1)
	{
		Page = 1;
	}
	else if(Rpush_number == 2)
	{
		if(button_flag ==0)
		{
			switch(pointer_position)
		  {
			case 0:
				option_page_1 = Page1_SetPoint;
				Page = 2;
				break;
			case 2:
				option_page_1 = Page1_Testing;
				Page = 2;
				break;
			case 4:
				option_page_1 = Page1_Calbration_sensor;
				Page = 2;
				break;
			case 6:
				option_page_1 = Page1_WifiConfig;
				Page = 2;
				break;
			case 8:
				option_page_1 = Page1_Back;
				Page = 0;
				Rpush_number = 0;
				break;
			default:
				break;
			}
			button_flag =1;
		}
	}
	else if(Rpush_number == 3)
	{
			// OPTION SETPOINT AT PAGE 1
			if(option_page_1 == Page1_SetPoint)
			{
				if(pointer_position == 0)
				{
					option_page_2 = Page2_ph;
				}
				else if(pointer_position == 2)
				{
					option_page_2 = Page2_ph_thr;
				}
				else if(pointer_position == 4)
				{
					option_page_2 = Page2_tds;
				}
				else if(pointer_position == 6)
				{
					option_page_2 = Page2_tds_thr;
				}
				else if(pointer_position == 8)
				{
					option_page_2 = Page2_Back;
				}

			}
			//// OPTION TESTING AT PAGE 1
			else if (option_page_1 ==Page1_Testing)
			{
				if(pointer_position == 0)
				{
					option_page_2 = Page2_pump_1;
				}
				else if(pointer_position == 2)
				{
					option_page_2 = Page2_pump_2;
				}
				else if(pointer_position == 4)
				{
					option_page_2 = Page2_pump_3;
				}
				else if(pointer_position == 6)
				{
					option_page_2 = Page2_pump_4;
				}
				else if(pointer_position == 8)
				{
					option_page_2 = Page2_Back;
				}

			}
			///// OPTION CALIB AT PAGE 1
			else if(option_page_1 == Page1_Calbration_sensor)
			{

				if(pointer_position == 0)
				{
					option_page_2 = Page2_calib_ph;
				}
				else if(pointer_position == 2)
				{
					option_page_2 = Page2_calib_tds;
				}
				else if(pointer_position == 8)
				{
					option_page_2 = Page2_Back;
				}

			}
			///OPTION WIFICONFIG PAGE 1
			else if(option_page_1 == Page1_WifiConfig)
			{
				if(pointer_position == 4)
				{
					option_page_2 = Page2_start;
				}
				else if(pointer_position == 6)
				{
					option_page_2 = Page2_end;
				}
				else if(pointer_position == 8)
				{
					option_page_2 = Page2_Back;
				}
			}
		}
		else if(Rpush_number == 4)  // Only for Set up value
		{
			if(option_page_1 == Page1_SetPoint)
			{
				option_page_2 = Page2_Nothing;
				Rpush_number = 2;
				button_flag = 1;
				pointer_position = pointer_position + 1;
				lcd_clear();
			}
		}
	}
	switch(Page)
	{
		case 0:									//Page 0 display
			LCD_Normal_Mode();
			break;
		case 1: 								//Page 1 display
			Pointer_Status(Rotary_volum());
			LCD_Menu_1(Rotary_volum());
			break;
		case 2: 								//Page 2 display
			Pointer_Status(Rotary_volum());
			//DISPLAY WITH OPTION SETPOINT
/*case 2 SETPOINT:*/
			if(option_page_1 == Page1_SetPoint)
			{
				  Read_SetPoint(flash_setpoint);
				  LCD_Menu_2_1();
				  if(option_page_2 == Page2_ph)
				  {
					  lcd_pointer_1 = PH_SetPoint;
					  tamp = PH_SetPoint;
					  HAL_TIM_Base_Stop_IT(&htim4);
						while(Rpush_number == 3) 		// Setting setpoint for PH
							{
											LCD_Menu_2_1();
											Push_Slect();
											Pointer_2_Status(0);
											tamp2= (float)((Rotary_volum()- tamp)/10);
											PH_SetPoint = tamp2 + tamp;
											HAL_IWDG_Refresh(&hiwdg);
							}

				  }
				  else if(option_page_2== Page2_ph_thr) // Setting setpoint for PH_THR
				  {
					  lcd_pointer_1 = PH_THR_SetPoint;
				  	  tamp = PH_THR_SetPoint;
					  HAL_TIM_Base_Stop_IT(&htim4);
						while(Rpush_number== 3)
							{
											LCD_Menu_2_1();
											Push_Slect();
											Pointer_2_Status(2);
											tamp2 = (float)((Rotary_volum()- tamp)/10);
											PH_THR_SetPoint =  tamp2 + tamp;
											HAL_IWDG_Refresh(&hiwdg);
							}

				  }
				  else if(option_page_2==Page2_tds)		// Setting setpoint for TDS
				  {

					  lcd_pointer_1 = TDS_SetPoint;
					  tamp = TDS_SetPoint;
					  HAL_TIM_Base_Stop_IT(&htim4);
						while(Rpush_number== 3)
							{
											LCD_Menu_2_1();
											Push_Slect();
											Pointer_2_Status(4);
											tamp2 = ((Rotary_volum()- tamp)*10);
											TDS_SetPoint =  (int)tamp2 + (int)tamp;
											HAL_IWDG_Refresh(&hiwdg);
							}

				  }
				  else if(option_page_2==Page2_tds_thr)	// Setting setpoint for TDS_thr
				  {
					  	HAL_TIM_Base_Stop_IT(&htim4);
					  	tamp = TDS_THR_SetPoint;
					    lcd_pointer_1 = TDS_THR_SetPoint;
						while(Rpush_number == 3)
						{
											LCD_Menu_2_1();
											Push_Slect();
											Pointer_2_Status(6);
											tamp2 = ((Rotary_volum()- tamp)*10);
											TDS_THR_SetPoint = (int)tamp2 + (int)tamp;
											HAL_IWDG_Refresh(&hiwdg);
						}
				  }
				  else if(option_page_2 == Page2_Back)	//Back option
				  {
					  Page = 1;
					  Rpush_number = 1;
					  button_flag = 0;
					  option_page_2 = Page2_Nothing;
					  lcd_clear();
				  }

				  if(Rpush_number == 4)			// Out of set value mode
				  {
					  Save_SetPoint();
					  HAL_TIM_Base_Start_IT(&htim4);
				  }
			}
/*case 2 TESTING:*/
			else if(option_page_1 == Page1_Testing) // Display with option testing
			{
				 LCD_Menu_2_2();
				  if(option_page_2 == Page2_pump_1)
				  {

				  }
				  else if(option_page_2== Page2_pump_2)
				  {

				  }
				  else if(option_page_2==Page2_pump_3)
				  {

				  }
				  else if(option_page_2==Page2_pump_4)
				  {

				  }
				  else if(option_page_2 == Page2_Back)
				  {
					  Page = 1;
					  Rpush_number = 1;
					  button_flag = 0;
					  option_page_2 = Page2_Nothing;
					  lcd_clear();
				  }
			}
/*case 2 CALIBRATION:*/
			else if(option_page_1 == Page1_Calbration_sensor) // Display with calibration
			{
					LCD_Menu_2_3();
					if(option_page_2 == Page2_calib_ph)
					{
						HAL_TIM_Base_Stop_IT(&htim4);
						while(Rpush_number == 3)
						{
											LCD_Menu_2_3();
											Push_Slect();
											Pointer_2_Status(0);
											lcd_send_cmd(0x80|0x16);
											lcd_send_string("Please put sensor into PH7 or PH4");
											HAL_IWDG_Refresh(&hiwdg);
						}
					}
					else if(option_page_2 == Page2_calib_tds)
					{

					}
					else if(option_page_2 == Page2_Back)
					{
					  Page = 1;
					  Rpush_number = 1;
					  button_flag = 0;
					  option_page_2 = Page2_Nothing;
					  lcd_clear();
					}
			}
/*case 2 WIFI_CONFIG:*/
			else if(option_page_1 == Page1_WifiConfig)
			{
				LCD_Menu_2_4();
				if(option_page_2 == Page2_start)
				{

				}
				else if(option_page_2 == Page2_end)
				{

				}
				else if(option_page_2 == Page2_Back)
				{
				  Page = 1;
				  Rpush_number = 1;
				  button_flag = 0;
				  option_page_2 = Page2_Nothing;
				  lcd_clear();
				}
			}
			break;

		default:
			break;
	}
 }

