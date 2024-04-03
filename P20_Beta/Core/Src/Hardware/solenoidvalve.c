/*
 * solenoidvalve.c
 *
 *  Created on: Nov 10, 2022
 *      Author: CBT
 */

#include "main.h"
#include "hardware.h"

// ------------------- Variables -------------------

// ------------------- Functions -------------------
/*
 * 솔레노이드 벨브 5가지
 *
 * 진공 - VacuumValve - GPIO_OUT1(DC)
 * 배기 - VentValve - GPIO_OUT4(DC)
 * 과수 - InjectionValve - GPIO_OUT5(DC)
 *
 */

//VacuumValve
void VacuumValve(int onoff){
	DC1(onoff);
}

void CirculationValve(int onoff){
	DC2(onoff);
}

void InhalationValve(int onoff){
	DC3(onoff);
}

//VentValve
void VentValve(int onoff){
	DC4(onoff);
}

//InjectionValve
void InjectionValve(int onoff){
	DC5(onoff);
}
