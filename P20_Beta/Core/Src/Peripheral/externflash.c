/*
 * extern_flash.c
 *
 *  Created on: Apr 12, 2023
 *      Author: CBT
 */

#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "hardware.h"
#include "sensor.h"
#include "peripheral.h"

/* W25Q16 SPI Commands */
#define W25Q16_CMD_WRITE_ENABLE 0x06
#define W25Q16_CMD_WRITE_DISABLE 0x04
#define W25Q16_CMD_READ_STATUS_REG1 0x05
#define W25Q16_CMD_READ_DATA 0x03
#define W25Q16_CMD_PAGE_PROGRAM 0x02
#define W25Q16_CMD_ERASE_SECTOR 0x20
#define W25Q16_CMD_ERASE_CHIP 0xC7
#define W25Q16_STATUS_BUSY 0x01

/* W25Q16 Pinout */
#define W25Q16_CS_Pin F_CS_Pin
#define W25Q16_CS_GPIO_Port F_CS_GPIO_Port

extern float PressureData[300];
extern float TemperatureData[300];

unsigned char loadflash_ID[10];
unsigned char flash_load_index;
extern struct data_format load_data;
float loadPressureData[300];
float loadTemperatureData[300];

struct RFID_format loadRFIDData;

unsigned int loadTotalTime;
unsigned int loadfProcessTime[7];

unsigned int loadtotalCount;
unsigned int loaddailyCount;

unsigned int loadTotalTime;
unsigned int loadfProcessTime[7];

unsigned int loadCarbonFilter;
unsigned int loadHEPAFilter;
unsigned int loadPlasmaAssy;

extern unsigned int inputyear, inputmonth, inputday;

unsigned int temptotalcycle[6];


#define HISTORY_FIRST_DATA	10//[240]


unsigned char read_data[1200]={};
unsigned char write_data[1200]={};
unsigned char write_setting_data[1200]={};
unsigned char read_setting_data[1200]={};
/*섹터 별 저장*/
void Write_Setting_Data_Flash(){
	DisplayPage(LCD_LOADING_PAGE);

	/* Enable W25Q16 */
	//HAL_GPIO_WritePin(W25Q16_CS_GPIO_Port, W25Q16_CS_Pin, GPIO_PIN_RESET);


	write_setting_data[2]=10;
	if(totalCount>=500){
		totalCount=1;
	}
	if(totalCount==0){
		write_setting_data[11]=0;
		write_setting_data[12]=0;
		write_setting_data[13]=0;	//년
		write_setting_data[14]=0;	//월
		write_setting_data[15]=0;	//일
	}
	else{
		write_setting_data[11+5*(totalCount-1)]=(int)totalCount/100;
		write_setting_data[12+5*(totalCount-1)]=(int)totalCount%100;
		write_setting_data[13+5*(totalCount-1)]=bcd2bin(p_data.year);	//년
		write_setting_data[14+5*(totalCount-1)]=bcd2bin(p_data.month);	//월
		write_setting_data[15+5*(totalCount-1)]=bcd2bin(p_data.day);	//일
	}

	/* Write data to W25Q16 */
	uint32_t write_address =1000;
	W25Q16_WritePage(write_address, write_setting_data,1200);


	/* Disable W25Q16 */
	//HAL_GPIO_WritePin(W25Q16_CS_GPIO_Port, W25Q16_CS_Pin, GPIO_PIN_SET);
}

void Read_Setting_Data_Flash(){
	DisplayPage(LCD_LOADING_PAGE);

	/* Enable W25Q16 */
	//HAL_GPIO_WritePin(W25Q16_CS_GPIO_Port, W25Q16_CS_Pin, GPIO_PIN_RESET);


	uint32_t read_address = 1000;
	W25Q16_ReadData(read_address, read_setting_data, 1200);
	for(int i=0;i<1200;i++){
		if(read_setting_data[i]==0xff){
			read_setting_data[i]=0;
		}
	}
	for(int i=0;i<6;i++){
		temptotalcycle[i]=0;
	}

	int j=1;
	if(totalCount==0){

	}
	else{
		for(int i=0;i<totalCount;i++){
			if(is_same_date(inputyear, inputmonth, inputday, read_setting_data[13+5*i], read_setting_data[14+5*i], read_setting_data[15+5*i])==1){
				temptotalcycle[j]=read_setting_data[11+5*i]*100+read_setting_data[12+5*i];
				j++;
				if(j>=6){
					j=1;
				}
			}
			else{

			}
		}

	}


	/* Disable W25Q16 */
	//HAL_GPIO_WritePin(W25Q16_CS_GPIO_Port, W25Q16_CS_Pin, GPIO_PIN_SET);
}

