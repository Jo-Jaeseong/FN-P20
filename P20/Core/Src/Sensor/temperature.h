/*
 * temperature.h
 *
 *  Created on: 2022. 10. 20.
 *      Author: CBT
 */

#ifndef SRC_SENSOR_TEMPERATURE_H_
#define SRC_SENSOR_TEMPERATURE_H_

extern float Temperature[5];
extern int DoorSettingTemp[3], ChamberSettingTemp[3], ChamberBackSettingTemp[3], VaporizerSettingTemp[3];
extern int CalibrationTemp[4];

uint8_t max6675ReadReg(uint16_t *reg, int Channel);
float max6675Temp(uint16_t reg);
void Check_Temp(int Channel);
void TempSetting();

#endif /* SRC_SENSOR_TEMPERATURE_H_ */
