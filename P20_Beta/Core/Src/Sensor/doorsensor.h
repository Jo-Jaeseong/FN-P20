/*
 * door_sensor.h
 *
 *  Created on: Nov 10, 2022
 *      Author: CBT
 */

#ifndef SRC_SENSOR_DOORSENSOR_H_
#define SRC_SENSOR_DOORSENSOR_H_

extern int DoorOpenFlag;
extern int DoorOpenVentFlag;
extern int DoorOpenVentCnt;

int DoorHandleCheck();
int DoorLatchCheck();
int BottleDoorCheck();
int BottleCheck();
int DoorOpenProcess();

#endif /* SRC_SENSOR_DOORSENSOR_H_ */
