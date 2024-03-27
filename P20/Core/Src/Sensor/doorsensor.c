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
#define DoorLatch_Port	GPIO_IN1_GPIO_Port
#define DoorLatch_Pin	GPIO_IN1_Pin

#define BottleSW_Port	GPIO_IN2_GPIO_Port
#define BottleSW_Pin	GPIO_IN2_Pin

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
	//return Limit1();
	return ValveCheck();	//테스트
}

int BottleCheck(){
	int BottleSW_flag=0;
	if(HAL_GPIO_ReadPin(BottleSW_Port, BottleSW_Pin)==1){
		BottleSW_flag=0;
	}
	else{
		BottleSW_flag=1;
	}
	return BottleSW_flag;
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
