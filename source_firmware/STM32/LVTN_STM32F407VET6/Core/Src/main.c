/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "sd_card.h"
#include "common.h"
#include "i2c-lcd.h"
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_SPI1_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/*=====================================COMMON=================================*/
#define SIZEOF_COMMAND 60
#define UART1_BUFFER_SIZE 1024

uint8_t count;
uint8_t isPress;
uint32_t rotary_first_value = 0, rotary_curent_value = 0;
uint32_t lcd_pointer = 0;;


float TDS = 500,TDS_THR= 0,PH = 120,PH_THR = 0,Temp = 35.5;
float TDS_SetPoint = 1,TDS_THR_SetPoint = 2,PH_THR_SetPoint = 3,PH_SetPoint = 4;
uint8_t day = 0, month = 0, hour= 0, minute = 0, second = 0;
uint16_t year = 0;
float   esp32_stm32_SetPoint[] = {0};
int     esp32_stm32_History[] = {0};

uint8_t SPI1_SD_WriteBufferData[] = "";			//Store SD data when Write to SD
uint8_t SPI1_SD_ReadBufferData[1024];			//Store SD data Read from SD
uint8_t FileName[20];							//Name of File namme
uint8_t UART1_TEMPBUFFER[SIZEOF_COMMAND];		//Temp buffer when receive data UART1
uint8_t UART1_MAINBUFFER[UART1_BUFFER_SIZE];	//MAIN buffer store data UART1
/*=====================================COMMON_END=================================*/
//
//
//
//
/*=====================================UART1=================================*/
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	memset(UART1_MAINBUFFER,0,strlen((char*)UART1_MAINBUFFER));
	if(huart->Instance == USART1)
	{
		memcpy(UART1_MAINBUFFER,UART1_TEMPBUFFER, Size);
		HAL_UARTEx_ReceiveToIdle_DMA(&huart1,UART1_TEMPBUFFER,SIZEOF_COMMAND);
		__HAL_DMA_DISABLE_IT(&hdma_usart1_rx,DMA_IT_HT);
	}
	HAL_UART_Transmit(&huart1, UART1_MAINBUFFER,strlen((char*)UART1_MAINBUFFER), 500);
}

void SEND_UART1(uint8_t *String)
{
	HAL_UART_Transmit(&huart1,(uint8_t*)String,strlen(String), 500);
}
/*=====================================PH,TDS_BEGIN=================================*/
uint32_t ADC_Value[2] = {0};
float 	a = 0,b = 0;
void PH_Calibration()
{
 //Ax+B = y
	int value_calib[2] = {0};
	float CALIB_ADC_PH_4 = 0,CALIB_ADC_PH_7 = 0;
	HAL_ADC_Start_DMA(&hadc1, value_calib, 2);
	  if(value_calib[0] > 1990 && value_calib[0] < 2030)
	  {
	      CALIB_ADC_PH_7 = value_calib[0];
	      CALIB_ADC_PH_4 = CALIB_ADC_PH_7 + 110;

	    }
	    else if(value_calib[0] < 2100 && value_calib[0] > 2060)
	    {
	      CALIB_ADC_PH_4 = value_calib[0];
	      CALIB_ADC_PH_7 = CALIB_ADC_PH_7 - 110;
	      }

	  CALIB_ADC_PH_4 = CALIB_ADC_PH_4/1000;
	  CALIB_ADC_PH_7 = CALIB_ADC_PH_7/1000;
	  a = (7-4)/(CALIB_ADC_PH_7 - CALIB_ADC_PH_4);
	  b = 4 - (a*CALIB_ADC_PH_4);
}
float PH_Calculator(float A, float B, uint32_t adc)
{
	return (adc/1000)*A + B;
	}

/*=====================================PH,TDS_END=================================*/

uint32_t now =0,last = 0, pointer_position = 0,pointer_2_position =0,Rpush_number =0,button_flag= 0,Page = 0;
uint32_t lcd_pointer_1;
typedef enum
{
	Down,
	Up,
	NoChange
}Pointer_state;

typedef enum
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

typedef enum
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
	Stds_phr

}Value_Setype;

/*=====================================ROTARY_START=================================*/
void Rotary_init()
{
	rotary_first_value = HAL_GPIO_ReadPin(GPIOE, Rotary_CLK_Pin);
}
uint32_t Rotary_volum()
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
//	  				if(Rpush_number > 1)
//	  				{
//	  					Rpush_number = 0;
//	  				}
	  			}
	  		}
	  		else{isPress = 0;}
}

/*=====================================ROTARY_END=================================*/
/*=====================================LCD_START=================================*/
Page1 option_page_1 = Page1_Nothing;
Page2 option_page_2 = Page2_Nothing;
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

void LCD_Menu_2_1() // Menu SetPoint
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
	strcpy(buffer_string,0);


	lcd_send_cmd(0x80 | 0x4B); //PH_THR_Setpoint
	sprintf(buffer_string,"%.2f",PH_THR_SetPoint);
	lcd_send_string(buffer_string);
	strcpy(buffer_string,0);

	lcd_send_cmd(0x80 | 0x1F); //TDS_Setpoint
	sprintf(buffer_string,"%.2f",TDS_SetPoint);
	lcd_send_string(buffer_string);
	strcpy(buffer_string,0);


	lcd_send_cmd(0x80 | 0x5F); //TDS_THR_Setpoint
	sprintf(buffer_string,"%.2f",TDS_THR_SetPoint);
	lcd_send_string(buffer_string);
	strcpy(buffer_string,0);
}

