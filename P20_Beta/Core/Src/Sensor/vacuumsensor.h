/*
 * adc.h
 *
 *  Created on: Oct 25, 2022
 *      Author: CBT
 */

#ifndef SRC_PERIPHERAL_VACCUM_SENSOR_H_
#define SRC_PERIPHERAL_VACCUM_SENSOR_H_

extern int Pressure;
extern float Pressure2;

typedef struct {
    float torrValue;
    int adcValue;
} SensorData;

extern SensorData dataPoint1, dataPoint2;
extern float torrValue1, torrValue2;
extern int data1,data2;

extern float vacuumsplope, vacuumintercept; // 선형 변환을 위한 변수들
extern int CalibrationVacuum;
extern int PreesureCondition[3];
extern int DoorOpenPressure;

void InitADC();
void GetValue();
void ValueFilter();
uint32_t movingAverageFilter(uint32_t *samples, uint8_t sampleCount);
void saveSensorDataAtTorrValue(SensorData *dataPoint, float torrValue);
void calculateLinearTransformation(float data1Point1, float torrPoint1, float data1Point2, float torrPoint2, float* m, float* b);
float convertData1ToTorr(float data1, float m, float b);


#endif /* SRC_PERIPHERAL_VACCUM_SENSOR_H_ */