void Write_Data_Flash(){
	DisplayPage(LCD_LOADING_PAGE);

	/* Enable W25Q16 */
	//HAL_GPIO_WritePin(W25Q16_CS_GPIO_Port, W25Q16_CS_Pin, GPIO_PIN_RESET);

	for(int i=0;i<10;i++){
		write_data[HISTORY_FIRST_DATA+i]=flash_ID[CurrentUser][i];
	}
	write_data[11]=RFIDData.production_year;
	write_data[12]=RFIDData.production_month;
	write_data[13]=RFIDData.production_day;
	write_data[14]=RFIDData.production_number;

	write_data[15]=RFIDData.open_year;
	write_data[16]=RFIDData.open_month;
	write_data[17]=RFIDData.open_day;

	write_data[18]=RFIDData.volume;
	write_data[19]=RFIDData.elapsed_days;

	//TotalTime
	/*
	int tempminute,tempsecond;
	tempminute=(TotalTime/100)/60;
	tempsecond=(TotalTime/100)%60;
	write_data[20]=tempminute;
	write_data[21]=tempsecond;

	//fProcessTime	22~43
	for(int i=0;i<6;i++){
		tempminute=(fProcessTime[1+i]/100)/60;
		tempsecond=(fProcessTime[1+i]/100)%60;
		write_data[22+2*i]=tempminute;
		write_data[23+2*i]=tempsecond;
	}
	 */
	write_data[44]=(int)totalCount/100;
	write_data[45]=(int)totalCount%100;
	write_data[46]=dailyCount;


	write_data[47]=(int)CarbonFilter/100;
	write_data[48]=(int)CarbonFilter%100;

	write_data[49]=(int)HEPAFilter/100;
	write_data[50]=(int)HEPAFilter%100;

	write_data[51]=(int)PlasmaAssy/100;
	write_data[52]=(int)PlasmaAssy%100;

	write_data[61]=p_data.cyclename;
	write_data[62]=p_data.year;
	write_data[63]=p_data.month;
	write_data[64]=p_data.day;

	write_data[65]=p_data.start_hour;
	write_data[66]=p_data.start_minute;
	write_data[67]=p_data.start_second;

	write_data[68]=p_data.end_hour;
	write_data[69]=p_data.end_minute;
	write_data[70]=p_data.end_second;

	write_data[71]=p_data.status;

	for(int i=0;i<6;i++){
		write_data[72+6*i]=(int)((p_data.tempmax[i+1]*10)/100);
		write_data[73+6*i]=((int)(p_data.tempmax[i+1]*10)%100);

		write_data[74+6*i]=(int)((p_data.pressuremax[i+1]*10)/100);
		write_data[75+6*i]=((int)(p_data.pressuremax[i+1]*10)%100);

		write_data[76+6*i]=(int)((p_data.pressuremin[i+1]*10)/100);
		write_data[77+6*i]=((int)(p_data.pressuremin[i+1]*10)%100);
	}

	for(int i=0;i<270;i++){
		write_data[115+4*i]=(int)((TemperatureData[i+1]*10)/100);
		write_data[116+4*i]=((int)(TemperatureData[i+1]*10)%100);

		write_data[117+4*i]=(int)((PressureData[i+1]*10)/100);
		write_data[118+4*i]=((int)(PressureData[i+1]*10)%100);
	}

	/* Write data to W25Q16 */
	uint32_t write_address =totalCount * 4096;
	W25Q16_WritePage(write_address, write_data, 1200);

	/* Disable W25Q16 */
	//HAL_GPIO_WritePin(W25Q16_CS_GPIO_Port, W25Q16_CS_Pin, GPIO_PIN_SET);
}

