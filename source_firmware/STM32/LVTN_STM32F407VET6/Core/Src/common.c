/*
 * common.c
 *
 *  Created on: Jan 14, 2023
 *      Author: Xuan Truong
 */

#include"common.h"

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

void GET_VALUE_FROM_ESP32(char string[])
{
    char *first;
    char *token;
    int i = 0;

    first = strstr(string,"{");
    token = strtok((char*)first+1,",");
    printf("%s\n",token);
    if(string[0] == 'S')		//Setpoint
    {
         while( token != NULL )
          {
              token = strtok(NULL,",");
              //printf("%s\n",token);
              esp32_stm32_SetPoint[i] = atof(token);
              printf("%.2f\n",esp32_stm32_SetPoint[i]);
              i++;
              if(i == 3){break;}
          }
    }
    else if (string[0] == 'H')
    {
         while( token != NULL )
          {
              token = strtok(NULL,",");
              //printf("%s\n",token);
              esp32_stm32_History[i] = atoi(token);
              printf("%d\n",esp32_stm32_History[i]);
              i++;
              if(i == 2){break;}
          }
    }
}
