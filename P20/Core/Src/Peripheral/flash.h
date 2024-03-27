/*
 * flash.h
 *
 *  Created on: 2023. 1. 11.
 *      Author: CBT
 */

#ifndef SRC_PERIPHERAL_FLASH_H_
#define SRC_PERIPHERAL_FLASH_H_

void Write_Flash();
void Read_Flash();

union fc {
	float fValue;
	char  cValue[4];
};

float char2float(const unsigned char *data);
void float2char(float fValue, char *data);

#endif /* SRC_PERIPHERAL_FLASH_H_ */
