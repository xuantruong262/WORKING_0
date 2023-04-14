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
#include "w25qxx.h"
#include "lcd_menu.h"
#include "ds1307.h"
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

I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c3;

IWDG_HandleTypeDef hiwdg;

RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;

TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim6;

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM4_Init(void);
static void MX_IWDG_Init(void);
static void MX_TIM6_Init(void);
static void MX_SPI2_Init(void);
static void MX_SPI1_Init(void);
static void MX_I2C3_Init(void);
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/*=====================================COMMON=================================*/
#define SIZEOF_COMMAND 1024
#define UART1_BUFFER_SIZE 1024

uint16_t time_tds = 0;
uint16_t time_ph = 0;

float TDS = 0,TDS_THR= 0,PH = 0,PH_THR = 0,Temperature = 0;


float TDS_SetPoint = 0,TDS_THR_SetPoint = 0,PH_THR_SetPoint = 0,PH_SetPoint = 0;

uint8_t config_wifi_flag = 0;
uint8_t save_sd_flag = 0;
uint8_t from_web_flag = 0;

float   esp32_stm32_SetPoint[] = {0};
int     esp32_stm32_History[] = {0};

//uint8_t SD_Write[] = "";			//Store SD data when Write to SD
uint8_t SD_Read[1024];							//Store SD data Read from SD
uint8_t FileName[20];							//Name of File namme
uint8_t UART1_TEMPBUFFER[SIZEOF_COMMAND];		//Temp buffer when receive data UART1
uint8_t UART1_MAINBUFFER[UART1_BUFFER_SIZE];	//MAIN buffer store data UART1

void delay_us(uint16_t time)
{
    __HAL_TIM_SET_COUNTER(&htim6,0);
    while ((__HAL_TIM_GET_COUNTER(&htim6))<time);
}


void Save_SetPoint(Save_Flash_Type tp);
/*=====================================COMMON_END=================================*/
//
//
//
//
/*=====================================UART1=================================*/
typedef enum
{
	Value,
	Volume,
	WifiConfig,
}Message_type;
char buffer_send[100];

void SEND_UART1(char *String)
{
	HAL_UART_Transmit(&huart1,(uint8_t*)String,strlen(String), 1000);
}


void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	memset(UART1_MAINBUFFER,0,strlen((char*)UART1_MAINBUFFER));
	if(huart->Instance == USART1)
	{
		memcpy(UART1_MAINBUFFER,UART1_TEMPBUFFER, Size);
		HAL_UARTEx_ReceiveToIdle_DMA(&huart1,UART1_TEMPBUFFER,SIZEOF_COMMAND);
		__HAL_DMA_DISABLE_IT(&hdma_usart1_rx,DMA_IT_HT);
	}

	if(!strcmp(UART1_MAINBUFFER,"Config_Wifi_done"))
	{
		HAL_UART_Transmit(&huart1, UART1_MAINBUFFER,strlen((char*)UART1_MAINBUFFER), 1000);
		config_wifi_flag = 1;
	}

	if(UART1_MAINBUFFER[2] == 80)
	{
//		SEND_UART1(UART1_MAINBUFFER);

		GET_VALUE_FROM_ESP32(UART1_MAINBUFFER);

	}
	memset(UART1_MAINBUFFER,0,Size);

}


void Handle_value_send(Message_type tp)
{
	char msg_send[100];

	if(tp == Value)
	{
		memset(msg_send,0,strlen(msg_send));
		sprintf(msg_send,"{\"ID\":\"123456789\",\"PH\":\"%.2f\",\"TDS\":\"%.0f\",\"Temp\":\"%.2f\"}",PH,TDS,Temperature);
		SD_save(msg_send);
		SEND_UART1(msg_send);
	}
	else if(tp == WifiConfig)
	{
		SEND_UART1("wificonfigmode");
	}
}
/*=====================================PH,TDS_BEGIN=================================*/
uint16_t ADC_Value[2] = {0};
int ADC_PH_4 = 0, ADC_PH_7 = 0;
float 	ph_a_value = 0,ph_b_value = 0,tds_k_value = 0;
void get_adc();
void PH_Calibration()
{
		//Ax+B = y => ADC_PH_4 - ADC_PH_7 = 640
	get_adc();

	  if(ADC_Value[0] > 1540 && ADC_Value[0] < 2180)
	  {
		  ADC_PH_7 = ADC_Value[0];
		  ADC_PH_4 = ADC_PH_7 + 640;
	    }
	    else if(ADC_Value[0] < 2820 && ADC_Value[0] > 2180)
	    {
	    	ADC_PH_4 = ADC_Value[0];
	    	ADC_PH_7 = ADC_PH_4 - 640;
	      }

	  ph_a_value = (float)(3/(float)(ADC_PH_7 - ADC_PH_4));
	  ph_b_value = (float)((4 - (ph_a_value*(float)ADC_PH_4)));

}
float PH_Calculator(float A, float B, uint16_t adc)
{
	return (float)(adc*A + B);
}

