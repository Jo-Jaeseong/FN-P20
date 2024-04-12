/*
 * adc.c
 *
 *  Created on: Oct 25, 2022
 *      Author: CBT
 */

#include <stdio.h>
#include <stdlib.h>
#include "main.h"

#include <sensor.h>
#include <peripheral.h>

extern ADC_HandleTypeDef hadc2;
extern DMA_HandleTypeDef hdma_adc2;


int maxDensity=0, Density=0;

int avgmax=0;
int Sensor_index=0;
int Sensor2_index=0;
int H2O2Sensor_Flag;
int PreesureCondition[3];

int CalibrationVacuum;


uint32_t uiDensity, adcData[5], arrDensity[5];
int Pressure;
float Pressure2;
int data1=0;
int data2=0;
int data3=0;

int sensorcount=0;

SensorData dataPoint1, dataPoint2;

float torrValue1, torrValue2;

float vacuumsplope=0.261, vacuumintercept=-207.5; // 선형 변환을 위한 변수들
int DoorOpenPressure;


void InitADC(){
	HAL_ADC_Start_DMA(&hadc2 , (uint32_t *)adcData, 5);
}

void GetValue(){
	//Pressure=adcData[0];
	//Pressure=adcData[0]/10;

	//Pressure=movingAverageFilter(adcData,5);
	//data2 *= .95;


	/*
	sensorcount++;
	if(sensorcount==5){
		Pressure2=(movingAverageFilter(arrDensity,5)*0.266-244.14);
		//Pressure2=(movingAverageFilter(arrDensity,5));
		sensorcount=0;
	}
	*/

	data2=movingAverageFilter(adcData,5);


	//data1=movingAverageFilter(adcData,5);

}
void ValueFilter(){
	//FirstMeasureFlag 언제 적용할지
	//초기값
	data1 *= .9;
	data1 += data2*0.1;

	//data1=data1/10;
	Pressure2=convertData1ToTorr(data1, vacuumsplope, vacuumintercept);
	if(Pressure2>760){
		Pressure2=760;
	}

	Pressure=Pressure2-10+CalibrationVacuum;
}

uint32_t movingAverageFilter(uint32_t *samples, uint8_t sampleCount) {
    uint32_t sum = 0;
    for (uint8_t i = 0; i < sampleCount; i++) {
        sum += samples[i];
    }
    return sum / sampleCount;
}

void saveSensorDataAtTorrValue(SensorData *dataPoint, float torrValue) {
    dataPoint->torrValue = torrValue;
    dataPoint->adcValue = data1 ;
    printf("Torr 값: %.2f, ADC 값: %d 저장되었습니다.\n", dataPoint->torrValue, dataPoint->adcValue);
}

void calculateLinearTransformation(float data1Point1, float torrPoint1, float data1Point2, float torrPoint2, float* m, float* b) {
    // 기울기(m) 계산
    *m = (torrPoint2 - torrPoint1) / (data1Point2 - data1Point1);

    // y절편(b) 계산
    *b = torrPoint1 - (*m * data1Point1);
}

// 주어진 data1 값의 스케일을 조절하여 Torr 단위로 변환하는 함수
float convertData1ToTorr(float data1, float m, float b) {
    return m * data1 + b;
}
