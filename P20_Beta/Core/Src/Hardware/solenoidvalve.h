/*
 * solenoidvalve.h
 *
 *  Created on: Nov 10, 2022
 *      Author: CBT
 */

#ifndef SRC_HARDWARE_SOLENOIDVALVE_H_
#define SRC_HARDWARE_SOLENOIDVALVE_H_


void VacuumValve(int onoff);
void CirculationValve(int onoff);
void InhalationValve(int onoff);
void VentValve(int onoff);
void InjectionValve(int onoff);

#endif /* SRC_HARDWARE_SOLENOIDVALVE_H_ */