void TDS_Calibration()
{
	get_adc();

	tds_k_value = (1000/(float)ADC_Value[1]);
}

float TDS_Calculator(float k, uint16_t adc)
{
	return (float)(k*adc);
}

/*=====================================PH,TDS_END=================================*/



/*=====================================Flash_Start=================================*/

void Save_SetPoint(Save_Flash_Type tp)
{

	if(tp == flash_setpoint)
	{
		time_tds = 80;
		time_ph = 80;
		HAL_Delay(10);
		W25qxx_EraseSector(1);
		W25qxx_EraseSector(2);
		W25qxx_EraseSector(3);
		W25qxx_EraseSector(4);
		W25qxx_WriteSector(&PH_SetPoint, 1, 0, 4);
		W25qxx_WriteSector(&PH_THR_SetPoint, 2, 0, 4);
		W25qxx_WriteSector(&TDS_SetPoint, 3, 0, 4);
		W25qxx_WriteSector(&TDS_THR_SetPoint, 4, 0, 4);
	}
	else if(tp ==flash_calibration_ph)
	{
		W25qxx_EraseSector(5);
		W25qxx_EraseSector(6);
		W25qxx_WriteSector(&ph_a_value, 5, 0, 4);
		W25qxx_WriteSector(&ph_b_value, 6, 0, 4);
	}
	else if (tp == flash_calibration_tds)
	{
		W25qxx_EraseSector(7);
		W25qxx_WriteSector(&tds_k_value, 7, 0, 4);

	}

}
void Read_SetPoint(Save_Flash_Type tp)
{
	if(tp == flash_setpoint)
	{
		W25qxx_ReadSector(&PH_SetPoint, 1, 0, 4);
		W25qxx_ReadSector(&PH_THR_SetPoint, 2, 0, 4);
		W25qxx_ReadSector(&TDS_SetPoint, 3, 0, 4);
		W25qxx_ReadSector(&TDS_THR_SetPoint, 4, 0, 4);
	}
	else if(tp == flash_calibration_ph)
	{
		W25qxx_ReadSector(&ph_a_value, 5, 0, 4);
		W25qxx_ReadSector(&ph_b_value, 6, 0, 4);
	}
	else if (tp == flash_calibration_tds)
	{
		W25qxx_ReadSector(&tds_k_value, 7, 0, 4);
	}
}

/*=====================================Flash_End=================================*/


/*=====================================DS18B20_Start=================================*/
uint8_t Temp_byte1, Temp_byte2;
uint16_t TEMP;
#define DS18B20_PORT Temperature_Pin_GPIO_Port
#define DS18B20_PIN Temperature_Pin_Pin

void Set_Pin_Output (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}
void Set_Pin_Input (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

uint8_t DS18B20_Start (void)
{
	uint8_t Response = 0;
	Set_Pin_Output(DS18B20_PORT, DS18B20_PIN);   // set the pin as output
	HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN, 0);  // pull the pin low
	delay_us(480);   // delay according to datasheet

	Set_Pin_Input(DS18B20_PORT, DS18B20_PIN);    // set the pin as input
	delay_us(80);    // delay according to datasheet

	if (!(HAL_GPIO_ReadPin (DS18B20_PORT, DS18B20_PIN))) Response = 1;    // if the pin is low i.e the presence pulse is detected
	else Response = -1;

	delay_us(400); // 480 us delay totally.

	return Response;
}


