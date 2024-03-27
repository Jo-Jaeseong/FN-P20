/*
 * heater.h
 *
 *  Created on: Nov 10, 2022
 *      Author: CBT
 */

#ifndef SRC_HARDWARE_HEATER_H_
#define SRC_HARDWARE_HEATER_H_


#define State1			0	//대기
#define State2			1	//공정
#define State3			2	//슬립


void DoorHeater(int onoff);
void ChamberHeater(int onoff);
void ChamberBackHeater(int onoff);
void VaporizerHeater(int onoff);

void HeaterControl();

void HeatingTest();
int ChamberHeaterCheck();
int VaporizerHeaterCheck();

#endif /* SRC_HARDWARE_HEATER_H_ */
