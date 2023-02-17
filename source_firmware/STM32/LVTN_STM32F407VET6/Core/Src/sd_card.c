/*
 * sd_card.c
 *
 *  Created on: Jan 15, 2023
 *      Author: Xuan Truong
 */


#include"sd_card.h"


FATFS fs;  								//File system
FIL file;								//File
FRESULT fresult;						//Store result
FILINFO fno;
UINT br, bw;
FATFS *pfs;
DWORD fre_clust;
uint32_t total, free_space;

void SD_Handling(uint8_t *SD_ReadBufferData)
{
	fresult = f_mount(&fs, "/", 1);
	if (fresult != FR_OK) {SEND_UART1("ERROR!!! in mounting SD CARD...\n\n");}
	else {
		f_getfree("", &fre_clust, &pfs);
		SEND_UART1("SD CARD mounted successfully...\n\n");
	  	total = (uint32_t)((pfs->n_fatent - 2) * pfs->csize * 0.5);
	  	sprintf (SD_ReadBufferData, "SD CARD Total Size: \t%lu\n",total);
	  	SEND_UART1(SD_ReadBufferData);
	  	BUFFER_CLEAR(SD_ReadBufferData,BUFF_SIZE(SD_ReadBufferData));
	  	free_space = (uint32_t)(fre_clust * pfs->csize * 0.5);
	  	sprintf (SD_ReadBufferData, "SD CARD Free Space: \t%lu\n\n",free_space);
	  	SEND_UART1(SD_ReadBufferData);
	  	BUFFER_CLEAR(SD_ReadBufferData,BUFF_SIZE(SD_ReadBufferData));
	}
}

void SD_Write(char *NameOfFile,  char *Data)
{

		fresult = f_open(&file, NameOfFile, FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
		f_lseek(&file, f_size(&file));
		  if (fresult == FR_OK)
			  {
			  	  SEND_UART1 ("file is created\n");
			  	 }
		  else
		  {
			  SEND_UART1 ("Create file is fail\n");
		  }
		f_puts(Data, &file);
	  	fresult = f_close(&file);
}

void SD_Read(char* NameOfFile, uint8_t *buffer)
{
	fresult = f_open(&file, NameOfFile,FA_READ);
	f_read(&file, buffer, f_size(&file), &br);
	//f_gets(buffer, file.fsize, &file);
	fresult = f_close(&file);

}
