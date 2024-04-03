/*
 * extern_flash.h
 *
 *  Created on: Apr 12, 2023
 *      Author: CBT
 */

#ifndef SRC_PERIPHERAL_EXTERN_FLASH_H_
#define SRC_PERIPHERAL_EXTERN_FLASH_H_

void W25Q128_Init(void);
void W25Q128_ChipSelect(uint8_t state);
void W25Q128_WriteEnable(void);
void W25Q128_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void W25Q128_ReadData(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);

#endif /* SRC_PERIPHERAL_EXTERN_FLASH_H_ */
