/*
 * flash.h
 *
 *  Created on: 2023. 1. 11.
 *      Author: CBT
 */

#ifndef SRC_PERIPHERAL_FLASH_H_
#define SRC_PERIPHERAL_FLASH_H_

extern unsigned char flash_ID[5][10];
extern unsigned char flash_PW[5][10];

void Write_Flash();
void Read_Flash();

void TestCycle();
void ShortCycle();
void StandardCycle();
void AdvancedCycle();


void rfid_callback();
//bool is_same(unsigned char a[], unsigned char b[], int length);
void shift_flash_data();

float char2float(const unsigned char *data);
void float2char(float fValue, char *data);

#endif /* SRC_PERIPHERAL_FLASH_H_ */
