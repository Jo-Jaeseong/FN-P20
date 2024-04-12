/*
 * heater.h
 *
 *  Created on: Nov 10, 2022
 *      Author: CBT
 */

#ifndef SRC_HARDWARE_HEATER_H_
#define SRC_HARDWARE_HEATER_H_


#define State1			0
#define State2			1
#define State3			2


extern int temperror[5];
extern int TestTempErrorValue;

	/*
	 *
	#define State1			0	슬립
	#define State2			1	대기
	#define State3			2	공정
	*/


void DoorHeater(int onoff);
void ChamberHeater(int onoff);
void ChamberBackHeater(int onoff);
void VaporizerHeater(int onoff);

void CirculationHeater(int onoff);
void HeaterControl();

void HeatingTest();
int ChamberHeaterCheck();
int VaporizerHeaterCheck();

#endif /* SRC_HARDWARE_HEATER_H_ */