void DS18B20_Write (uint8_t data)
{
	Set_Pin_Output(DS18B20_PORT, DS18B20_PIN);  // set as output

	for (int i=0; i<8; i++)
	{
		if ((data & (1<<i))!=0)  // if the bit is high
		{
			// write 1

			Set_Pin_Output(DS18B20_PORT, DS18B20_PIN);  // set as output
			HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN, 0);  // pull the pin LOW
			delay_us(1);  // wait for 1 us

			Set_Pin_Input(DS18B20_PORT, DS18B20_PIN);  // set as input
			delay_us(50);  // wait for 60 us
		}

		else  // if the bit is low
		{
			// write 0
			Set_Pin_Output(DS18B20_PORT, DS18B20_PIN);
			HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN, 0);  // pull the pin LOW
			delay_us(50);  // wait for 60 us
			Set_Pin_Input(DS18B20_PORT, DS18B20_PIN);
		}
	}
}

uint8_t DS18B20_Read (void)
{
	uint8_t value=0;

	Set_Pin_Input(DS18B20_PORT, DS18B20_PIN);

	for (int i=0;i<8;i++)
	{
		Set_Pin_Output(DS18B20_PORT, DS18B20_PIN);   // set as output

		HAL_GPIO_WritePin (DS18B20_PORT, DS18B20_PIN, 0);  // pull the data pin LOW
		delay_us(1);  // wait for > 1us

		Set_Pin_Input(DS18B20_PORT, DS18B20_PIN);  // set as input
		if (HAL_GPIO_ReadPin (DS18B20_PORT, DS18B20_PIN))  // if the pin is HIGH
		{
			value |= 1<<i;  // read = 1
		}
		delay_us(50);  // wait for 60 us
	}
	return value;
}

float Get_Temperature_DS18B20()
{
		  DS18B20_Start ();
		  HAL_Delay(1);
		  DS18B20_Write (0xCC);  // skip ROM
		  DS18B20_Write (0x44);  // convert t
		  HAL_Delay (800);

		  DS18B20_Start ();
	      HAL_Delay(1);
	      DS18B20_Write (0xCC);  // skip ROM
	      DS18B20_Write (0xBE);  // Read Scratch-pad
	      Temp_byte1 = DS18B20_Read();
		  Temp_byte2 = DS18B20_Read();
		  TEMP = (Temp_byte2<<8)|Temp_byte1;
		  return (float)TEMP/16;
}
/*=====================================DS18B20_End=================================*/

/*=====================================Interrupt_Start=========================*/

void TDS_Control()
{
	   uint32_t i = 0;
	   if((time_tds%90) == 0)
	   {
		   if(TDS < (TDS_SetPoint-TDS_THR_SetPoint))
		   {
			   HAL_GPIO_WritePin(NutriA_GPIO_Port,NutriA_Pin, GPIO_PIN_RESET);
			   HAL_GPIO_WritePin(NutriB_GPIO_Port,NutriB_Pin, GPIO_PIN_RESET);
			   while(i<14000000)
			   {
				   i++;
			   }

			   HAL_GPIO_WritePin(NutriA_GPIO_Port,NutriA_Pin, GPIO_PIN_SET);
			   HAL_GPIO_WritePin(NutriB_GPIO_Port,NutriB_Pin, GPIO_PIN_SET);
		   }

		   time_tds = 1;
	   }
	   time_tds++;
}

void PH_Control()
{
	   uint32_t i = 0;
	   if((time_ph%90) == 0)
	   {
		   if(PH < (PH_SetPoint-PH_THR_SetPoint))
		   {
			   HAL_GPIO_WritePin(BASE_GPIO_Port,BASE_Pin, GPIO_PIN_RESET);
			   while(i<14000000)
			   {
				   i++;
			   }
			   HAL_GPIO_WritePin(BASE_GPIO_Port,BASE_Pin, GPIO_PIN_SET);
		   }
		   else if(PH > (PH_SetPoint + PH_THR_SetPoint))
		   {
			   HAL_GPIO_WritePin(ACID_GPIO_Port,ACID_Pin, GPIO_PIN_RESET);
			   while(i<14000000)
			   {
				   i++;
			   }
			   HAL_GPIO_WritePin(ACID_GPIO_Port,ACID_Pin, GPIO_PIN_SET);
		   }
		   time_ph = 1;
	   }
	   time_ph++;
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

	 if(htim->Instance == htim4.Instance)
	 {
	   HAL_GPIO_TogglePin(test_pin_GPIO_Port,test_pin_Pin);
	   Handle_value_send(Value);
	   TDS_Control();
	   PH_Control();

	 }
}
/*=====================================Interrupt_End=========================*/

