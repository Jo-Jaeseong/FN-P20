/*
 * flash.h
 *
 *  Created on: 2023. 1. 11.
 *      Author: CBT
 */

#ifndef SRC_PERIPHERAL_FLASH_H_
#define SRC_PERIPHERAL_FLASH_H_

#include <stdbool.h>

extern unsigned char flash_sterilant_production_year,flash_sterilant_production_month,flash_sterilant_production_day;
extern unsigned char flash_sterilant_production_number;
extern unsigned char flash_sterilant_open_year, flash_sterilant_open_month, flash_sterilant_open_day;
extern unsigned char flash_sterilant_volume;

extern unsigned char flash_MODEL_NAME[10];
extern unsigned char flash_SERIAL_NUMBER[10];
extern unsigned char flash_DEPARTMENT_NAME[10];
extern unsigned char flash_FACILITY_NAME[10];
extern unsigned char flash_HARDWARE_VERSION[10];
extern unsigned char flash_SOFTWARE_VERSION[10];
extern unsigned char flash_LANGUAGE[10];

extern unsigned char flash_ID[5][10];
extern unsigned char flash_PW[5][10];
extern unsigned char flashuserCount;

void Write_Flash();
void Read_Flash();

void TestCycle();
void ShortCycle();
void StandardCycle();
void AdvancedCycle();

HAL_StatusTypeDef Flash_Write_Int(uint32_t address, uint32_t value);
uint32_t Flash_Read_Int(uint32_t address);

float char2float(const unsigned char *data);
void float2char(float fValue, char *data);

#endif /* SRC_PERIPHERAL_FLASH_H_ */