void Read_Data_Flash(){
	DisplayPage(LCD_LOADING_PAGE);
	/* Enable W25Q16 */
	//HAL_GPIO_WritePin(W25Q16_CS_GPIO_Port, W25Q16_CS_Pin, GPIO_PIN_RESET);



	/* Read data from W25Q16 */
/*
	unsigned char read_setting_data[10]={};

	W25Q16_ReadData(0, read_setting_data, strlen((char*)read_setting_data));
	flash_save_index=read_setting_data[0];
*/
//	unsigned char read_data[1200]={};

	uint32_t read_address = flash_load_index * 4096;
	W25Q16_ReadData(read_address, read_data, 1200);

	for(int i=0;i<1200;i++){
		if(read_data[i]==0xff){
			read_data[i]=0;
		}
	}

	for(int i=0;i<10;i++){
		loadflash_ID[i]=read_data[i];
	}



	loadRFIDData.production_year=read_data[11];
	loadRFIDData.production_month=read_data[12];
	loadRFIDData.production_day=read_data[13];
	loadRFIDData.production_number=read_data[14];

	loadRFIDData.open_year=read_data[15];
	loadRFIDData.open_month=read_data[16];
	loadRFIDData.open_day=read_data[17];

	loadRFIDData.volume=read_data[18];

	/*
	 - 날짜
	 - total cycle
	 - daily cycle
	 - total time
	 - cycle time

	 - caborn filter
	 - hepa filter
	 - plasma assy

	 */


	//TotalTime
	loadTotalTime=(read_data[20]*100)*60+(read_data[21]*100);

	//fProcessTime	22~43
	for(int i=0;i<6;i++){
		loadfProcessTime[1+i]=(read_data[22+2*i]*100)*60+(read_data[23+2*i]*100);
	}

	loadtotalCount=read_data[44]*100+read_data[45];
	loaddailyCount=read_data[46];
	loadCarbonFilter=read_data[47]*100+read_data[48];
	loadCarbonFilter=read_data[49]*100+read_data[50];
	loadPlasmaAssy=read_data[51]*100+read_data[52];




	load_data.cyclename=read_data[61];
	load_data.year=read_data[62];
	load_data.month=read_data[63];
	load_data.day=read_data[64];

	load_data.start_hour=read_data[65];
	load_data.start_minute=read_data[66];
	load_data.start_second=read_data[67];

	load_data.end_hour=read_data[68];
	load_data.end_minute=read_data[69];
	load_data.end_second=read_data[70];

	load_data.status=read_data[71];


	for(int i=0;i<6;i++){
		load_data.tempmax[i+1]=(read_data[72+6*i]*10)+(read_data[73+6*i]/10);
		load_data.pressuremax[i+1]=(read_data[74+6*i]*10)+(read_data[75+6*i]/10);
		load_data.pressuremin[i+1]=(read_data[76+6*i]*10)+(read_data[77+6*i]/10);
	}

	for(int i=0;i<270;i++){
		loadTemperatureData[i]=(read_data[115+4*i]*10)+(read_data[116+4*i]/10);
		loadPressureData[i]=(read_data[117+4*i]*10)+(read_data[118+4*i]/10);
	}

	/* Disable W25Q16 */
	//HAL_GPIO_WritePin(W25Q16_CS_GPIO_Port, W25Q16_CS_Pin, GPIO_PIN_SET);
}


void read_extern_flash(void){
	/* Enable W25Q16 */
	HAL_GPIO_WritePin(W25Q16_CS_GPIO_Port, W25Q16_CS_Pin, GPIO_PIN_RESET);

	/* Erase W25Q16 chip */
	W25Q16_EraseChip();

	/* Write data to W25Q16 */
	uint8_t write_data[] = "Hello, world!";
	uint32_t write_address = 0;
	W25Q16_WritePage(write_address, write_data, strlen((char*)write_data));

	/* Read data from W25Q16 */
	uint8_t read_data[16];
	uint32_t read_address = 0;
	W25Q16_ReadData(read_address, read_data, sizeof(read_data));

	/* Disable W25Q16 */
	HAL_GPIO_WritePin(W25Q16_CS_GPIO_Port, W25Q16_CS_Pin, GPIO_PIN_SET);
}

void Erase_flash(void){
	DisplayPage(LCD_LOADING_PAGE);
	/* Enable W25Q16 */
	HAL_GPIO_WritePin(W25Q16_CS_GPIO_Port, W25Q16_CS_Pin, GPIO_PIN_RESET);

	/* Erase W25Q16 chip */
	W25Q16_EraseChip();

	/* Disable W25Q16 */
	HAL_GPIO_WritePin(W25Q16_CS_GPIO_Port, W25Q16_CS_Pin, GPIO_PIN_SET);
}


/* Send a byte of data to W25Q16 via SPI */
uint8_t W25Q16_SendByte(uint8_t data){
	uint8_t rx_data;
	//HAL_SPI_TransmitReceive(&hspi1, &data, &rx_data, 1, HAL_MAX_DELAY);
	return rx_data;
}

/* Send W25Q16 command to enable write operations */
void W25Q16_WriteEnable(void){
	HAL_GPIO_WritePin(W25Q16_CS_GPIO_Port, W25Q16_CS_Pin, GPIO_PIN_RESET);
	W25Q16_SendByte(W25Q16_CMD_WRITE_ENABLE);
	HAL_GPIO_WritePin(W25Q16_CS_GPIO_Port, W25Q16_CS_Pin, GPIO_PIN_SET);
}

