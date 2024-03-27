/*
 * plasma.c
 *
 *  Created on: Nov 10, 2022
 *      Author: CBT
 */

#include "main.h"
#include "hardware.h"

// ------------------- Variables -------------------
int PlasmaTime[2];
int PlasmaOnTime=1;
int PlasmaOffTime=0;
int PlasmaControl_flag=0;

// ------------------- Functions -------------------
void Plasma(int onoff){
	AC7(onoff);
}

void PlasmaControlSetting(){
	PlasmaOnTime=PlasmaTime[0]*10;
	PlasmaOffTime=PlasmaTime[1]*10;
}

void PlasmaControl(){
	if(PlasmaOnTime<=0){
		Plasma(0);
		if(PlasmaOffTime<=0){
			Plasma(1);
			PlasmaOnTime=PlasmaTime[0]*10;
		}
		else{
			PlasmaOffTime--;
		}
	}
	else{
		Plasma(1);
		PlasmaOnTime--;
		PlasmaOffTime=PlasmaTime[1]*10;
	}

}
