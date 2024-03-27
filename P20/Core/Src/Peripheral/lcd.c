/*
 * lcd.c
 *
 *  Created on: Oct 25, 2022
 *      Author: CBT
 */
#include <string.h>

#include "main.h"

#include "hardware.h"
#include "sensor.h"

#include "lcd.h"
#include "flash.h"
#include "extern_flash.h"
#include "print.h"
#include "process.h"
#include "rfid.h"
#include "usb.h"

//RTC
const unsigned char rtc_setting[13] = {0x5A, 0xA5, 0x0a, 0x80, 0x1f, 0x5a, 0x19, 0x06, 0x11, 0x03, 0x10, 0x26, 0x00};
const unsigned char rtc_date_get[6] = {0x5A, 0xA5, 0x03, 0x81, 0x20, 0x03};
const unsigned char rtc_time_get[6] = {0x5A, 0xA5, 0x03, 0x81, 0x24, 0x03};

extern unsigned char UART_Receive_Flag;

unsigned char   LCD_page[7] = {0x5A, 0xA5, 0x04, 0x80, 0x03, 0x00, 0x0a};
unsigned char	LCD_rx_data[30]={};

unsigned char   icon_partstest_index[10] = {0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,0x80,0x90};

struct date_format today_date;

extern float Pressure;

extern char test1234[4], ReadTest[4];




void InitLCD(void){
	HAL_Delay(1000);
	DisplayPage(70);
	HAL_Delay(100);
    HAL_UART_Receive_IT(LCD_USART, (uint8_t*)LCD_rx_data, 9);
}

void DisplayPage(int page){
	LCD_page[6] = page;
    HAL_UART_Transmit(LCD_USART, LCD_page, 7, 10);
}

void DisplayVacuumSensor(){
	DisplayPageValue(0x5F,0x91,Pressure*10);
}

void DisplayPageValue(int page ,int index, int value){
	unsigned char   PageValue[8] = {0x5A, 0xA5, 0x05, 0x82, 0x00, 0x00, 0x00, 0x00};
	PageValue[4] = page;
	PageValue[5] = index;
	PageValue[6] = value >> 8;
	PageValue[7] = value & 0xff;
    HAL_UART_Transmit(LCD_USART, PageValue, 8, 10);
}

void LCD_Process(){
	int iValue;
    switch(LCD_rx_data[4]) {
        case 0 :    //button
            LCD_Function_Process(LCD_rx_data[5], LCD_rx_data[8]);
			break;
        case 0x6F :
			iValue = LCD_rx_data[7];
			iValue <<= 8;
			iValue |= LCD_rx_data[8];
			LCD_70(LCD_rx_data[5], iValue);
			break;
    }
    HAL_UART_Receive_IT(LCD_USART, (uint8_t*)LCD_rx_data, 9);
}

void LCD_Function_Process(int index, int value){
    switch(index){
        case 2:
        	TestButton(value);
            break;
    }
}