/*=====================================WIFI_CONFIG_Start=========================*/
void Wifi_Config()
{
	Handle_value_send(WifiConfig);
}

RTC_TimeTypeDef sTime = {0};
RTC_DateTypeDef sDate = {0};

float Ring_Buffer_PH[10]  = {0};
float Ring_Buffer_TDS[10] = {0};
//
void get_time(void)
{
	char tmp_string[100] = {0};
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
	sprintf(tmp_string,"%d/%d/%d- %d:%d:%d\n",sDate.Date,sDate.Month,sDate.Year,sTime.Hours,sTime.Minutes,sTime.Seconds);
	memset(tmp_string,0,strlen(tmp_string));
}
void ADC_Select_CH0 (void)
{
	ADC_ChannelConfTypeDef sConfig = {0};
	  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
	  */
	  sConfig.Channel = ADC_CHANNEL_0;
	  sConfig.Rank = 1;
	  sConfig.SamplingTime = ADC_SAMPLETIME_144CYCLES;
	  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
}

void ADC_Select_CH1 (void)
{
	ADC_ChannelConfTypeDef sConfig = {0};
	  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
	  */
	  sConfig.Channel = ADC_CHANNEL_1;
	  sConfig.Rank = 1;
	  sConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES;
	  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
}
void get_adc()
{
	ADC_Select_CH0();
	  HAL_ADC_Start(&hadc1);
	  HAL_ADC_PollForConversion(&hadc1, 10);
	  ADC_Value[0] = HAL_ADC_GetValue(&hadc1);
	  HAL_ADC_Stop(&hadc1);
	ADC_Select_CH1();
	  ADC_Select_CH1();
	  HAL_ADC_Start(&hadc1);
	  HAL_ADC_PollForConversion(&hadc1, 10);
	  ADC_Value[1] = HAL_ADC_GetValue(&hadc1);
	  HAL_ADC_Stop(&hadc1);
	}
float PH_average(uint32_t t, float ph)
{
	return (float)(ph/t);
}

float TDS_average(uint32_t t, float tds)
{
	return (float)(tds/t);
}

uint32_t tim 		= 0;
uint32_t tong_ph 	= 0;
uint32_t tong_tds = 0;
float    PH_t 	= 0;
float 	TDS_t 	= 0;
/*=====================================WIFI_CONFIG_End=========================*/

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
  MX_FATFS_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_TIM4_Init();
  MX_IWDG_Init();
  MX_TIM6_Init();
  MX_SPI2_Init();
  MX_SPI1_Init();
  MX_I2C3_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
 //=========================================================================  TESTING


 //=========================================================================  CONFIG_MAIN
  HAL_TIM_Base_Start_IT(&htim4);
  HAL_TIM_Base_Start(&htim6);

  HAL_UARTEx_ReceiveToIdle_DMA(&huart1,UART1_TEMPBUFFER,SIZEOF_COMMAND);
  __HAL_DMA_DISABLE_IT(&hdma_usart1_rx,DMA_IT_HT);
//
  for(int i =0;i<5;i++)
  {
	  HAL_GPIO_TogglePin(test_pin_GPIO_Port,test_pin_Pin);
	  HAL_Delay(200);
  }
  HAL_Delay(50);
  lcd_init();
  Rotary_init();
  lcd_clear();
  W25qxx_Init();

//  Save_SetPoint(flash_setpoint);
//  Save_SetPoint(flash_calibration_tds);
//  Save_SetPoint(flash_calibration_ph);
  Read_SetPoint(flash_setpoint);
  Read_SetPoint(flash_calibration_tds);
  Read_SetPoint(flash_calibration_ph);
  SD_Handling(SD_Read);
