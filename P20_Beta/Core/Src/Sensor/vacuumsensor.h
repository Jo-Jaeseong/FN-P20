/*
 * adc.h
 *
 *  Created on: Oct 25, 2022
 *      Author: CBT
 */

#ifndef SRC_PERIPHERAL_VACCUM_SENSOR_H_
#define SRC_PERIPHERAL_VACCUM_SENSOR_H_

void InitADC();
void GetValue();
void DisplayAvgValue();
void DisplayAvgPressure();

void arrInsert(int *ar, int idx, int Iar);
void arrAppend(int *ar, float Iar);

#endif /* SRC_PERIPHERAL_VACCUM_SENSOR_H_ */
