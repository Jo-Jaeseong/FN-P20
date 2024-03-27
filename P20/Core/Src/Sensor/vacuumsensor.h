/*
 * adc.h
 *
 *  Created on: Oct 25, 2022
 *      Author: CBT
 */

#ifndef SRC_SENSOR_VACUUMSENSOR_H_
#define SRC_SENSOR_VACUUMSENSOR_H_

#include <stdio.h>
#include <stdlib.h>

void InitADC();
void GetValue();
void DisplayAvgDensity();
void arrInsert(int *ar, int idx, int Iar);
void arrAppend(int *ar, float Iar);
uint32_t movingAverageFilter(uint32_t *samples, uint8_t sampleCount);
int compare(const void *a, const void *b);
int medianFilter(int *data, int size);

#endif /* SRC_SENSOR_VACUUMSENSOR_H_ */
