/*
 * common.c
 *
 *  Created on: Jan 14, 2023
 *      Author: Xuan Truong
 */

#include"common.h"
#include <math.h>

extern TIM_HandleTypeDef htim6;
extern void Read_SetPoint(Save_Flash_Type tp);
extern void Save_SetPoint(Save_Flash_Type tp);
extern float TDS_SetPoint,TDS_THR_SetPoint,PH_THR_SetPoint,PH_SetPoint;
void BUFFER_CLEAR(uint8_t *buff, uint32_t sizebuff)
{
	for(int i = 0; i<sizebuff;i++)
	{
		buff[i] = '\0';
	}
}

unsigned int BUFF_SIZE(uint8_t *buff)
{
	int i = 0;
	while(*(buff++) != '\0')
	{i++;}
	return i;
}




