/*
 * adc.c
 *
 *  Created on: Oct 25, 2022
 *      Author: CBT
 */


#include <vacuumsensor.h>
#include "main.h"

extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;

uint32_t uiDensity, adcData[5];
float Pressure=0;
extern unsigned char Running_Flag;
int maxDensity=0, Density=0;
int arrDensity[10],arrPressure[5];
int avgmax=0;
int Sensor_index=0;
int Sensor2_index=0;
int H2O2Sensor_Flag;
int PreesureCondition[3]={};

float CalibrationVacuum;


void InitADC(){
	HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adcData, 5);
}

void GetValue(){
	uiDensity = adcData[0];
	Density = uiDensity-792;
	float test = Density/(3.78);
//	float test = uiDensity;
	arrDensity[Sensor_index]=test;
	//Pressure=uiDensity;
	//Pressure=adcData[0];
	/*
	Sensor_index++;
	if(Sensor_index==10
			){
		DisplayAvgValue();
	}
	*/

}
/*
//중간값
void DisplayAvgValue() {
        int n = sizeof(arrDensity) / sizeof(int);
        int tmp;

        for(int i=0; i < n - 1; i++){
                for(int j=0; j < n - 1 - i; j++) {
                        if (arrDensity[j] > arrDensity[j + 1]) {
                                tmp = arrDensity[j];
                                arrDensity[j] = arrDensity[j + 1];
                                arrDensity[j + 1] = tmp;
                        }
                }
        }

        if(n % 2 == 1) {
        	Pressure = arrDensity[(n / 2)];
        } else {
        	Pressure = (arrDensity[n / 2 - 1] + arrDensity[n / 2]) * 1.0 / 2.0;
        }
        Sensor_index=0;
}
*/

//평균값
void DisplayAvgValue(){
	int hap=0,max=0,min=10000;

	float avg=0;
	int repeat=sizeof(arrDensity)/sizeof(int);
	for(int i=0;i<repeat;i++){
		hap+=arrDensity[i];
		if(arrDensity[i]>max){
			max = arrDensity[i];
		}
		if(arrDensity[i]<min){
			min = arrDensity[i];
		}
	}
	hap = hap-max-min;
	avg = hap/(sizeof(arrDensity)/sizeof(int)-2);
	Pressure = avg;
	Sensor_index=0;


	arrPressure[Sensor2_index]=avg;
	Sensor2_index++;
	if(Sensor2_index==5){
		DisplayAvgPressure();
	}
}

void DisplayAvgPressure(){
	int hap=0,max=0,min=10000;

	float avg=0;
	int repeat=sizeof(arrPressure)/sizeof(int);
	for(int i=0;i<repeat;i++){
		hap+=arrPressure[i];
		if(arrPressure[i]>max){
			max = arrPressure[i];
		}
		if(arrPressure[i]<min){
			min = arrPressure[i];
		}
	}
	hap = hap-max-min;
	avg = hap/(sizeof(arrPressure)/sizeof(int)-2);
	Pressure= avg;
	Sensor2_index=0;
}

void arrInsert(int *ar, int idx, int Iar){
	memmove(ar+idx+1,ar+idx,sizeof(ar)/sizeof(int)-idx+1);
	ar[idx]=Iar;
	idx++;
}
void arrAppend(int *ar, float Iar){
	arrInsert(ar, sizeof(ar)/sizeof(int), Iar);
}
