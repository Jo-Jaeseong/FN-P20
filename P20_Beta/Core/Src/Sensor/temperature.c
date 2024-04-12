/*
 * temperature.c
 *
 *  Created on: 2022. 10. 20.
 *      Author: CBT
 */

#include "main.h"
#include "temperature.h"
#include "lcd.h"
// ------------------- Variables -------------------

extern SPI_HandleTypeDef hspi2;
#define MAX6675_SPI	&hspi2

#define MAX6675_OK		1
#define MAX6675_ERROR	0

#define TEMP1_Port GPIO_OUT19_GPIO_Port
#define TEMP1_Pin GPIO_OUT19_Pin

#define TEMP2_Port GPIO_OUT20_GPIO_Port
#define TEMP2_Pin GPIO_OUT20_Pin

#define TEMP3_Port GPIO_OUT21_GPIO_Port
#define TEMP3_Pin GPIO_OUT21_Pin

#define TEMP4_Port GPIO_OUT22_GPIO_Port
#define TEMP4_Pin GPIO_OUT22_Pin

#define TEMP5_Port GPIO_OUT23_GPIO_Port
#define TEMP5_Pin GPIO_OUT23_Pin

float Temperature[5];
int DoorSettingTemp[3], ChamberSettingTemp[3], ChamberBackSettingTemp[3], VaporizerSettingTemp[3];
int CalibrationTemp[4];

// ------------------- Functions -------------------

uint8_t max6675ReadReg(uint16_t *reg, int Channel)
{
	HAL_StatusTypeDef hal_answer = HAL_ERROR;

	uint8_t temp[2] = {0};
	//__disable_irq();
	if(Channel==0){
		HAL_GPIO_WritePin(TEMP1_Port, TEMP1_Pin, GPIO_PIN_RESET);
		//HAL_Delay(1);
		hal_answer = HAL_SPI_Receive(MAX6675_SPI, temp, 2, 100);
		HAL_GPIO_WritePin(TEMP1_Port, TEMP1_Pin, GPIO_PIN_SET);
	}

	else if(Channel==1){
		HAL_GPIO_WritePin(TEMP2_Port, TEMP2_Pin, GPIO_PIN_RESET);
		//HAL_Delay(1);
		hal_answer = HAL_SPI_Receive(MAX6675_SPI, temp, 2, 100);
		HAL_GPIO_WritePin(TEMP2_Port, TEMP2_Pin, GPIO_PIN_SET);
	}
	else if(Channel==2){
		HAL_GPIO_WritePin(TEMP3_Port, TEMP3_Pin, GPIO_PIN_RESET);
		//HAL_Delay(1);
		hal_answer = HAL_SPI_Receive(MAX6675_SPI, temp, 2, 100);
		HAL_GPIO_WritePin(TEMP3_Port, TEMP3_Pin, GPIO_PIN_SET);
	}
	else if(Channel==3){
		HAL_GPIO_WritePin(TEMP4_Port, TEMP4_Pin, GPIO_PIN_RESET);
		//HAL_Delay(1);
		hal_answer = HAL_SPI_Receive(MAX6675_SPI, temp, 2, 100);
		HAL_GPIO_WritePin(TEMP4_Port, TEMP4_Pin, GPIO_PIN_SET);
	}
	else if(Channel==4){
		HAL_GPIO_WritePin(TEMP5_Port, TEMP5_Pin, GPIO_PIN_RESET);
		//HAL_Delay(1);
		hal_answer = HAL_SPI_Receive(MAX6675_SPI, temp, 2, 100);
		HAL_GPIO_WritePin(TEMP5_Port, TEMP5_Pin, GPIO_PIN_SET);
	}
	else{

	}
	//__enable_irq();
	if(hal_answer == HAL_OK)
	{
		if(temp[1] & 0x04) return MAX6675_ERROR;

		*reg = (uint16_t)(temp[1]>>3);
		*reg |=(uint16_t)(temp[0]<<5);

		return MAX6675_OK;
	}
	return MAX6675_ERROR;
}

float max6675Temp(uint16_t reg)
{
	return reg*0.25;
}
void Check_Temp(int Channel){	//SPI통신 - 모듈 온도 측정
	uint8_t answer = 0;
	uint16_t reg = 0;
	float ErrorCheckTemp=0;

	answer = max6675ReadReg(&reg,Channel);
	if(answer == MAX6675_OK){
		ErrorCheckTemp=max6675Temp(reg);
		Temperature[Channel]=ErrorCheckTemp;
		Temperature[Channel]=Temperature[Channel]+CalibrationTemp[Channel]-10;
	}
	else{

	}

	switch(Channel){
		case 0 :
			DisplayPageValue(0x31,0x01,Temperature[0]*10);
			break;
		case 1 :
			DisplayPageValue(0x31,0x05,Temperature[1]*10);
			break;
		case 2 :
			DisplayPageValue(0x31,0x09,Temperature[2]*10);
			break;
		case 3 :
			DisplayPageValue(0x31,0x0D,Temperature[3]*10);
			break;
	}
}

/*
#define Offmode			0
#define Onmode			1
#define Processmode		2
#define Sleepmode		3
*/

void TempSetting(){
	DoorSettingTemp[0]=0;
	DoorSettingTemp[1]=0;
	DoorSettingTemp[2]=0;

	ChamberSettingTemp[0]=0;
	ChamberSettingTemp[1]=0;
	ChamberSettingTemp[2]=0;

	ChamberBackSettingTemp[0]=0;
	ChamberBackSettingTemp[1]=0;
	ChamberBackSettingTemp[2]=0;

	VaporizerSettingTemp[0]=0;
	VaporizerSettingTemp[1]=0;
	VaporizerSettingTemp[2]=0;
}
/*
void TempSetting(){
	DoorSettingTemp[0]=0;	//0
	DoorSettingTemp[1]=CycleData[ProcessNum][StepNum].Temp[0]+10;;	//60
	if(DoorSettingTemp[1]==0){
		DoorSettingTemp[1]=CycleData[1][1].Temp[0]+10;
	}
	DoorSettingTemp[2]=CycleData[1][1].Temp[0]+10;	//60
	DoorSettingTemp[3]=10;	//55

	ChamberSettingTemp[0]=0;	//0
	ChamberSettingTemp[1]=CycleData[ProcessNum][StepNum].Temp[0];;	//60
	if(ChamberSettingTemp[1]==0){
		ChamberSettingTemp[1]=CycleData[1][1].Temp[0];
	}
	ChamberSettingTemp[2]=CycleData[1][1].Temp[0];	//60
	ChamberSettingTemp[3]=10;	//55

	ChamberBackSettingTemp[0]=0;	//0
	ChamberBackSettingTemp[1]=CycleData[ProcessNum][StepNum].Temp[0];;	//60
	if(ChamberBackSettingTemp[1]==0){
		ChamberBackSettingTemp[1]=CycleData[1][1].Temp[0];
	}
	ChamberBackSettingTemp[2]=CycleData[1][1].Temp[0];	//60
	ChamberBackSettingTemp[3]=10;	//55

	VaporizerSettingTemp[0]=0;		//0
	VaporizerSettingTemp[1]=CycleData[ProcessNum][StepNum].Temp[1];;		//80
	if(VaporizerSettingTemp[1]==0){
		VaporizerSettingTemp[1]=CycleData[1][1].Temp[0];
	}
	VaporizerSettingTemp[2]=CycleData[1][1].Temp[1];	//140
	VaporizerSettingTemp[3]=10;		//80
}
*/
