/*
 * adc.c
 *
 *  Created on: Oct 25, 2022
 *      Author: CBT
 */

#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "vacuumsensor.h"

extern ADC_HandleTypeDef hadc2;
extern DMA_HandleTypeDef hdma_adc2;


uint32_t uiDensity, adcData[5], arrDensity[5];
int Pressure, Pressure2=0;
int sensorcount=0;


void InitADC(){
	HAL_ADC_Start_DMA(&hadc2 , (uint32_t *)adcData, 5);
}

void GetValue(){
	Pressure=adcData[0];
	//Pressure=adcData[0]/10;

	arrDensity[sensorcount]=movingAverageFilter(adcData,5);
	sensorcount++;
	if(sensorcount==5){
		Pressure2=(movingAverageFilter(arrDensity,5)*0.266-244.14);
		//Pressure2=(movingAverageFilter(arrDensity,5));
		sensorcount=0;
	}


}

uint32_t movingAverageFilter(uint32_t *samples, uint8_t sampleCount) {
    uint32_t sum = 0;
    for (uint8_t i = 0; i < sampleCount; i++) {
        sum += samples[i];
    }
    return sum / sampleCount;
}
