/*
 * hardware.h
 *
 *  Created on: Nov 10, 2022
 *      Author: CBT
 */

#ifndef SRC_HARDWARE_HARDWARE_H_
#define SRC_HARDWARE_HARDWARE_H_

#include "doorlatch.h"
#include "fan.h"
#include "heater.h"
#include "peristalticpump.h"
#include "plasma.h"
#include "solenoidvalve.h"
#include "vacuumpump.h"

extern int DoorHeater_flag, AC2_flag, ChamberBackHeater_flag, VaporizerHeater_flag, AC5_flag, VacuumPump_flag, Plasma_flag, AC8_flag;	//AC
extern int VacuumValve_flag, DC2_flag, DC3_flag, VentValve_flag, InjectionValve_flag, DoorLatch_flag, Fan_flag, ChamberHeater_flag, PeristalticPump_flag;	//DC
extern int CirculationHeater_flag;	//AC5_flag 할당
extern int CirculationValve_flag, InhalationValve_flag;	//DC2_flag, DC3_flag 할당

void DC1(int onoff);
void DC2(int onoff);
void DC3(int onoff);
void DC4(int onoff);
void DC5(int onoff);
void DC6(int onoff);
void DC7(int onoff);
void DC8(int onoff);

void AC1(int onoff);
void AC2(int onoff);
void AC3(int onoff);
void AC4(int onoff);
void AC5(int onoff);
void AC6(int onoff);
void AC7(int onoff);
void AC8(int onoff);

#endif /* SRC_HARDWARE_HARDWARE_H_ */
