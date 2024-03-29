/*
 * common.h
 *
 *  Created on: Jan 14, 2023
 *      Author: Xuan Truong
 */

#ifndef INC_COMMON_H_
#define INC_COMMON_H_

#include "stm32f4xx_hal.h"
#include<string.h>
#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include "main.h"

extern float  PH,PH_THR,TDS,TDS_THR,NU;
extern float  esp32_stm32_SetPoint[];
extern int    esp32_stm32_History[];
//extern float TDS_SetPoint_from_web,TDS_THR_SetPoint_from_web,PH_THR_SetPoint_from_web,PH_SetPoint_from_web;

void BUFFER_CLEAR(uint8_t *buff, uint32_t sizebuff);
unsigned int BUFF_SIZE(uint8_t *buff);
//void GET_VALUE_FROM_ESP32(char string[]);
//void delay_us(uint32_t time);



#endif /* INC_COMMON_H_ */