void LCD_Menu_2_2() // Menu Testing
{
	char buffer_string[100];
	lcd_send_cmd(0x80 | 0x02); //PH
	lcd_send_string("Pump 1:");
	lcd_send_cmd(0x80 | 0x42); //PH
	lcd_send_string("Pump 2:");
	lcd_send_cmd(0x80 | 0x16); //PH
	lcd_send_string("Pump 3:");
	lcd_send_cmd(0x80 | 0x56); //PH
	lcd_send_string("Pump 4:");

}

void LCD_Menu_2_3() // Menu Calib
{
	char buffer_string[100];
	lcd_send_cmd(0x80 | 0x02); //PH
	lcd_send_string("PH:");
	lcd_send_cmd(0x80 | 0x42); //PH
	lcd_send_string("TDS:");
	lcd_send_cmd(0x80 | 0x16); //PH

}

void LCD_Menu_2_4() // Menu Wifi_Config
{
	char buffer_string[100];
	lcd_send_cmd(0x80 | 0x02); //PH
	lcd_send_string("Go to the link:");
	lcd_send_cmd(0x80 | 0x42); //PH
	lcd_send_string("setupwifi.com.vn");
	lcd_send_cmd(0x80 | 0x16); //PH
	lcd_send_string("Start");
	lcd_send_cmd(0x80 | 0x56); //PH
	lcd_send_string("End");
}
void LCD_Menu_1()   // Menu Select
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

void LCD_Normal_Mode() // Default Page
{
	char PH_String[100];
	sprintf(PH_String,"PH  :%.2f   ",PH);
	lcd_send_cmd(0x80 | 0x03); //PH
	lcd_send_string(PH_String);
	strcpy(PH_String,0);

	sprintf(PH_String,"TDS :%.0f   ",TDS);
	lcd_send_cmd(0x80 | 0x43); //PH
	lcd_send_string(PH_String);
	strcpy(PH_String,0);

	sprintf(PH_String,"TEMP:%.2f   ",Temp);
	lcd_send_cmd(0x80 | 0x17); //PH
	lcd_send_string(PH_String);
	strcpy(PH_String,0);
	lcd_send_cmd(0x80 | 0x54); //PH
	lcd_send_string("PRESS TO CONFIG MODE");
}

void LCD_Display()
{
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
				  LCD_Menu_2_1();
				  if(option_page_2 == Page2_ph)
				  {
					  lcd_pointer_1 = PH_SetPoint;
						while(Rpush_number == 3)
							{
											LCD_Menu_2_1();
											Push_Slect();
											Pointer_2_Status(0);
											PH_SetPoint = Rotary_volum();

							}

				  }
				  else if(option_page_2== Page2_ph_thr)
				  {
					  lcd_pointer_1 = PH_THR_SetPoint;
						while(Rpush_number== 3)
							{
											LCD_Menu_2_1();
											Push_Slect();
											Pointer_2_Status(2);
											PH_THR_SetPoint =  Rotary_volum();
							}

				  }
				  else if(option_page_2==Page2_tds)
				  {
					  lcd_pointer_1 = TDS_SetPoint;
						while(Rpush_number== 3)
							{
											LCD_Menu_2_1();
											Push_Slect();
											Pointer_2_Status(4);
											TDS_SetPoint = Rotary_volum();
							}

				  }
				  else if(option_page_2==Page2_tds_thr)
				  {
					    lcd_pointer_1 = TDS_THR_SetPoint;
						while(Rpush_number == 3)
						{
											LCD_Menu_2_1();
											Push_Slect();
											Pointer_2_Status(6);
											TDS_THR_SetPoint = Rotary_volum();
						}
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
			else if(option_page_1 == Page1_Calbration_sensor) // Display with calibration
			{
					LCD_Menu_2_3();
					if(option_page_2 == Page2_calib_ph)
					{
						while(Rpush_number == 3)
						{
											LCD_Menu_2_3();
											Push_Slect();
											Pointer_2_Status(0);
											lcd_send_cmd(0x80|0x16);
											lcd_send_string("Please put sensor into PH7 or PH4");

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
					  lcd_clear();}
			}
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


/*=====================================LCD_END=================================*/

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_SPI1_Init();
  MX_FATFS_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
//  HAL_UARTEx_ReceiveToIdle_DMA(&huart1,UART1_TEMPBUFFER,SIZEOF_COMMAND);
//  __HAL_DMA_DISABLE_IT(&hdma_usart1_rx,DMA_IT_HT);
//  char *FileName = "Truongs_house.txt";
//  HAL_Delay(500);
//  SD_Handling(SPI1_SD_ReadBufferData);
//  SD_Write(FileName, "Truong was here!!!\n");
//  SD_Read(FileName, SPI1_SD_ReadBufferData);
//
//  SEND_UART1(SPI1_SD_ReadBufferData);
  HAL_Delay(50);
  lcd_init();
  Rotary_init();


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */


    /* USER CODE BEGIN 3 */
	  LCD_Display();

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV6;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 2;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_144CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
  /* DMA2_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : SPI1_CS_Pin */
  GPIO_InitStruct.Pin = SPI1_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SPI1_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : Rotary_CLK_Pin Rotary_DT_Pin Rotary_SW_Pin */
  GPIO_InitStruct.Pin = Rotary_CLK_Pin|Rotary_DT_Pin|Rotary_SW_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
