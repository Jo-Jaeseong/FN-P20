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
	if(HAL_GPIO_ReadPin(LevelSensor1_Port, LevelSensor1_Pin)==1){
		return 1;
	}
	else{
		return 0;
	}
}

int LevelSensor2Check(){
	if(HAL_GPIO_ReadPin(LevelSensor2_Port, LevelSensor2_Pin)==1){
		return 1;
	}
	else{
		return 0;
	}
}
