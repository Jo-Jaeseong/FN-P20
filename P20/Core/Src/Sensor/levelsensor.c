/*
 * liquidlevel.c
 *
 *  Created on: Nov 10, 2022
 *      Author: CBT
 */
#include "main.h"
#include "levelsensor.h"

#define LevelSensor1_Port	GPIO_IN6_GPIO_Port
#define LevelSensor1_Pin	GPIO_IN6_Pin

#define LevelSensor2_Port	GPIO_IN7_GPIO_Port
#define LevelSensor2_Pin	GPIO_IN7_Pin

int LevelSensor1Check(){
	int LiquidSensor_flag=0;
	if(HAL_GPIO_ReadPin(LevelSensor1_Port, LevelSensor1_Pin)==1){
		LiquidSensor_flag=1;
	}
	else{
		LiquidSensor_flag=0;
	}
	return LiquidSensor_flag;
}

int LevelSensor2Check(){
	int LiquidSensor_flag=0;
	if(HAL_GPIO_ReadPin(LevelSensor2_Port, LevelSensor2_Pin)==1){
		LiquidSensor_flag=1;
	}
	else{
		LiquidSensor_flag=0;
	}
	return LiquidSensor_flag;
}
