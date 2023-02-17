/*
 * sd_card.h
 *
 *  Created on: Jan 15, 2023
 *      Author: Xuan Truong
 */

#ifndef _SD_CARD_H_
#define _SD_CARD_H_
#include "fatfs.h"
#include"fatfs_sd.h"
#include"common.h"




void SD_Handling(uint8_t *SD_ReadBufferData);
void SD_Write(char *NameOfFile,  char *Data);
void SD_Read(char* NameOfFile, uint8_t *buffer);
#endif /* INC_SD_CARD_H_ */
