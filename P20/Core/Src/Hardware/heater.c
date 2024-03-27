/*
 * heater.c
 *
 *  Created on: Nov 10, 2022
 *      Author: CBT
 */
#include "main.h"
#include "heater.h"
#include "hardware.h"
#include "sensor.h"

// ------------------- Variables -------------------
int temperror[5]={};
int TestTempErrorValue=1;



// ------------------- Functions -------------------

/*
 * 히터 5가지
 *
 * 도어   - DoorHeater
 * 챔버1 - ChamberHeater
 * 챔버2 - ChamberbackHeater
 * 기화기 - VaporizerHeater
 *
 */
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



void DoorHeaterControl(int Temp){
	if(Temperature[0]>=Temp){
		DoorHeater(0);
	}
	else{
		DoorHeater(1);
	}
}

void ChamberHeaterControl(int Temp){
	if(Temperature[1]>=Temp){
		ChamberHeater(0);
	}
	else{
		ChamberHeater(1);
	}
}

void ChamberBackHeaterControl(int Temp){
	if(Temperature[2]>=Temp){
		ChamberBackHeater(0);
	}
	else{
		ChamberBackHeater(1);
	}
}


void VaporizerHeaterControl(int Temp){
	if(Temperature[3]>=Temp){
		VaporizerHeater(0);
	}
	else{
		VaporizerHeater(1);
	}
}

void HeaterControl(){
	//DoorHeaterControl
	if(DoorHeater_flag==State1){			//대기
		DoorHeaterControl(DoorSettingTemp[State1]);
	}
	else if(DoorHeater_flag==State2){	//공정
		DoorHeaterControl(DoorSettingTemp[State2]);
	}
	else{									//슬립
		DoorHeaterControl(DoorSettingTemp[State3]);
	}

	//ChamberHeaterControl
	if(ChamberHeater_flag==State1){
		ChamberHeaterControl(ChamberSettingTemp[State1]);
	}
	else if(ChamberHeater_flag==State2){
		ChamberHeaterControl(ChamberSettingTemp[State1]);
	}
	else{
		ChamberHeaterControl(ChamberSettingTemp[State3]);
	}

	//ChamberBackHeaterControl
	if(ChamberBackHeater_flag==State1){
		ChamberBackHeaterControl(ChamberBackSettingTemp[State1]);
	}
	else if(ChamberBackHeater_flag==State2){
		ChamberBackHeaterControl(ChamberBackSettingTemp[State2]);
	}
	else{
		ChamberBackHeaterControl(ChamberBackSettingTemp[State3]);
	}

	//VaporizerHeaterControl
	if(VaporizerHeater_flag==State1){
		VaporizerHeaterControl(VaporizerSettingTemp[State1]);
	}
	else if(VaporizerHeater_flag==State2){
		VaporizerHeaterControl(VaporizerSettingTemp[State2]);
	}
	else{
		VaporizerHeaterControl(VaporizerSettingTemp[State3]);
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