void LCD_70(int index, int value){	//
	switch(index) {
		case 0x00 :
			switch(value) {
				case 1://도어 히터
					if(DoorHeater_flag==1){
						DoorHeater(0);
						DoorHeater_flag=0;
					}
					else{
						DoorHeater(1);
						DoorHeater_flag=1;
					}
					//printmain();
					Write_Flash();
					break;
				case 2://AC2
					if(AC2_flag==1){
						AC2(0);
						AC2_flag=0;
					}
					else{
						AC2(1);
						AC2_flag=1;
					}
					Read_Flash();
					DisplayPageValue(0x5F,0x71,test1234[0]);
					break;
				case 3://챔버 백 히터
					if(ChamberBackHeater_flag==1){
						ChamberBackHeater(0);
						ChamberBackHeater_flag=0;
					}
					else{
						ChamberBackHeater(1);
						ChamberBackHeater_flag=1;
					}
					W25Q128_WritePage(test1234, 0x000000, 4);
					break;
				case 4://분사기 히터
					if(VaporizerHeater_flag==1){
						VaporizerHeater(0);
						VaporizerHeater_flag=0;
					}
					else{
						VaporizerHeater(1);
						VaporizerHeater_flag=1;
					}
					W25Q128_ReadData(ReadTest, 0x000000, 4);
					break;

				case 5://AC5
					if(AC5_flag==1){
						AC5(0);
						AC5_flag=0;
					}
					else{
						AC5(1);
						AC5_flag=1;
					}
					break;
				case 6://진공 펌프
					if(VacuumPump_flag==1){
						VacuumPump(0);
						VacuumPump_flag=0;

					}
					else{
						VacuumPump(1);
						VacuumPump_flag=1;
					}
					break;

				case 7://플라즈마
					if(Plasma_flag==1){
						Plasma(0);
						Plasma_flag=0;
					}
					else{
						Plasma(1);
						Plasma_flag=1;
					}

					//SD테스트
					SaveToSDCard();
					break;

				case 8://AC8
					if(AC8_flag==1){
						AC8(0);
						AC8_flag=0;
					}
					else{
						AC8(1);
						AC8_flag=1;
					}
					SaveToUSBMemory();
					break;

				case 9://진공 벨브
					if(VacuumValve_flag==1){
						VacuumValve(0);
						VacuumValve_flag=0;
					}
					else{
						VacuumValve(1);
						VacuumValve_flag=1;
					}
					break;
				case 0x0A://DC2
					if(DC2_flag==1){
						DC2(0);
						DC2_flag=0;
					}
					else{
						DC2(1);
						DC2_flag=1;
					}
					break;
				case 0x0B://DC3
					if(DC3_flag==1){
						DC3(0);
						DC3_flag=0;
					}
					else{
						DC3(1);
						DC3_flag=1;
					}
					break;
				case 0x0C://환기 밸브
					if(VentValve_flag==1){
						VentValve(0);
						VentValve_flag=0;
					}
					else{
						VentValve(1);
						VentValve_flag=1;
					}
					break;
				case 0x0D://주입 밸브
					if(InjectionValve_flag==1){
						InjectionValve(0);
						InjectionValve_flag=0;
					}
					else{
						InjectionValve(1);
						InjectionValve_flag=1;
					}
					break;

				case 0x0E://도어 래치
					if(DoorLatch_flag==1){
						DoorLatch(0);
						DoorLatch_flag=0;
					}
					else{
						DoorLatch(1);
						DoorLatch_flag=1;
					}
					break;
				case 0x0F://주입 밸브
					if(Fan_flag==1){
						Fan(0);
						Fan_flag=0;
					}
					else{
						Fan(1);
						Fan_flag=1;
					}
					break;
				case 0x10://챔버 히터
					if(ChamberHeater_flag==1){
						ChamberHeater(0);
						ChamberHeater_flag=0;
					}
					else{
						ChamberHeater(1);
						ChamberHeater_flag=1;
					}
					break;
				case 0x11://페리 펌프
					if(PeristalticPump_flag==1){
						PeristalticSpeed();
						PeriPump(0);
						PeristalticPump_flag=0;
					}
					else{
						PeristalticSpeed();
						PeriPump(1);
						PeristalticPump_flag=1;
					}
					break;
				case 0x12://RFID 리드
					InitRFID();
					ReadRFID();
					DisplayPageValue(0x5F,0x71,RFIDData.production_number);
					break;
				}
			break;
		case 0x01 :
			break;
	}
}

void TestButton(int key){	//02XX

}

void DisplayPartsTESTIcon(int index1, int index2, int value){
	unsigned char   icon_display[8] = {0x5A, 0xA5, 0x05, 0x82, 0x00, 0x00, 0x00, 0x01};
	icon_display[4] = index1;
	icon_display[5] = index2;
	icon_display[7] = value;
	HAL_UART_Transmit(LCD_USART, icon_display, 8, 10);
}

void DisplayIcon(){
	DisplayPartsTESTIcon(0x5F,0x10,DoorHandleCheck());
	DisplayPartsTESTIcon(0x5F,0x20,DoorLatchCheck());
	DisplayPartsTESTIcon(0x5F,0x30,BottleCheck());
	DisplayPartsTESTIcon(0x5F,0x50,LevelSensor1Check());
	DisplayPartsTESTIcon(0x5F,0x60,LevelSensor2Check());
}

void DisplayTemprature(){
	DisplayPageValue(0x5F,0x81,Temperature[0]*10);
	DisplayPageValue(0x5F,0x85,Temperature[1]*10);
	DisplayPageValue(0x5F,0x89,Temperature[2]*10);
	DisplayPageValue(0x5F,0x8D,Temperature[3]*10);

}
