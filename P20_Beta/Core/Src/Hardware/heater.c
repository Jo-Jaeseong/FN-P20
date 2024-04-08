/*
 * heater.c
 *
 *  Created on: Nov 10, 2022
 *      Author: CBT
 */
#include "main.h"
#include "sensor.h"
#include "hardware.h"
#include "peripheral.h"

int temperror[5]={};

int TestTempErrorValue=1;


/*
 * 히터 5가지
 *
 * 도어   - DoorHeater
 * 챔버1 - ChamberHeater
 * 챔버2 - ChamberHeater2
 * 기화기 - VaporizerHeater
 * 순환   - CirculationHeater
 *
 */
// ------------------- Variables -------------------


#define DoorHeater_Port GPIO_OUT11_GPIO_Port
#define DoorHeater_Pin GPIO_OUT11_Pin

#define ChamberHeater_Port GPIO_OUT8_GPIO_Port
#define ChamberHeater_Pin GPIO_OUT8_Pin

#define ChamberHeater2_Port GPIO_OUT13_GPIO_Port
#define ChamberHeater2_Pin GPIO_OUT13_Pin

#define VaporizerHeater_Port GPIO_OUT14_GPIO_Port
#define VaporizerHeater_Pin GPIO_OUT14_Pin

#define CirculationHeater_Port GPIO_OUT15_GPIO_Port
#define CirculationHeater_Pin GPIO_OUT15_Pin


// ------------------- Functions -------------------

void DoorHeater(int onoff){
	AC1(onoff);
}

void ChamberHeater(int onoff){
	DC8(onoff);
}

void ChamberBackHeater(int onoff){
	AC3(onoff);
}

void VaporizerHeater(int onoff){
	AC4(onoff);
}




void CirculationHeater(int onoff){
	AC5(onoff);
}




void DoorHeaterControl(int Temp){
	//컨트롤 하는 방식 고민
	//얼마에 한번씩 체크 할지 생각
	if(Temperature[0]>=Temp){
		DoorHeater(0);
	}
	else{
		DoorHeater(1);
	}
}

void ChamberHeaterControl(int Temp){
	//컨트롤 하는 방식 고민
	//얼마에 한번씩 체크 할지 생각
	if(Temperature[1]>=Temp){
		ChamberHeater(0);
	}
	else{
		ChamberHeater(1);
	}
}

void ChamberBackHeaterControl(int Temp){
	//컨트롤 하는 방식 고민
	//얼마에 한번씩 체크 할지 생각
	if(Temperature[2]>=Temp){
		ChamberBackHeater(0);
	}
	else{
		ChamberBackHeater(1);
	}
}


void VaporizerHeaterControl(int Temp){
	//컨트롤 하는 방식 고민
	//얼마에 한번씩 체크 할지 생각
	if(Temperature[3]>=Temp){
		VaporizerHeater(0);
	}
	else{
		VaporizerHeater(1);
	}
}

void HeaterControl(){

	switch (HeaterControlMode) {
		case 0:	//컨트롤 안함
			break;
		case 1:	//대기
			DoorHeaterControl(DoorSettingTemp[0]);
			ChamberHeaterControl(ChamberSettingTemp[0]);
			ChamberBackHeaterControl(ChamberBackSettingTemp[0]);
			VaporizerHeaterControl(VaporizerSettingTemp[0]);
			break;

		case 2:	//공정
			DoorHeaterControl(DoorSettingTemp[1]);
			ChamberHeaterControl(ChamberSettingTemp[1]);
			ChamberBackHeaterControl(ChamberBackSettingTemp[1]);
			VaporizerHeaterControl(VaporizerSettingTemp[1]);
			break;

		case 3:	//슬립
			DoorHeaterControl(DoorSettingTemp[2]);
			ChamberHeaterControl(ChamberSettingTemp[2]);
			ChamberBackHeaterControl(ChamberBackSettingTemp[2]);
			VaporizerHeaterControl(VaporizerSettingTemp[2]);
			break;
		default:
			break;
	}
}

void HeatingTest(){
	if((DoorSettingTemp[0]-TestTempErrorValue)>Temperature[0]){
		temperror[0]=1;
		temperror[4]=1;
	}

	if((ChamberSettingTemp[0]-TestTempErrorValue)>Temperature[1]){
		temperror[1]=1;
		temperror[4]=1;
	}

	if((ChamberBackSettingTemp[0]-TestTempErrorValue)>Temperature[2]){
		temperror[2]=1;
		temperror[4]=1;
	}
	if((VaporizerSettingTemp[0]-TestTempErrorValue)>Temperature[3]){
		temperror[3]=1;
		temperror[4]=1;
	}
}

int ChamberHeaterCheck(){
	if((ChamberSettingTemp[1]-TestTempErrorValue)>Temperature[1]){
		return 1;
	}
	else{
		return 0;
	}
}
int VaporizerHeaterCheck(){
	if((VaporizerSettingTemp[1]-TestTempErrorValue)>Temperature[3]){
		return 1;
	}
	else{
		return 0;
	}
}
