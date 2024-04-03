/*
 * door_sensor.c
 *
 *  Created on: Nov 10, 2022
 *      Author: CBT
 */

#include "main.h"
#include "doorsensor.h"
#include "sensor.h"

// ------------------- Variables -------------------
#define DoorHandle_Port	GPIO_IN5_GPIO_Port
#define DoorHandle_Pin	GPIO_IN5_Pin

extern int DoorLatch_flag;
int DoorCheckcnt=0;

// ------------------- Functions -------------------


int DoorHandleCheck(){
	int DoorHandle_flag=0;
	if(HAL_GPIO_ReadPin(DoorHandle_Port, DoorHandle_Pin)==1){
		DoorHandle_flag=0;
	}
	else{
		DoorHandle_flag=1;
	}
	return DoorHandle_flag;
}


int DoorLatchCheck(){
	return Limit1();
	//return ValveCheck();	//테스트
}

int BottleDoorCheck(){
	return Limit2;
}

int BottleCheck(){
	return Limit3;
}



int DoorOpenProcess(){
	if(DoorHandleCheck()){
		DoorCheckcnt++;
	}
	else{
		DoorCheckcnt=0;
	}
	if(DoorCheckcnt==2){
		if(DoorLatchCheck()){
			return 1;
			DoorCheckcnt=0;
		}
		else{
			return 0;
		}
	}
	else{
		return 0;
	}
}
