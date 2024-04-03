/*
 * sensor.c
 *
 *  Created on: Mar 19, 2024
 *      Author: CBT_DEV
 */

#include "main.h"

#include "doorsensor.h"
#include "levelsensor.h"
#include "temperature.h"
#include "vacuumsensor.h"
#include "valvesensor.h"

#include "sensor.h"

#define Limit1_Port		GPIO_IN1_GPIO_Port
#define Limit1_Pin		GPIO_IN1_Pin

#define Limit2_Port		GPIO_IN2_GPIO_Port
#define Limit2_Pin		GPIO_IN2_Pin

#define Limit3_Port		GPIO_IN3_GPIO_Port
#define Limit3_Pin		GPIO_IN3_Pin

#define Limit4_Port		GPIO_IN4_GPIO_Port
#define Limit4_Pin		GPIO_IN4_Pin


int Limit1(){
	if(HAL_GPIO_ReadPin(Limit1_Port, Limit1_Pin)==1){
		return 0;
	}
	else{
		return 1;
	}
}

int Limit2(){
	if(HAL_GPIO_ReadPin(Limit2_Port, Limit2_Pin)==1){
		return 0;
	}
	else{
		return 1;
	}
}

int Limit3(){
	if(HAL_GPIO_ReadPin(Limit3_Port, Limit3_Pin)==1){
		return 0;
	}
	else{
		return 1;
	}
}

int Limit4(){
	if(HAL_GPIO_ReadPin(Limit4_Port, Limit4_Pin)==1){
		return 0;
	}
	else{
		return 1;
	}
}