//  int co = 0;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    while (1)
  {
	  //=========================================================================  TESTING


	  //=========================================================================  CONFIG_MAIN
	  LCD_Display();
      get_adc();
	  PH_t = PH_Calculator(ph_a_value, ph_b_value, ADC_Value[0]);
	  TDS_t = TDS_Calculator(tds_k_value, ADC_Value[1]);
	  tong_ph = tong_ph + PH_t;
	  tong_tds = tong_tds + TDS_t;
	  tim++;
      if(tim == 100)
      {
//    	  PH = PH_Calculator(ph_a_value, ph_b_value, ADC_Value[0]);
//    	  TDS = TDS_Calculator(tds_k_value, ADC_Value[1]);
//    	  Temperature = Get_Temperature_DS18B20();
    	  PH = PH_average(tim, tong_ph);
    	  TDS = TDS_average(tim, tong_tds);
    	  Temperature = Get_Temperature_DS18B20();
    	  tim = 0;
    	  tong_ph = 0;
    	  tong_tds = 0;

      }
//	  Ring_Buffer_PH[tim] = PH_t;
//	  Ring_Buffer_TDS[tim] = TDS_t;
      get_time();
	  HAL_IWDG_Refresh(&hiwdg);



    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
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
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
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
//  sConfig.Channel = ADC_CHANNEL_0;
//  sConfig.Rank = 1;
//  sConfig.SamplingTime = ADC_SAMPLETIME_144CYCLES;
//  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
//  {
//    Error_Handler();
//  }
//
//  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
//  */
//  sConfig.Channel = ADC_CHANNEL_1;
//  sConfig.Rank = 2;
//  sConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES;
//  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
//  {
//    Error_Handler();
//  }
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
  * @brief I2C3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C3_Init(void)
{

  /* USER CODE BEGIN I2C3_Init 0 */

  /* USER CODE END I2C3_Init 0 */

  /* USER CODE BEGIN I2C3_Init 1 */

  /* USER CODE END I2C3_Init 1 */
  hi2c3.Instance = I2C3;
  hi2c3.Init.ClockSpeed = 100000;
  hi2c3.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c3.Init.OwnAddress1 = 0;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C3_Init 2 */

  /* USER CODE END I2C3_Init 2 */

}

/**
  * @brief IWDG Initialization Function
  * @param None
  * @retval None
  */
static void MX_IWDG_Init(void)
{

  /* USER CODE BEGIN IWDG_Init 0 */

  /* USER CODE END IWDG_Init 0 */

  /* USER CODE BEGIN IWDG_Init 1 */

  /* USER CODE END IWDG_Init 1 */
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_256;
  hiwdg.Init.Reload = 2499;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN IWDG_Init 2 */

  /* USER CODE END IWDG_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */


  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */
//  if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) == 0x2608)
//  {
//	  sTime.Hours = 0;
//	  sTime.Minutes = 24;
//	  sTime.Seconds = 0x0;
//	  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
//	  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
//	  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
//	  {
//	    Error_Handler();
//	  }
//	  sDate.WeekDay = RTC_WEEKDAY_WEDNESDAY;
//	  sDate.Month = RTC_MONTH_MARCH;
//	  sDate.Date = 0x23;
//	  sDate.Year = 0x23;
//
//	  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
//	  {
//	    Error_Handler();
//	  }
//	  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x2608);
//  }
  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
//  sTime.Hours = 0x10;
//  sTime.Minutes = 0x40;
//  sTime.Seconds = 0x0;
//  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
//  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
//  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  sDate.WeekDay = RTC_WEEKDAY_FRIDAY;
//  sDate.Month = RTC_MONTH_MARCH;
//  sDate.Date = 0x24;
//  sDate.Year = 0x23;
//
//  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
//  {
//    Error_Handler();
//  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

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
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
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
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 50000-1;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 14400-1;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 72-1;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 0xffff-1;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SD_CS_SPI2_GPIO_Port, SD_CS_SPI2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, Temperature_Pin_Pin|test_pin_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, ACID_Pin|BASE_Pin|NutriA_Pin|NutriB_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : SD_CS_SPI2_Pin */
  GPIO_InitStruct.Pin = SD_CS_SPI2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SD_CS_SPI2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : Temperature_Pin_Pin test_pin_Pin */
  GPIO_InitStruct.Pin = Temperature_Pin_Pin|test_pin_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : FLASH_CS_Pin */
  GPIO_InitStruct.Pin = FLASH_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(FLASH_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : Rotary_CLK_Pin Rotary_DT_Pin Rotary_SW_Pin */
  GPIO_InitStruct.Pin = Rotary_CLK_Pin|Rotary_DT_Pin|Rotary_SW_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : ACID_Pin BASE_Pin NutriA_Pin NutriB_Pin */
  GPIO_InitStruct.Pin = ACID_Pin|BASE_Pin|NutriA_Pin|NutriB_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

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