/* Send W25Q16 command to disable write operations */
void W25Q16_WriteDisable(void){
	HAL_GPIO_WritePin(W25Q16_CS_GPIO_Port, W25Q16_CS_Pin, GPIO_PIN_RESET);
	W25Q16_SendByte(W25Q16_CMD_WRITE_DISABLE);
	HAL_GPIO_WritePin(W25Q16_CS_GPIO_Port, W25Q16_CS_Pin, GPIO_PIN_SET);
}

/* Read status register 1 of W25Q16 */
uint8_t W25Q16_ReadStatusReg1(void){
	uint8_t status_reg1;
	HAL_GPIO_WritePin(W25Q16_CS_GPIO_Port, W25Q16_CS_Pin, GPIO_PIN_RESET);
	W25Q16_SendByte(W25Q16_CMD_READ_STATUS_REG1);
	status_reg1 = W25Q16_SendByte(0xFF);
	HAL_GPIO_WritePin(W25Q16_CS_GPIO_Port, W25Q16_CS_Pin, GPIO_PIN_SET);
	return status_reg1;
}

/* Wait until W25Q16 is ready for next operation */
void W25Q16_WaitForReady(void){
	while ((W25Q16_ReadStatusReg1() & W25Q16_STATUS_BUSY) == W25Q16_STATUS_BUSY);
}

/* Erase a sector of W25Q16 */
void W25Q16_EraseSector(uint32_t sector_address){
	W25Q16_WriteEnable();
	HAL_GPIO_WritePin(W25Q16_CS_GPIO_Port, W25Q16_CS_Pin, GPIO_PIN_RESET);
	W25Q16_SendByte(W25Q16_CMD_ERASE_SECTOR);
	W25Q16_SendByte((sector_address >> 16) & 0xFF);
	W25Q16_SendByte((sector_address >> 8) & 0xFF);
	W25Q16_SendByte(sector_address & 0xFF);
	HAL_GPIO_WritePin(W25Q16_CS_GPIO_Port, W25Q16_CS_Pin, GPIO_PIN_SET);
	W25Q16_WaitForReady();
}

/* Erase the entire W25Q16 chip */
void W25Q16_EraseChip(void){
	W25Q16_WriteEnable();
	HAL_GPIO_WritePin(W25Q16_CS_GPIO_Port, W25Q16_CS_Pin, GPIO_PIN_RESET);
	W25Q16_SendByte(W25Q16_CMD_ERASE_CHIP);
	HAL_GPIO_WritePin(W25Q16_CS_GPIO_Port, W25Q16_CS_Pin, GPIO_PIN_SET);
	W25Q16_WaitForReady();
}

/* Write data to a page of W25Q16 */
void W25Q16_WritePage(uint32_t address, uint8_t* data, uint16_t length){
	/* Make sure the length of data is within a single page */
	if (length > 256) {
		length = 256;
	}
	W25Q16_WriteEnable();
	HAL_GPIO_WritePin(W25Q16_CS_GPIO_Port, W25Q16_CS_Pin, GPIO_PIN_RESET);
	W25Q16_SendByte(W25Q16_CMD_PAGE_PROGRAM);
	W25Q16_SendByte((address >> 16) & 0xFF);
	W25Q16_SendByte((address >> 8) & 0xFF);
	W25Q16_SendByte(address & 0xFF);
	for (uint16_t i = 0; i < length; i++) {
		W25Q16_SendByte(data[i]);
	}
	HAL_GPIO_WritePin(W25Q16_CS_GPIO_Port, W25Q16_CS_Pin, GPIO_PIN_SET);
	W25Q16_WaitForReady();
}


/* Read data from W25Q16 */
void W25Q16_ReadData(uint32_t address, uint8_t* data, uint16_t length){
	HAL_GPIO_WritePin(W25Q16_CS_GPIO_Port, W25Q16_CS_Pin, GPIO_PIN_RESET);
	W25Q16_SendByte(W25Q16_CMD_READ_DATA);
	W25Q16_SendByte((address >> 16) & 0xFF);
	W25Q16_SendByte((address >> 8) & 0xFF);
	W25Q16_SendByte(address & 0xFF);
	for (uint16_t i = 0; i < length; i++) {
		data[i] = W25Q16_SendByte(0xFF);
	}
	HAL_GPIO_WritePin(W25Q16_CS_GPIO_Port, W25Q16_CS_Pin, GPIO_PIN_SET);
}

int is_same_date(uint8_t year1, uint8_t month1, uint8_t day1, uint8_t year2, uint8_t month2, uint8_t day2) {
    if (year1 == year2 && month1 == month2 && day1 == day2) {
        return 1; // 두 날짜가 같습니다.
    } else {
        return 0; // 두 날짜가 다릅니다.
    }
}
