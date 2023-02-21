/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

typedef enum
{
	flash_setpoint,
	flash_calibration

}Save_Flash_Type;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ADC1_PH_Sensor_Pin GPIO_PIN_0
#define ADC1_PH_Sensor_GPIO_Port GPIOA
#define ADC1_TDS_Sensor_Pin GPIO_PIN_1
#define ADC1_TDS_Sensor_GPIO_Port GPIOA
#define SPI1_CS_Pin GPIO_PIN_4
#define SPI1_CS_GPIO_Port GPIOA
#define test_pin_Pin GPIO_PIN_6
#define test_pin_GPIO_Port GPIOA
#define FLASH_CS_Pin GPIO_PIN_0
#define FLASH_CS_GPIO_Port GPIOB
#define Rotary_CLK_Pin GPIO_PIN_7
#define Rotary_CLK_GPIO_Port GPIOE
#define Rotary_DT_Pin GPIO_PIN_8
#define Rotary_DT_GPIO_Port GPIOE
#define Rotary_SW_Pin GPIO_PIN_9
#define Rotary_SW_GPIO_Port GPIOE
#define LCD_I2C1_SCL_Pin GPIO_PIN_6
#define LCD_I2C1_SCL_GPIO_Port GPIOB
#define LCD_I2C1_SDA_Pin GPIO_PIN_7
#define LCD_I2C1_SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
