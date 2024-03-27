/*
 * lcd.h
 *
 *  Created on: Oct 25, 2022
 *      Author: CBT
 */

#ifndef SRC_PERIPHERAL_LCD_H_
#define SRC_PERIPHERAL_LCD_H_

#include "main.h"

extern UART_HandleTypeDef huart1;
extern RTC_HandleTypeDef hrtc;
#define LCD_USART	&huart1

struct date_format {
	unsigned char year, month, day;
	unsigned char hour, minute, second;
};

extern struct date_format today_date;

void InitLCD();
void DisplayPage(int page);
void DisplayVacuumSensor();
void DisplayPageValue(int page ,int index, int value);

void LCD_Process();
void LCD_70(int index, int value);
void LCD_Function_Process(int index, int value);
void TestButton(int key);
void DisplayPartsTESTIcon(int index1, int index2, int value);
void DisplayIcon();

void DisplayTemprature();

#endif /* SRC_PERIPHERAL_LCD_H_ */
