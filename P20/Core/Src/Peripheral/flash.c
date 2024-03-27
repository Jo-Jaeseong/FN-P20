/*
 * flash.c
 *
 *  Created on: 2023. 1. 11.
 *      Author: CBT
 */

#include "main.h"

#include "flash.h"
#include "lcd.h"
#include "hardware.h"
#include "stm32f4xx_hal.h"
#include <string.h>
#include <rfid.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

__attribute__((__section__(".user_data"))) const char userConfig[1024];

extern char test1234[4];

void Write_Flash(){
	unsigned char ucData[1024]={};

	ucData[0]=1;
	ucData[1]=2;
	ucData[2]=3;
	ucData[3]=4;

	HAL_FLASH_Unlock();
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGSERR );
	FLASH_Erase_Sector(FLASH_SECTOR_6, VOLTAGE_RANGE_3);

	for(int i = 0; i < 1024; i++) {
	 HAL_FLASH_Program(TYPEPROGRAM_BYTE, (uint32_t)&userConfig[0] + i, ucData[i]);
	}

	HAL_FLASH_Lock();
}

void Read_Flash(){
	test1234[0]=userConfig[0];
	test1234[1]=userConfig[1];
	test1234[2]=userConfig[2];
	test1234[3]=userConfig[3];
}



float char2float(const unsigned char *data)
{
	union fc	temp;
	for(int i = 0; i < 4; i++) {
		temp.cValue[i] = data[i];
	}
	return(temp.fValue);
}

void float2char(float fValue, char *data)
{
	union fc	temp;

	temp.fValue = fValue;
	for(int i = 0; i < 4; i++) {
		data[i] = temp.cValue[i];
	}
}
