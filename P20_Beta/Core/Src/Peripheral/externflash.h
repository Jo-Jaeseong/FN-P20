/*
 * extern_flash.h
 *
 *  Created on: Apr 12, 2023
 *      Author: CBT
 */

#ifndef SRC_PERIPHERAL_EXTERNFLASH_H_
#define SRC_PERIPHERAL_EXTERNFLASH_H_

#include <stdint.h>

void Write_Data_Flash();
void Read_Data_Flash();

void Write_Setting_Data_Flash();
void Read_Setting_Data_Flash();

void read_extern_flash(void);
void Erase_flash(void);
uint8_t W25Q16_SendByte(uint8_t data);
void W25Q16_WriteEnable(void);
void W25Q16_WriteDisable(void);
uint8_t W25Q16_ReadStatusReg1(void);

void W25Q16_WaitForReady(void);
void W25Q16_EraseSector(uint32_t sector_address);
void W25Q16_EraseChip(void);
void W25Q16_WritePage(uint32_t address, uint8_t* data, uint16_t length);
void W25Q16_ReadData(uint32_t address, uint8_t* data, uint16_t length);
int is_same_date(uint8_t year1, uint8_t month1, uint8_t day1, uint8_t year2, uint8_t month2, uint8_t day2);

#endif /* SRC_PERIPHERAL_EXTERNFLASH_H_ */
