/*
 * lcd.c
 *
 *  Created on: Oct 25, 2022
 *      Author: CBT
 */
#include <externflash.h>
#include <string.h>
#include "main.h"

#include "hardware.h"
#include "sensor.h"
#include "peripheral.h"


extern int flashtest;



extern uint32_t adcData[5];

extern UART_HandleTypeDef huart1;
extern RTC_HandleTypeDef hrtc;
#define LCD_USART	&huart1

//RTC
const unsigned char rtc_setting[13] = {0x5A, 0xA5, 0x0a, 0x80, 0x1f, 0x5a, 0x19, 0x06, 0x11, 0x03, 0x10, 0x26, 0x00};
const unsigned char rtc_date_get[6] = {0x5A, 0xA5, 0x03, 0x81, 0x20, 0x03};
const unsigned char rtc_time_get[6] = {0x5A, 0xA5, 0x03, 0x81, 0x24, 0x03};

extern unsigned char UART_Receive_Flag;

extern unsigned char Running_Flag;

unsigned char   main_page[7] = {0x5A, 0xA5, 0x04, 0x80, 0x03, 0x00, 0x0a};
unsigned char	LCD_rx_data[30]={};

extern int TestMode;
extern int TestCompleteflag;
int current_page=0;
int before_page=0;
extern int HeaterControlMode;
extern int TestTempErrorValue;

int TestVacuumValue=10;
int TestLeakValue=2;

extern int autoprint;



//extern char RFIDData[4]; //확인 필요
extern struct RFID_format RFIDData;

extern int checkret;
int ret1;
extern int tempcnt;



struct Process_data_format	CycleData[7][21];


unsigned int TotalTime;
unsigned int fProcessTime[7];
unsigned int CycleTime=0;
unsigned int EndTime=0;
unsigned int ProcessTime[7]={};
unsigned int StepTime[21]={};
extern unsigned char CurrentProcess, CurrentStep;

#define SHORT		1
#define STANDARD	2
#define ADVANCED	3

int CycleName=SHORT;
int ProcessNum,StepNum;
int testbit;

extern float Temperature[5];

extern int DoorSettingTemp[3], ChamberSettingTemp[3], ChamberBackSettingTemp[3], VaporizerSettingTemp[3];
float FlashSettingTemp[4][3]={};

extern int PreesureCondition[3];
int FlashPreesureCondition[3]={};


extern int PlasmaTime[2];
int FlashPlasmaTime[2];


extern float Pressure;
extern int CalibrationTemp[4];
extern float CalibrationVacuum;


struct date_format today_date;
extern struct data_format p_data;

extern unsigned char flash_MODEL_NAME[10];
extern unsigned char flash_SERIAL_NUMBER[10];
extern unsigned char flash_DEPARTMENT_NAME[10];
extern unsigned char flash_FACILITY_NAME[10];
extern unsigned char flash_HARDWARE_VERSION[10];
extern unsigned char flash_SOFTWARE_VERSION[10];
extern unsigned char flash_LANGUAGE[10];


extern unsigned char flash_sterilant_production_year[3],flash_sterilant_production_month[3],flash_sterilant_production_day[3];
extern unsigned char flash_sterilant_production_number[3];
extern unsigned char flash_sterilant_open_year[3], flash_sterilant_open_month[3], flash_sterilant_open_day[3];
extern unsigned char flash_sterilant_volume[3];

extern unsigned char flash_ID[5][10];
extern unsigned char flash_PW[5][10];
extern unsigned char flashuserCount;


unsigned int inputyear, inputmonth, inputday;

int Select_ID;
extern unsigned int CurrentUser;

extern unsigned char loadflash_ID[10];
extern unsigned char flash_load_index;
extern struct data_format load_data;
extern float loadPressureData[300];
extern float loadTemperatureData[300];
extern unsigned int temptotalcycle[6];

extern float PressureData[300];
extern float TemperatureData[300];

extern unsigned int DataCounter;

void InitLCD(void){	//LCD 초기화
	HAL_Delay(1000);
	//DisplayFirstPage();
	DisplayPage(60);
	//DisplayPage(LCD_INFO_HISTORY_PAGE);
	SetRTCFromLCD();
    //Start_Page
	HAL_Delay(100);

    HAL_UART_Receive_IT(LCD_USART, (uint8_t*)LCD_rx_data, 9);
}

void DisplayFirstPage(){
    DisplayPage(LCD_FIRST_PAGE);
}

void DisplayPage(int page){
	main_page[6] = page;
	if(page!=LCD_LOADING_PAGE)
		current_page=page;
    HAL_UART_Transmit(LCD_USART, main_page, 7, 10);
}

void DisplayDot(int vp1,int vp2,int x, int y){
	unsigned char   Display_Dot[18] = {0x5A, 0xA5,
														0x0F,
														0x82,
														0x40, 0x00,
														0x00, 0x01,
														0x00, 0x01,
														0x00, 0x80,
														0x00, 0x80,
														0xF8, 0x00,
														0xFF, 0x00};
	Display_Dot[10] = x>>8;
	Display_Dot[11] = x&0xff;
	Display_Dot[12] = y>>8;
	Display_Dot[13] = y&0xff;

    HAL_UART_Transmit(LCD_USART, Display_Dot, 18, 10);
}

void DisplayTempGraph(int number, int color){
	unsigned char   Display_Dot[1500] = {0x5A, 0xA5,//헤더
															0x17,//데이터 개수
															0x82,//명령어
															0x40, 0x00,//주소
															0x00, 0x02,//선연결 데이터
															0x00, 0x02,//연결선의 수//8,9
															//0x00, 0x1F,//컬러//10,11
															0xF8, 0x00,//컬러//10,11
															0x00, 0x64, 0x01, 0x9A,//점1	//dot1	12,13,14,15
															0x03, 0x98, 0x00, 0x82,//점2//dot2	16,17,18,19
															0x00, 0xB0, 0x00, 0x80,//점3//dot3	20,21,22,23
															//.....
															0xFF, 0x00}; //End//20
		int DisplayStartPoint[10] = {0,110,200,290,380,470,560,650,740,830};
		int Displaysector;
		unsigned char DisplaysectorAddress1[10]={0x00,0x40,0x40,0x41,0x41,0x42,0x42,0x43,0x43,0x44};
		unsigned char DisplaysectorAddress2[10]={0x00,0x00,0x50,0x00,0x50,0x00,0x50,0x00,0x50,0x50};

		Displaysector=(int)(number+1)/30;

		for(int ii=0;ii<Displaysector;ii++){
			Display_Dot[2]=15+4*30;
			Display_Dot[4]=DisplaysectorAddress1[ii+1];
			Display_Dot[5]=DisplaysectorAddress2[ii+1];
			Display_Dot[8]=30>>8;
			Display_Dot[9]=30&0xFF;

			Display_Dot[12]=DisplayStartPoint[ii+1]>>8;
			Display_Dot[13]=DisplayStartPoint[ii+1]&0xFF;
			Display_Dot[14]=(410-(int)(TemperatureData[ii*30]*3.5))>>8;
			Display_Dot[15]=(410-(int)(TemperatureData[ii*30]*3.5))&0xFF;

			for(int i=1;i<=30;i++){
				Display_Dot[12+i*4]=(3*i+DisplayStartPoint[ii+1])>>8;
				Display_Dot[13+i*4]=(3*i+DisplayStartPoint[ii+1])&0xFF;
				Display_Dot[14+i*4]=(410-(int)(TemperatureData[i+ii*30]*3.5))>>8;
				Display_Dot[15+i*4]=(410-(int)(TemperatureData[i+ii*30]*3.5))&0xFF;
			}
			Display_Dot[16+4*30]=0xFF;
			Display_Dot[17+4*30]=0x00;
			HAL_UART_Transmit(LCD_USART, Display_Dot, 18+4*30, 100);
		}
		if(number!=Displaysector*30){
			Display_Dot[2]=15+4*(number-Displaysector*30);
			Display_Dot[4]=DisplaysectorAddress1[Displaysector+1];
			Display_Dot[5]=DisplaysectorAddress2[Displaysector+1];
			Display_Dot[8]=(number-Displaysector*30)>>8;
			Display_Dot[9]=(number-Displaysector*30)&0xFF;

			Display_Dot[12]=DisplayStartPoint[Displaysector+1]>>8;
			Display_Dot[13]=DisplayStartPoint[Displaysector+1]&0xFF;
			Display_Dot[14]=(410-(int)(TemperatureData[Displaysector*30]*3.5))>>8;
			Display_Dot[15]=(410-(int)(TemperatureData[Displaysector*30]*3.5))&0xFF;

			for(int i=1;i<=(number-Displaysector*30);i++){
				Display_Dot[12+i*4]=(3*i+DisplayStartPoint[Displaysector+1])>>8;
				Display_Dot[13+i*4]=(3*i+DisplayStartPoint[Displaysector+1])&0xFF;
				Display_Dot[14+i*4]=(410-(int)(TemperatureData[i+Displaysector*30]*3.5))>>8;
				Display_Dot[15+i*4]=(410-(int)(TemperatureData[i+Displaysector*30]*3.5))&0xFF;
			}
			Display_Dot[16+4*(number-Displaysector*30)]=0xFF;
			Display_Dot[17+4*(number-Displaysector*30)]=0x00;

			HAL_UART_Transmit(LCD_USART, Display_Dot, 18+4*(number-Displaysector*30), 100);
		}

}



void DisplayInitTempGraph(){
	unsigned char   Display_Dot[1500] = {0x5A, 0xA5,//헤더
															0x17,//데이터 개수
															0x82,//명령어
															0x40, 0x00,//주소
															0x00, 0x02,//선연결 데이터
															0x00, 0x02,//연결선의 수//8,9
															0x00, 0x1F,//컬러//10,11
															0x00, 0x64, 0x01, 0x9A,//점1	//dot1	12,13,14,15
															0x03, 0x98, 0x00, 0x82,//점2//dot2	16,17,18,19
															0x00, 0xB0, 0x00, 0x80,//점3//dot3	20,21,22,23
															//.....
															0xFF, 0x00}; //End//20
		unsigned char DisplaysectorAddress1[10]={0x00,0x40,0x40,0x41,0x41,0x42,0x42,0x43,0x43,0x44};
		unsigned char DisplaysectorAddress2[10]={0x00,0x00,0x50,0x00,0x50,0x00,0x50,0x00,0x50,0x50};

		for(int ii=1;ii<=9;ii++){
			Display_Dot[2]=15+4*30;
			Display_Dot[4]=DisplaysectorAddress1[ii];
			Display_Dot[5]=DisplaysectorAddress2[ii];
			Display_Dot[8]=0x00;
			Display_Dot[9]=0x00;
			Display_Dot[16+4*30]=0xFE;
			Display_Dot[17+4*30]=0x00;
			HAL_UART_Transmit(LCD_USART, Display_Dot, 18+4*30, 100);
		}
}

void DisplayInitVacuumGraph(){
	unsigned char   Display_Dot[1500] = {0x5A, 0xA5,//헤더
															0x17,//데이터 개수
															0x82,//명령어
															0x40, 0x00,//주소
															0x00, 0x02,//선연결 데이터
															0x00, 0x02,//연결선의 수//8,9
															0x00, 0x1F,//컬러//10,11
															0x00, 0x64, 0x01, 0x9A,//점1	//dot1	12,13,14,15
															0x03, 0x98, 0x00, 0x82,//점2//dot2	16,17,18,19
															0x00, 0xB0, 0x00, 0x80,//점3//dot3	20,21,22,23
															//.....
															0xFF, 0x00}; //End//20
		unsigned char DisplaysectorAddress1[10]={0x00,0x45,0x45,0x46,0x46,0x47,0x47,0x48,0x48,0x49};
		unsigned char DisplaysectorAddress2[10]={0x00,0x00,0x50,0x00,0x50,0x00,0x50,0x00,0x50,0x00};

		for(int ii=1;ii<=9;ii++){
			Display_Dot[2]=15+4*30;
			Display_Dot[4]=DisplaysectorAddress1[ii];
			Display_Dot[5]=DisplaysectorAddress2[ii];
			Display_Dot[8]=0x00;
			Display_Dot[9]=0x00;
			Display_Dot[16+4*30]=0xFE;
			Display_Dot[17+4*30]=0x00;
			HAL_UART_Transmit(LCD_USART, Display_Dot, 18+4*30, 100);
		}
}


void DisplayVacuumGraph(int number, int color){
	unsigned char   Display_Dot[1500] = {0x5A, 0xA5,//헤더
															0x17,//데이터 개수
															0x82,//명령어
															0x40, 0x00,//주소
															0x00, 0x02,//선연결 데이터
															0x00, 0x02,//연결선의 수//8,9
															0x00, 0x1F,//컬러//10,11
															0x00, 0x64, 0x01, 0x9A,//점1	//dot1	12,13,14,15
															0x03, 0x98, 0x00, 0x82,//점2//dot2	16,17,18,19
															0x00, 0xB0, 0x00, 0x80,//점3//dot3	20,21,22,23
															//.....
															0xFF, 0x00}; //End//20
		int DisplayStartPoint[10] = {0,110,200,290,380,470,560,650,740,830};
		int Displaysector;
		unsigned char DisplaysectorAddress1[10]={0x00,0x45,0x45,0x46,0x46,0x47,0x47,0x48,0x48,0x49};
		unsigned char DisplaysectorAddress2[10]={0x00,0x00,0x50,0x00,0x50,0x00,0x50,0x00,0x50,0x00};

		Displaysector=(int)(number+1)/30;

		for(int ii=0;ii<Displaysector;ii++){
			Display_Dot[2]=15+4*30;
			Display_Dot[4]=DisplaysectorAddress1[ii+1];
			Display_Dot[5]=DisplaysectorAddress2[ii+1];
			Display_Dot[8]=30>>8;
			Display_Dot[9]=30&0xFF;

			Display_Dot[12]=DisplayStartPoint[ii+1]>>8;
			Display_Dot[13]=DisplayStartPoint[ii+1]&0xFF;
			Display_Dot[14]=(410-(int)(PressureData[ii*30]*0.35))>>8;
			Display_Dot[15]=(410-(int)(PressureData[ii*30]*0.35))&0xFF;

			for(int i=1;i<=30;i++){
				Display_Dot[12+i*4]=(3*i+DisplayStartPoint[ii+1])>>8;
				Display_Dot[13+i*4]=(3*i+DisplayStartPoint[ii+1])&0xFF;
				Display_Dot[14+i*4]=(410-(int)(PressureData[i+ii*30]*0.35))>>8;
				Display_Dot[15+i*4]=(410-(int)(PressureData[i+ii*30]*0.35))&0xFF;
			}
			Display_Dot[16+4*30]=0xFF;
			Display_Dot[17+4*30]=0x00;
			HAL_UART_Transmit(LCD_USART, Display_Dot, 18+4*30, 100);
		}
		if(number!=Displaysector*30){
			Display_Dot[2]=15+4*(number-Displaysector*30);
			Display_Dot[4]=DisplaysectorAddress1[Displaysector+1];
			Display_Dot[5]=DisplaysectorAddress2[Displaysector+1];
			Display_Dot[8]=(number-Displaysector*30)>>8;
			Display_Dot[9]=(number-Displaysector*30)&0xFF;

			Display_Dot[12]=DisplayStartPoint[Displaysector+1]>>8;
			Display_Dot[13]=DisplayStartPoint[Displaysector+1]&0xFF;
			Display_Dot[14]=(410-(int)(PressureData[Displaysector*30]*0.35))>>8;
			Display_Dot[15]=(410-(int)(PressureData[Displaysector*30]*0.35))&0xFF;

			for(int i=1;i<=(number-Displaysector*30);i++){
				Display_Dot[12+i*4]=(3*i+DisplayStartPoint[Displaysector+1])>>8;
				Display_Dot[13+i*4]=(3*i+DisplayStartPoint[Displaysector+1])&0xFF;
				Display_Dot[14+i*4]=(410-(int)(PressureData[i+Displaysector*30]*0.35))>>8;
				Display_Dot[15+i*4]=(410-(int)(PressureData[i+Displaysector*30]*0.35))&0xFF;
			}
			Display_Dot[16+4*(number-Displaysector*30)]=0xFF;
			Display_Dot[17+4*(number-Displaysector*30)]=0x00;

			HAL_UART_Transmit(LCD_USART, Display_Dot, 18+4*(number-Displaysector*30), 100);
		}

}

//LCD 수신
void LCD_Process(){
    int iValue;
    switch(LCD_rx_data[4]) {
        case 0 :    //button
            LCD_Function_Process(LCD_rx_data[5], LCD_rx_data[8]);
            break;
        case 0x02 :
			iValue = LCD_rx_data[7];
			iValue <<= 8;
			iValue |= LCD_rx_data[8];
			LCD_02(LCD_rx_data[5], iValue);
			break;
        case 0x03 :
			iValue = LCD_rx_data[7];
			iValue <<= 8;
			iValue |= LCD_rx_data[8];
			LCD_03(LCD_rx_data[5], iValue);
			break;
        case 0x04 :
            iValue = LCD_rx_data[7];
            iValue <<= 8;
            iValue |= LCD_rx_data[8];
            LCD_04(LCD_rx_data[5], iValue);
            break;
        case 0x05 :
            iValue = LCD_rx_data[7];
            iValue <<= 8;
            iValue |= LCD_rx_data[8];
            LCD_05(LCD_rx_data[5], iValue);
            break;
        case 0x06 :
            iValue = LCD_rx_data[7];
            iValue <<= 8;
            iValue |= LCD_rx_data[8];
            LCD_06(LCD_rx_data[5], iValue);
            break;
        case 0x07 :
            iValue = LCD_rx_data[7];
            iValue <<= 8;
            iValue |= LCD_rx_data[8];
            LCD_07(LCD_rx_data[5], iValue);
            break;
        case 0x08 :
            iValue = LCD_rx_data[7];
            iValue <<= 8;
            iValue |= LCD_rx_data[8];
            LCD_08(LCD_rx_data[5], iValue);
            break;
        case 0x09 :
            iValue = LCD_rx_data[7];
            iValue <<= 8;
            iValue |= LCD_rx_data[8];
            LCD_09(LCD_rx_data[5], iValue);
            break;
        case 0x10 :
            iValue = LCD_rx_data[7];
            iValue <<= 8;
            iValue |= LCD_rx_data[8];
            LCD_10(LCD_rx_data[5], iValue);
            break;
        case 0x11 :
			iValue = LCD_rx_data[7];
			iValue <<= 8;
			iValue |= LCD_rx_data[8];
			LCD_11(LCD_rx_data[5], iValue);
			break;
        case 0x12 :
			iValue = LCD_rx_data[7];
			iValue <<= 8;
			iValue |= LCD_rx_data[8];
			LCD_12(LCD_rx_data[5], iValue);
			break;
        case 0x13 :
			iValue = LCD_rx_data[7];
			iValue <<= 8;
			iValue |= LCD_rx_data[8];
			LCD_13(LCD_rx_data[5], iValue);
			break;
        case 0x14 :
			iValue = LCD_rx_data[7];
			iValue <<= 8;
			iValue |= LCD_rx_data[8];
			LCD_14(LCD_rx_data[5], iValue);
			break;

        case 0x21 :
			iValue = LCD_rx_data[7];
			iValue <<= 8;
			iValue |= LCD_rx_data[8];
			LCD_21(LCD_rx_data[5], iValue);
			break;

        case 0x22 :
			iValue = LCD_rx_data[7];
			iValue <<= 8;
			iValue |= LCD_rx_data[8];
			LCD_22(LCD_rx_data[5], iValue);
			break;

        case 0x23 :
			iValue = LCD_rx_data[7];
			iValue <<= 8;
			iValue |= LCD_rx_data[8];
			LCD_23(LCD_rx_data[5], iValue);
			break;
        case 0x24 :
			iValue = LCD_rx_data[7];
			iValue <<= 8;
			iValue |= LCD_rx_data[8];
			LCD_24(LCD_rx_data[5], iValue);
			break;
        case 0x25 :    // set value;
            iValue = LCD_rx_data[7];
            iValue <<= 8;
            iValue |= LCD_rx_data[8];
            LCD_25(LCD_rx_data[5], iValue);
            break;
        case 0x26 :    // set value;
            iValue = LCD_rx_data[7];
            iValue <<= 8;
            iValue |= LCD_rx_data[8];
            LCD_26(LCD_rx_data[5], iValue);
            break;
        case 0x27 :    // set value;
            iValue = LCD_rx_data[7];
            iValue <<= 8;
            iValue |= LCD_rx_data[8];
            LCD_27(LCD_rx_data[5], iValue);
            break;
        case 0x28 :    // set value;
            iValue = LCD_rx_data[7];
            iValue <<= 8;
            iValue |= LCD_rx_data[8];
            LCD_28(LCD_rx_data[5], iValue);
            break;
        case 0x30 :    // set value;
            iValue = LCD_rx_data[7];
            iValue <<= 8;
            iValue |= LCD_rx_data[8];
            LCD_30(LCD_rx_data[5], iValue);
            break;
        case 0x31 :    // set value;
            iValue = LCD_rx_data[7];
            iValue <<= 8;
            iValue |= LCD_rx_data[8];
            LCD_31(LCD_rx_data[5], iValue);
            break;
        case 0x33 :    // set setting;
			iValue = LCD_rx_data[7];
			iValue <<= 8;
			iValue |= LCD_rx_data[8];
			LCD_33(LCD_rx_data[5], iValue);
			break;
        case 0x34 :    // set setting;
			iValue = LCD_rx_data[7];
			iValue <<= 8;
			iValue |= LCD_rx_data[8];
			LCD_34(LCD_rx_data[5], iValue);
			break;
        case 0x35 :    // set setting;
			iValue = LCD_rx_data[7];
			iValue <<= 8;
			iValue |= LCD_rx_data[8];
			LCD_35(LCD_rx_data[5], iValue);
			break;
        case 0x36 :    // set setting;
			iValue = LCD_rx_data[7];
			iValue <<= 8;
			iValue |= LCD_rx_data[8];
			LCD_36(LCD_rx_data[5], iValue);
			break;
        case 0x37 :    // set setting;
			iValue = LCD_rx_data[7];
			iValue <<= 8;
			iValue |= LCD_rx_data[8];
			LCD_37(LCD_rx_data[5], iValue);
			break;
        case 0x38 :    // set setting;
			iValue = LCD_rx_data[7];
			iValue <<= 8;
			iValue |= LCD_rx_data[8];
			LCD_38(LCD_rx_data[5], iValue);
			break;
        case 0x39 :    // set setting;
			iValue = LCD_rx_data[7];
			iValue <<= 8;
			iValue |= LCD_rx_data[8];
			LCD_39(LCD_rx_data[5], iValue);
			break;
        case 0x44 :    // set setting;
			iValue = LCD_rx_data[7];
			iValue <<= 8;
			iValue |= LCD_rx_data[8];
			LCD_44(LCD_rx_data[5], iValue);
			break;
        case 0x60 :    // 60page board test
			iValue = LCD_rx_data[7];
			iValue <<= 8;
			iValue |= LCD_rx_data[8];
			LCD_60(LCD_rx_data[5], iValue);
			break;
    }
    HAL_UART_Receive_IT(LCD_USART, (uint8_t*)LCD_rx_data, 9);
}

void LCD_Function_Process(int index, int value){
    switch(index){
        case 0:
            DoActionButton(value);
            break;
        case 2:

            break;
        case 3:
        	ProcessSettingButton(value);
            break;
            /*
        case 4:
            MaintenanceButton(value);
            break;
        case 5:
			DeveloperButton(value);
			break;
			*/
    }
}

void LCD_02(int index, int value){	//
	switch(index) {
		case 0x00 :
			switch(value) {
				case 0x01 : // Sign in
					if(loginProcess()){
						DisplayPage(LCD_HOME_PAGE);
						Display05page();
					}
					else{
						Display02page();
						DisplayPage(LCD_LOGIN_POPUP_PAGE);
					}
					//DisplayPage();
					break;
		        case 0x02:	//Create Account
		        	DisplayPage(LCD_CREATE_ID);
		        	Display03page();
					break;
		        case 0x45:	//Return to login page
	        		DisplayPage(LCD_LOGIN_PAGE);
					Display03page();
					break;

			}
			break;
		case 0x01 :
			break;

	}
}
void LCD_03(int index, int value){
	switch(index) {
		case 0x00 :
			switch(value) {
				case 0x01 : // management
	        		DisplayPage(LCD_MANAGEMENT_ID);
	        		Display04page();
					break;
		        case 0x02:	// sign up(생성)
		        	switch(createUser()) {
						case 0 :
			        		DisplayPage(LCD_CREATE_ID_POPUP_PAGE);
			        		Display03page();
							break;
						case 1 :
			        		Write_Flash();
			        		DisplayPage(LCD_LOGIN_PAGE);
							break;
						case 2 :
			        		DisplayPage(LCD_CREATE_ID_POPUP_PAGE2);
			        		Display03page();
							break;
						case 3 :
			        		DisplayPage(LCD_CREATE_ID_POPUP_PAGE);
			        		Display03page();
							break;

		        	}
					break;
		        case 0x03:	// cancel
		        	DisplayPage(LCD_LOGIN_PAGE);
		        	Display02page();
					break;

		        case 0x46:	//Return to LCD_CREATE_ID page
		        	DisplayPage(LCD_CREATE_ID);
		        	Display02page();
					break;

		        case 0x47:	//Return to LCD_CREATE_ID page
		        	DisplayPage(LCD_CREATE_ID);
		        	Display02page();
					break;
			}
			break;
		case 0x01 :
			break;

	}
}

void LCD_04(int index, int value){
	switch(index) {
		case 0x00 :
			switch(value) {
				case 0x01 : // sign up(생성)
		        	DisplayPage(LCD_CREATE_ID);
		        	Display03page();
					break;

		        case 0x02:	// Change ID1
		        	if(flash_ID[0][0]!=0){
						Select_ID=0;
						Display04page();
						DisplayPage10Char(0x04,0x70,"");
						DisplayPage(LCD_MANAGEMENT_CHANGE_PW_POPUP1);
		        	}
					break;
		        case 0x03:	// Change ID2
		        	if(flash_ID[1][0]!=0){
						Select_ID=1;
						Display04page();
						DisplayPage10Char(0x04,0x70,"");
						DisplayPage(LCD_MANAGEMENT_CHANGE_PW_POPUP1);
		        	}
					break;
		        case 0x04:	// Change ID3
		        	if(flash_ID[2][0]!=0){
						Select_ID=2;
						Display04page();
						DisplayPage10Char(0x04,0x70,"");
						DisplayPage(LCD_MANAGEMENT_CHANGE_PW_POPUP1);
		        	}
					break;
		        case 0x05:	// Change ID4
		        	if(flash_ID[3][0]!=0){
						Select_ID=3;
						Display04page();
						DisplayPage10Char(0x04,0x70,"");
						DisplayPage(LCD_MANAGEMENT_CHANGE_PW_POPUP1);
		        	}
					break;
		        case 0x06:	// Change ID5
		        	if(flash_ID[4][0]!=0){
						Select_ID=4;
						Display04page();
						DisplayPage10Char(0x04,0x70,"");
						DisplayPage(LCD_MANAGEMENT_CHANGE_PW_POPUP1);
		        	}
					break;

		        case 0x07:	// DELET ID1
		        	if(flash_ID[0][0]!=0){
						Select_ID=0;
						Display04page();
						DisplayPage10Char(0x04,0x70,"");
						DisplayPage(LCD_MANAGEMENT_DELET_ID_POPUP);
		        	}
					break;
		        case 0x08:	// DELET ID2
		        	if(flash_ID[1][0]!=0){
						Select_ID=1;
						Display04page();
						DisplayPage10Char(0x04,0x70,"");
						DisplayPage(LCD_MANAGEMENT_DELET_ID_POPUP);
		        	}
					break;
		        case 0x09:	// DELET ID3
		        	if(flash_ID[2][0]!=0){
						Select_ID=2;
						Display04page();
						DisplayPage10Char(0x04,0x70,"");
						DisplayPage(LCD_MANAGEMENT_DELET_ID_POPUP);
		        	}
					break;
		        case 0x0A:	// DELET ID4
		        	if(flash_ID[3][0]!=0){
						Select_ID=3;
						Display04page();
						DisplayPage10Char(0x04,0x70,"");
						DisplayPage(LCD_MANAGEMENT_DELET_ID_POPUP);
		        	}
					break;
		        case 0x0B:	// DELET ID5
		        	if(flash_ID[4][0]!=0){
						Select_ID=4;
						Display04page();
						DisplayPage10Char(0x04,0x70,"");
						DisplayPage(LCD_MANAGEMENT_DELET_ID_POPUP);
		        	}
					break;

		        case 0x0C:	// BACK
		        	DisplayPage(LCD_LOGIN_PAGE);
		        	Display02page();
					break;
		        case 0x10:	// ID,PASSWORD CHECK
		        	if(changePWloginUser()){
		        		DisplayPage(LCD_MANAGEMENT_CHANGE_PW_POPUP2);
		        		DisplayPage10Char(0x04,0x70,"");
		        		DisplayPage10Char(0x04,0x80,"");
		        	}
		        	else{
		        		DisplayPage(LCD_MANAGEMENT_WRONG_PW_POPUP);
		        	}

					break;

		        case 0x12:	// Change PW
		        	if(changePWUser()){
		        		Write_Flash();
		        		Display04page();
		        		DisplayPage(LCD_MANAGEMENT_ID);
		        	}
		        	else{
		        		DisplayPage(LCD_MANAGEMENT_WRONG_PW_POPUP);
		        	}
					break;

		        case 0x13:	// CANCEL
		        	DisplayPage(LCD_MANAGEMENT_ID);
		            Display04page();
					break;

		        case 0x14:	// DELET ID
		        	if(DeletConfirmloginUser()){
		        		Write_Flash();
			        	DisplayPage(LCD_MANAGEMENT_ID);
			            Display04page();
		        		//ID 카운트 및 정렬문제 추가 확인 필요
		        	}
		        	else{
		        		DisplayPage(LCD_MANAGEMENT_WRONG_PW_POPUP);
		        	}
					break;

			}
			break;
		case 0x01 :
			break;

	}

}

void LCD_05(int index, int value){	//input Value
    switch(index) {
		case 0x00 :
			switch(value) {
				case 0x01 :
					//SHORT MODE
					CycleName=SHORT;
					Read_Flash();
					ProcessNum=1;
					StepNum=1;
					CurrentProcess=1;
					CurrentStep=1;
		        	ReadProcessTime();
		        	ReadCycleTime();
		        	ReadStepTime();
		        	DisplayPage(LCD_STANDBY_PAGE);
		        	Display06page();
					break;
				case 0x02 :
					//STANDARD MODE
					CycleName=STANDARD;
					Read_Flash();
					ProcessNum=1;
					StepNum=1;
					CurrentProcess=1;
					CurrentStep=1;
		        	ReadProcessTime();
		        	ReadCycleTime();
		        	ReadStepTime();
		        	DisplayPage(LCD_STANDBY_PAGE);
		        	Display06page();
					break;
				case 0x03 :
					//ADVANCED MODE
					CycleName=ADVANCED;
					Read_Flash();
					ProcessNum=1;
					StepNum=1;
					CurrentProcess=1;
					CurrentStep=1;
		        	ReadProcessTime();
		        	ReadCycleTime();
		        	ReadStepTime();
		        	DisplayPage(LCD_STANDBY_PAGE);
		        	Display06page();
					break;

				case 0x10 :
					//Logout Popup
					DisplayPage(LCD_LOGOUT_POPUP);
					break;
				case 0x11 :
					//Logout yes
					//아이디 초기화
					CurrentUser=0;
					DisplayPage(LCD_FIRST_PAGE);
					break;
				case 0x12 :
					//Logout no
					DisplayPage(LCD_HOME_PAGE);
					break;
			}
			break;
    }
}
void LCD_06(int index, int value){	//input Value
    switch(index) {
		case 0x00 :
			switch(value) {
				case 0x01 :
					//GO TO HOME
					DisplayPage(LCD_HOME_PAGE);
					break;
			}
			break;
    }
    Display06page();
}
void LCD_07(int index, int value){	//input Value
    switch(index) {
		case 0x00 :
			switch(value) {
				case 0x01 :
					DisplayPage(LCD_STANDBY_PAGE);
					break;
			}
			break;
    }
}

void LCD_08(int index, int value){	//input Value
    switch(index) {
		case 0x00 :
			switch(value) {
				case 0x01 :
					//GO TO HOME
					DisplayPage(LCD_HOME_PAGE);
					break;
				case 0x02 :
					//START BUTTON
					//알람 관련 체크 후  Start
					if(Before_CycleAlarm_Check()==0){
						Start();
						DisplayPage(LCD_RUNNING_PAGE);
					}
					else{
						Alarm_Check();
						DisplayPage(LCD_STANDBY_ALRAM_PAGE);
					}

					break;
			}
			break;
    }
}

void LCD_09(int index, int value){	//input Value
    switch(index) {
		case 0x00 :
			switch(value) {
				case 0x01 :
					//MONITOR
					before_page=current_page;
					DisplayPage(LCD_MONITOR_PAGE);
					Display11page();
		        	DisplayInitTempGraph();
		        	DisplayInitVacuumGraph();

		        	DisplayTempGraph(DataCounter-1,0);
		        	DisplayVacuumGraph(DataCounter-1,1);
					break;
				case 0x02 :
					//STOP
					DisplayPage(LCD_SELECT_STOP_PAGE);
					break;
				case 0x03 :
					//ADVANCED MODE
					break;
			}
			break;
    }
}

void LCD_10(int index, int value){	//input Value
    switch(index) {
		case 0x00 :
			switch(value) {
				case 0x01 :
					Stop();
					break;
				case 0x02 :
					DisplayPage(LCD_RUNNING_PAGE);
					break;
			}
			break;
    }
}
void LCD_11(int index, int value){	//input Value
    switch(index) {
		case 0x00 :
			switch(value) {
				case 0x01 :
					DisplayPage(before_page);
					break;
			}
			break;
    }

}
void LCD_12(int index, int value){	//input Value
    switch(index) {
		case 0x00 :
			switch(value) {
				case 0x01 :
					//Monitor
					before_page=current_page;
		        	DisplayPage(LCD_MONITOR_PAGE);
		        	Display11page();
		        	DisplayInitTempGraph();
		        	DisplayInitVacuumGraph();
		        	DisplayTempGraph(DataCounter-1,0);
		        	DisplayVacuumGraph(DataCounter-1,1);
					break;
				case 0x02 :
		        	DisplayPage(LCD_HOME_PAGE);
					break;
				case 0x03 :
					//공정 결과 프린트
					CyclePrint();
					break;
			}
			break;
    }
}

void LCD_13(int index, int value){	//input Value
    switch(index) {
		case 0x00 :
			switch(value) {
				case 0x01 :
					//LCD_EORROR_WAIT_PAGE
					if(CurrentProcess==5){
						DisplayTime(0x14,0x01,ProcessTime[5]+ProcessTime[6]);
					}
					else if(CurrentProcess==6){
						DisplayTime(0x14,0x01,ProcessTime[6]);
					}
					DisplayPage(LCD_EORROR_WAIT_PAGE);
					break;
			}
			break;
    }
}

void LCD_14(int index, int value){	//input Value
    switch(index) {
		case 0x00 :
			switch(value) {
				case 0x01 :
					//Moniter
					before_page=current_page;
					DisplayPage(LCD_MONITOR_PAGE);
					Display11page();
		        	DisplayInitTempGraph();
		        	DisplayInitVacuumGraph();
		        	DisplayTempGraph(DataCounter-1,0);
		        	DisplayVacuumGraph(DataCounter-1,1);
					break;
			}
			break;
    }
}

void LCD_21(int index, int value){	//input Value
    switch(index) {
		case 0x00 :
			switch(value) {
				case 0x03 :
					printInformation();
					break;
			}
			break;
    }
}

void LCD_22(int index, int value){	//input Value
    switch(index) {
		case 0x00 :
			switch(value) {
				case 0x01 :
		        	InitRFID();
		        	ret1=ReadRFID();
		        	DisplayRFIDNumber();
		        	if(ret1==1){
		        		rfid_callback();
		        		Write_Flash();
		        		DisplayPage(current_page);
		        	}
					break;

				case 0x03 :
					if((int)flash_sterilant_production_year[RFIDData.currentID]==0){

					}
					else{
						printSterilant();
					}
					break;
			}
			break;
    }
    Display22page();
}

void LCD_23(int index, int value){	//input Value
    switch(index) {
		case 0x00 :
			switch(value) {
				case 0x01 :
					if(inputyear==0||inputmonth==0||inputday==0){

					}
					else{
						Read_Setting_Data_Flash();
						flash_load_index=0;
						DisplaySelectIcon(1,0);
						DisplaySelectIcon(2,0);
						DisplaySelectIcon(3,0);
						DisplaySelectIcon(4,0);
						DisplaySelectIcon(5,0);
					}
					DisplayPage(LCD_INFO_HISTORY_PAGE);
					break;

				case 0x03 :
					break;

				case 0x0A :
					if(load_data.cyclename!=0){
						LoadCyclePrint();
					}
					break;
				case 0x11 :
					flash_load_index=temptotalcycle[1];
					DisplaySelectIcon(1,1);
					DisplaySelectIcon(2,0);
					DisplaySelectIcon(3,0);
					DisplaySelectIcon(4,0);
					DisplaySelectIcon(5,0);
					//flash_load_index=1;
					Read_Data_Flash();
					DisplayPage(LCD_INFO_HISTORY_PAGE);
					break;
				case 0x12 :
					flash_load_index=temptotalcycle[2];
					DisplaySelectIcon(1,0);
					DisplaySelectIcon(2,1);
					DisplaySelectIcon(3,0);
					DisplaySelectIcon(4,0);
					DisplaySelectIcon(5,0);
					//flash_load_index=2;
					Read_Data_Flash();
					DisplayPage(LCD_INFO_HISTORY_PAGE);
					break;
				case 0x13 :
					flash_load_index=temptotalcycle[3];
					DisplaySelectIcon(1,0);
					DisplaySelectIcon(2,0);
					DisplaySelectIcon(3,1);
					DisplaySelectIcon(4,0);
					DisplaySelectIcon(5,0);
					//flash_load_index=3;
					Read_Data_Flash();
					DisplayPage(LCD_INFO_HISTORY_PAGE);
					break;
				case 0x14 :
					flash_load_index=temptotalcycle[4];
					DisplaySelectIcon(1,0);
					DisplaySelectIcon(2,0);
					DisplaySelectIcon(3,0);
					DisplaySelectIcon(4,1);
					DisplaySelectIcon(5,0);
					//flash_load_index=4;
					Read_Data_Flash();
					DisplayPage(LCD_INFO_HISTORY_PAGE);
					break;
				case 0x15 :
					flash_load_index=temptotalcycle[5];
					DisplaySelectIcon(1,0);
					DisplaySelectIcon(2,0);
					DisplaySelectIcon(3,0);
					DisplaySelectIcon(4,0);
					DisplaySelectIcon(5,1);
					Read_Data_Flash();
					DisplayPage(LCD_INFO_HISTORY_PAGE);
					break;
			}
			break;
		case 0xB0 :
			inputyear=value-2000;
			break;

		case 0xB5 :
			inputmonth=value;
			break;

		case 0xB9 :
			inputday=value;
			break;
    }
    Display23page();
}

void LCD_24(int index, int value){	//input Value
    switch(index) {
		case 0x00 :
			switch(value) {
				case 0x01 :
					//토글
					if(autoprint==1){
						autoprint=0;
					}
					else{
						autoprint=1;
					}
					break;
				case 0x02 :
					//
					break;
				case 0x03 :
					//
					break;
			}
			break;
    }
    Display24page();
}

void LCD_25(int index, int value){	//input Value
    switch(index) {
		case 0x00 :
			switch(value) {
				case 0x01 :
					TestModeStart(1);
					break;
				case 0x02 :
					if(TestMode==1){
						TestModeStop(1);
					}
					break;
				case 0x03 :
					if(TestCompleteflag==1){
						PrintLeakTest();
					}
					break;
			}
			break;
    }
    Display25page();

}
void LCD_26(int index, int value){	//input Value
    switch(index) {
		case 0x00 :
			switch(value) {
				case 0x01 :
					TestModeStart(2);
					DisplayPage4Char(0x26,0x10,"TEST");
					DisplayPage4Char(0x26,0x14,"TEST");
					DisplayPage4Char(0x26,0x18,"TEST");
					DisplayPage4Char(0x26,0x1C,"TEST");
					DisplayPage4Char(0x26,0x20,"TEST");
					break;
				case 0x02 :
					if(TestMode==2){
						TestModeStop(2);
						DisplayPage4Char(0x26,0x10,"");
						DisplayPage4Char(0x26,0x14,"");
						DisplayPage4Char(0x26,0x18,"");
						DisplayPage4Char(0x26,0x1C,"");
						DisplayPage4Char(0x26,0x20,"");
					}
					break;
				case 0x03 :
					if(TestCompleteflag==1){
						PrintHeaterTest();
					}
					break;
			}
			break;
    }
    Display26page();

}

void LCD_27(int index, int value){	//input Value
    switch(index) {
		case 0x00 :
			switch(value) {
				case 0x01 :
					TestModeStart(3);
					break;
				case 0x02 :
					if(TestMode==3){
						TestModeStop(3);
					}
					break;
				case 0x03 :
					if(TestCompleteflag==1){
						PrintPartsTest();
					}
					break;
			}
			break;
    }
    Display27page();

}

void LCD_28(int index, int value){	//input Value
    switch(index) {
		case 0x00 :
			switch(value) {
				case 0x01 :
					tempCarbonFilter=CarbonFilterMax;
					DisplayPageValue(0x28,0x40,tempCarbonFilter);
					break;
				case 0x02 :
					tempHEPAFilter=HEPAFilterMax;
					DisplayPageValue(0x28,0x50,tempHEPAFilter);
					break;
				case 0x03 :
					tempPlasmaAssy=PlasmaAssyMax;
					DisplayPageValue(0x28,0x60,tempPlasmaAssy);
					break;
				case 0x99 :
					CarbonFilterMax=tempCarbonFilterMax;
					HEPAFilterMax=tempHEPAFilterMax;
					PlasmaAssyMax=tempPlasmaAssyMax;
					CarbonFilter=tempCarbonFilter;
					HEPAFilter=tempHEPAFilter;
					PlasmaAssy=tempPlasmaAssy;
					Write_Flash();
					Display28page();
					DisplayPage(current_page);
					break;
			}
			break;

		case 0x10 :
			tempCarbonFilterMax=value;
			DisplayPageValue(0x28,0x10,tempCarbonFilterMax);
			break;
		case 0x20 :
			tempHEPAFilterMax=value;
			DisplayPageValue(0x28,0x20,tempHEPAFilterMax);
			break;
		case 0x30 :
			tempPlasmaAssyMax=value;
			DisplayPageValue(0x28,0x30,tempPlasmaAssyMax);
			break;

		case 0x40 :
			tempCarbonFilter=value;
			if(tempCarbonFilterMax<tempCarbonFilter){
				tempCarbonFilter=tempCarbonFilterMax;
			}
			DisplayPageValue(0x28,0x40,tempCarbonFilter);
			break;
		case 0x50 :
			tempHEPAFilter=value;
			if(tempHEPAFilterMax<tempHEPAFilter){
				tempHEPAFilter=tempHEPAFilterMax;
			}
			DisplayPageValue(0x28,0x50,tempHEPAFilter);
			break;
		case 0x60 :
			tempPlasmaAssy=value;
			if(tempPlasmaAssyMax<tempPlasmaAssy){
				tempPlasmaAssy=tempPlasmaAssyMax;
			}
			DisplayPageValue(0x28,0x60,tempPlasmaAssy);
			break;
    }
}

void LCD_30(int index, int value){	//input Value
    switch(index) {
		case 0x00 :
			switch(value) {
				case 0x90 :
					W25Q16_EraseSector(0);
					W25Q16_EraseSector(4096*1);
					W25Q16_EraseSector(4096*2);
					W25Q16_EraseSector(4096*3);
					W25Q16_EraseSector(4096*4);
					W25Q16_EraseSector(4096*5);
					break;
				case 0x99 :
					ReadInforDataFromLCD();
					Write_Flash();
					DisplayPage(current_page);
					break;
			}
			break;
		case 0x01 : // TestValue
			TestVacuumValue=value/10;
			break;
		case 0x05 : // 리크 률
			TestLeakValue=value/10;
			break;
		case 0x09 : // 에러 온도 률
			TestTempErrorValue=value;
			break;
    }
    //Display30page();
}

void LCD_31(int index, int value){	//input Value
    switch(index) {
		case 0x00 :
			switch(value) {
				case 0x99 :
					Write_Flash();
					break;
			}
			break;
		case 0x21 :
			CalibrationTemp[0]=value/10;
			break;
		case 0x25 :
			CalibrationTemp[1]=value/10;
			break;
		case 0x29 :
			CalibrationTemp[2]=value/10;
			break;
		case 0x2D :
			CalibrationTemp[3]=value/10;
			break;
		case 0x35 :
			CalibrationVacuum=(float)value/10;
			break;

    }
    Display31page();

}

void LCD_33(int index, int value){	//input Value
    switch(index) {
    	case 0x01 : // Set Chamber Temp	//대기
    		FlashSettingTemp[0][0]=(float)value/10;
            break;
    	case 0x05 : // Set Chamber Temp	//공정
    		FlashSettingTemp[0][1]=(float)value/10;
            break;
    	case 0x09 : // Set Chamber Temp	//슬립
    		FlashSettingTemp[0][2]=(float)value/10;
            break;


    	case 0x11 : // Set Chamber Temp
    		FlashSettingTemp[1][0]=(float)value/10;
            break;
    	case 0x15 : // Set Chamber Temp
    		FlashSettingTemp[1][1]=(float)value/10;
            break;
    	case 0x19 : // Set Chamber Temp
    		FlashSettingTemp[1][2]=(float)value/10;
            break;


    	case 0x21 : // Set Chamber Temp
    		FlashSettingTemp[2][0]=(float)value/10;
			break;
    	case 0x25 : // Set Chamber Temp
    		FlashSettingTemp[2][1]=(float)value/10;
			break;
    	case 0x29 : // Set Chamber Temp
    		FlashSettingTemp[2][2]=(float)value/10;
			break;

    	case 0x31 : // Set Chamber Temp
    		FlashSettingTemp[3][0]=(float)value/10;
			break;
    	case 0x35 : // Set Chamber Temp
    		FlashSettingTemp[3][1]=(float)value/10;
			break;
    	case 0x39 : // Set Chamber Temp
    		FlashSettingTemp[3][2]=(float)value/10;
			break;

    	case 0x41 : // Pressure condition1
    		FlashPreesureCondition[0]=(float)value/10;
			break;
    	case 0x45 : // Pressure condition2
    		FlashPreesureCondition[1]=(float)value/10;
			break;
    	case 0x49 : // Pressure condition3
    		FlashPreesureCondition[2]=(float)value/10;
			break;

    	case 0x51 : // Plasma on time
    		FlashPlasmaTime[0]=(float)value;
			break;
    	case 0x55 : // Plasma off time
    		FlashPlasmaTime[1]=(float)value;
			break;
    }
    //DisplayProcessSettingValues();
    //Display33page();
}

void LCD_34(int index, int value){	//input Value
	switch(index) {
		case 0x00 :
			switch(value) {
				case 0x01:
					ProcessNum=1;
					DisplayPage(LCD_FACTORY_TESTMODE_PROCESSSETTINGS1_PAGE);
					StepNum=0;
					break;
				case 0x02:
					ProcessNum=2;
					DisplayPage(LCD_FACTORY_TESTMODE_PROCESSSETTINGS1_PAGE);
					StepNum=0;
					break;
				case 0x03:
					ProcessNum=3;
					DisplayPage(LCD_FACTORY_TESTMODE_PROCESSSETTINGS1_PAGE);

					StepNum=0;
					break;
				case 0x04:
					ProcessNum=4;
					DisplayPage(LCD_FACTORY_TESTMODE_PROCESSSETTINGS1_PAGE);

					StepNum=0;
					break;
				case 0x05:
					ProcessNum=5;
					DisplayPage(LCD_FACTORY_TESTMODE_PROCESSSETTINGS1_PAGE);

					StepNum=0;
					break;
				case 0x06:
					ProcessNum=6;
					DisplayPage(LCD_FACTORY_TESTMODE_PROCESSSETTINGS1_PAGE);
					StepNum=0;
					break;


				case 0x11:
					StepNum=1;
					break;
				case 0x12:
					StepNum=2;
					break;
				case 0x13:
					StepNum=3;
					break;
				case 0x14:
					StepNum=4;
					break;
				case 0x15:
					StepNum=5;
					break;
				case 0x16:
					StepNum=6;
					break;
				case 0x17:
					StepNum=7;
					break;
				case 0x18:
					StepNum=8;
					break;
				case 0x19:
					StepNum=9;
					break;
				case 0x1A:
					StepNum=10;
					break;
				case 0x21:
					StepNum=11;
					break;
				case 0x22:
					StepNum=12;
					break;
				case 0x23:
					StepNum=13;
					break;
				case 0x24:
					StepNum=14;
					break;
				case 0x25:
					StepNum=15;
					break;
				case 0x26:
					StepNum=16;
					break;
				case 0x27:
					StepNum=17;
					break;
				case 0x28:
					StepNum=18;
					break;
				case 0x29:
					StepNum=19;
					break;
				case 0x2A:
					StepNum=20;
					break;


				case 0x41:
					if((CycleData[ProcessNum][StepNum].PartsSetting&0x01)==0x01){
						CycleData[ProcessNum][StepNum].PartsSetting=CycleData[ProcessNum][StepNum].PartsSetting-0x01;
					}
					else{
						CycleData[ProcessNum][StepNum].PartsSetting=CycleData[ProcessNum][StepNum].PartsSetting+0x01;
					}
					break;
				case 0x42:
					if((CycleData[ProcessNum][StepNum].PartsSetting&0x02)==0x02){
						CycleData[ProcessNum][StepNum].PartsSetting=CycleData[ProcessNum][StepNum].PartsSetting-0x02;
					}
					else{
						CycleData[ProcessNum][StepNum].PartsSetting=CycleData[ProcessNum][StepNum].PartsSetting+0x02;
					}
					break;
				case 0x43:
					if((CycleData[ProcessNum][StepNum].PartsSetting&0x04)==0x04){
						CycleData[ProcessNum][StepNum].PartsSetting=CycleData[ProcessNum][StepNum].PartsSetting-0x04;
					}
					else{
						CycleData[ProcessNum][StepNum].PartsSetting=CycleData[ProcessNum][StepNum].PartsSetting+0x04;
					}
					break;
				case 0x44:
					if((CycleData[ProcessNum][StepNum].PartsSetting&0x08)==0x08){
						CycleData[ProcessNum][StepNum].PartsSetting=CycleData[ProcessNum][StepNum].PartsSetting-0x08;
					}
					else{
						CycleData[ProcessNum][StepNum].PartsSetting=CycleData[ProcessNum][StepNum].PartsSetting+0x08;
					}
					break;
				case 0x45:
					if((CycleData[ProcessNum][StepNum].PartsSetting&0x10)==0x10){
						CycleData[ProcessNum][StepNum].PartsSetting=CycleData[ProcessNum][StepNum].PartsSetting-0x10;
					}
					else{
						CycleData[ProcessNum][StepNum].PartsSetting=CycleData[ProcessNum][StepNum].PartsSetting+0x10;
					}
					break;
				case 0x46:	//NO check	0x00
					CycleData[ProcessNum][StepNum].PartsSetting=CycleData[ProcessNum][StepNum].PartsSetting-(CycleData[ProcessNum][StepNum].PartsSetting&0x60);
					break;
				case 0x47:	//0x20
					CycleData[ProcessNum][StepNum].PartsSetting=CycleData[ProcessNum][StepNum].PartsSetting-(CycleData[ProcessNum][StepNum].PartsSetting&0x60)+0x20;
					break;
				case 0x48:	//0x40
					CycleData[ProcessNum][StepNum].PartsSetting=CycleData[ProcessNum][StepNum].PartsSetting-(CycleData[ProcessNum][StepNum].PartsSetting&0x60)+0x40;
					break;
				case 0x49:	//0x60
					CycleData[ProcessNum][StepNum].PartsSetting=CycleData[ProcessNum][StepNum].PartsSetting-(CycleData[ProcessNum][StepNum].PartsSetting&0x60)+0x60;
					break;
				case 0x50 :	//Cycle 선택 팝업
					Display44page();
					before_page=current_page;
					DisplayPage(LCD_FACTORY_TESTMODE_SELECT_CYCLE_PAGE);
					break;
				case 0x51:
					DisplayPage(LCD_FACTORY_TESTMODE_PROCESSSETTINGS2_PAGE);
					StepNum=11;
					Display35page();
					break;

		        case 0x55:
					//testbit=CycleData[ProcessNum][StepNum].PartsSetting;
		        	//센서 세팅 값에서 저장된 값 저장시
		        	if(FlashSettingTemp[0][0]!=0){
						for(int i=0;i<3;i++){
							DoorSettingTemp[i]=FlashSettingTemp[0][i];
							ChamberSettingTemp[i]=FlashSettingTemp[1][i];
							ChamberBackSettingTemp[i]=FlashSettingTemp[2][i];
							VaporizerSettingTemp[i]=FlashSettingTemp[3][i];
						}
		        	}
		        	if(FlashPreesureCondition[0]!=0){
		        		PreesureCondition[0]=FlashPreesureCondition[0];
		        		PreesureCondition[1]=FlashPreesureCondition[1];
		        		PreesureCondition[2]=FlashPreesureCondition[2];
		        	}
		        	if(FlashPlasmaTime[0]!=0){
		        		PlasmaTime[0]=FlashPlasmaTime[0];
		        		PlasmaTime[1]=FlashPlasmaTime[1];
		        	}
		        	Write_Flash();
		        	DisplayProcessSettingValues();
		        	DisplayPage(current_page);
					break;
			}
			break;
		case 0x51 : // Time 시간 저장
			if(StepNum==0){
				CycleData[ProcessNum][StepNum].Time=0;
			}
			else{
				CycleData[ProcessNum][StepNum].Time=(float)value;
			}
			break;

	}

	Display34page();
}
void LCD_35(int index, int value){	//input Value
	switch(index) {
		case 0x00 :
			switch(value) {
				case 0x50 : // Cycle 선택
					Display44page();
					before_page=current_page;
					DisplayPage(LCD_FACTORY_TESTMODE_SELECT_CYCLE_PAGE);
					break;
				case 0x51:
		        	DisplayPage(LCD_FACTORY_TESTMODE_PROCESSSETTINGS1_PAGE);
		        	StepNum=1;
					Display34page();
		        	break;
		        case 0x55:
					//testbit=CycleData[ProcessNum][StepNum].PartsSetting;
		        	//센서 세팅 값에서 저장된 값 저장시
		        	if(FlashSettingTemp[0][0]!=0){
						for(int i=0;i<3;i++){
							DoorSettingTemp[i]=FlashSettingTemp[0][i];
							ChamberSettingTemp[i]=FlashSettingTemp[1][i];
							ChamberBackSettingTemp[i]=FlashSettingTemp[2][i];
							VaporizerSettingTemp[i]=FlashSettingTemp[3][i];
						}
		        	}
		        	if(FlashPreesureCondition[0]!=0){
		        		PreesureCondition[0]=FlashPreesureCondition[0];
		        		PreesureCondition[1]=FlashPreesureCondition[1];
		        		PreesureCondition[2]=FlashPreesureCondition[2];
		        	}
		        	if(FlashPlasmaTime[0]!=0){
		        		PlasmaTime[0]=FlashPlasmaTime[0];
		        		PlasmaTime[1]=FlashPlasmaTime[1];
		        	}
		        	Write_Flash();
		        	DisplayProcessSettingValues();
		        	DisplayPage(current_page);
					break;
			}
			break;
		case 0x01 :
			break;

	}
	Display35page();
}
void LCD_36(int index, int value){	//input Value
	switch(index) {
		case 0x00 :
			switch(value) {
				case 0x50 : // Cycle 선택
					Display44page();
					before_page=current_page;
					DisplayPage(LCD_FACTORY_TESTMODE_SELECT_CYCLE_PAGE);
					break;
				case 0x51:
		        	DisplayPage(LCD_FACTORY_TESTMODE_PROCESSTEST4_PAGE);
		        	StepNum=11;
					Display37page();
		        	break;
		        case 0x55:	//TEST START
		        	ReadProcessTime();
		        	ReadCycleTime();
		        	ReadStepTime();
		        	FactoryTestStart();
		        	DisplayPage(LCD_FACTORY_TESTMODE_PROCESSTEST3_PAGE);
		        	DisplayProcessTestValues();
					break;
			}
			break;
		case 0x01 :
			break;

	}
	Display36page();
}
void LCD_37(int index, int value){	//input Value
	switch(index) {
		case 0x00 :
			switch(value) {
				case 0x50 : // Cycle 선택
					Display44page();
					before_page=current_page;
					DisplayPage(LCD_FACTORY_TESTMODE_SELECT_CYCLE_PAGE);
					break;
				case 0x51:
					DisplayPage(LCD_FACTORY_TESTMODE_PROCESSTEST3_PAGE);
					StepNum=1;
					Display36page();
		        case 0x55:	//TEST START
		        	ReadProcessTime();
		        	ReadCycleTime();
		        	ReadStepTime();
		        	FactoryTestStart();
		        	DisplayPage(LCD_FACTORY_TESTMODE_PROCESSTEST3_PAGE);
		        	ReadProcessTime();
		        	ReadStepTime();
		        	DisplayProcessTestValues();
					break;
			}
			break;
		case 0x01 :
			break;

	}
	Display37page();
}
void LCD_38(int index, int value){	//input Value
	switch(index) {
		case 0x00 :
			switch(value) {
				case 0x50 : // Cycle 선택
					Display44page();
					before_page=current_page;
					DisplayPage(LCD_FACTORY_TESTMODE_SELECT_CYCLE_PAGE);
					break;
		        case 0x55:	//TEST STOP
		        	ReadProcessTime();
		        	ReadCycleTime();
		        	ReadStepTime();
		        	FactoryTestStop();
		        	DisplayPage(LCD_FACTORY_TESTMODE_PROCESSTEST1_PAGE);
		        	DisplayProcessTestValues();
					break;
			}
			break;
		case 0x01 :
			break;

	}
	Display38page();
}
void LCD_39(int index, int value){	//input Value
	switch(index) {
		case 0x00 :
			switch(value) {
				case 0x50 : // Cycle 선택
					Display44page();
					before_page=current_page;
					DisplayPage(LCD_FACTORY_TESTMODE_SELECT_CYCLE_PAGE);
					break;
		        case 0x55:	//TEST STOP
		        	ReadProcessTime();
		        	ReadCycleTime();
		        	ReadStepTime();
		        	FactoryTestStop();
		        	DisplayPage(LCD_FACTORY_TESTMODE_PROCESSTEST1_PAGE);
		        	DisplayProcessTestValues();
					break;
			}
			break;
		case 0x01 :
			break;

	}
	Display39page();
}

void LCD_44(int index, int value){	//input Value
	switch(index) {
		case 0x00 :
			switch(value) {
				case 0x01 : // Cycle 선택
					CycleName=SHORT;
					Read_Flash();
					ProcessNum=1;
					StepNum=1;
					CurrentProcess=1;
					CurrentStep=1;
		        	ReadProcessTime();
		        	ReadCycleTime();
		        	ReadStepTime();
		        	DisplayPage(before_page);
					Display34page();
					break;
				case 0x02 : // Cycle 선택
					CycleName=STANDARD;
					Read_Flash();
					ProcessNum=1;
					StepNum=1;
					CurrentProcess=1;
					CurrentStep=1;
		        	ReadProcessTime();
		        	ReadCycleTime();
		        	ReadStepTime();
					DisplayPage(before_page);
					Display34page();
					break;
				case 0x03 : // Cycle 선택
					CycleName=ADVANCED;
					Read_Flash();
					ProcessNum=1;
					StepNum=1;
					CurrentProcess=1;
					CurrentStep=1;
		        	ReadProcessTime();
		        	ReadCycleTime();
		        	ReadStepTime();
					DisplayPage(before_page);
					Display34page();
					break;
			}
			break;
		case 0x01 : // Cycle 선택
			break;

	}

	Display44page();
}

void LCD_60(int index, int value){	//input Value
	switch(index) {
		case 0x00 :
			switch(value) {
				//AC Test
				case 0x01 :
					if(!HAL_GPIO_ReadPin(GPIO_OUT11_GPIO_Port, GPIO_OUT11_Pin)){
						AC1(0);
		        		DisplayIcon(0x6A,0x10,0);
					}
					else{
						AC1(1);
		        		DisplayIcon(0x6A,0x10,1);
					}
					break;
				case 0x02 :
					if(!HAL_GPIO_ReadPin(GPIO_OUT12_GPIO_Port, GPIO_OUT12_Pin)){
						AC2(0);
		        		DisplayIcon(0x6A,0x20,0);
					}
					else{
						AC2(1);
		        		DisplayIcon(0x6A,0x20,1);
					}
					break;
				case 0x03 :
					if(!HAL_GPIO_ReadPin(GPIO_OUT13_GPIO_Port, GPIO_OUT13_Pin)){
						AC3(0);
		        		DisplayIcon(0x6A,0x30,0);
					}
					else{
						AC3(1);
		        		DisplayIcon(0x6A,0x30,1);
					}
					break;
				case 0x04 :
					if(!HAL_GPIO_ReadPin(GPIO_OUT14_GPIO_Port, GPIO_OUT14_Pin)){
						AC4(0);
		        		DisplayIcon(0x6A,0x40,0);
					}
					else{
						AC4(1);
		        		DisplayIcon(0x6A,0x40,1);
					}
					break;
				case 0x05 :
					if(!HAL_GPIO_ReadPin(GPIO_OUT15_GPIO_Port, GPIO_OUT15_Pin)){
						AC5(0);
		        		DisplayIcon(0x6A,0x50,0);
					}
					else{
						AC5(1);
		        		DisplayIcon(0x6A,0x50,1);
					}
					break;
				case 0x06 :
					if(!HAL_GPIO_ReadPin(GPIO_OUT16_GPIO_Port, GPIO_OUT16_Pin)){
						//AC6(0);
						VacuumPump(0);
		        		DisplayIcon(0x6A,0x60,0);
					}
					else{
						//AC6(1);
						VacuumPump(1);
		        		DisplayIcon(0x6A,0x60,1);
					}
					break;
				case 0x07 :
					if(!HAL_GPIO_ReadPin(GPIO_OUT17_GPIO_Port, GPIO_OUT17_Pin)){
						AC7(0);
		        		DisplayIcon(0x6A,0x70,0);
					}
					else{
						AC7(1);
		        		DisplayIcon(0x6A,0x70,1);
					}
					break;
				case 0x08 :
					if(!HAL_GPIO_ReadPin(GPIO_OUT18_GPIO_Port, GPIO_OUT18_Pin)){
						AC8(0);
		        		DisplayIcon(0x6A,0x80,0);
					}
					else{
						AC8(1);
		        		DisplayIcon(0x6A,0x80,1);
					}
					break;

				//DC Test
				case 0x09 :
					if(HAL_GPIO_ReadPin(GPIO_OUT1_GPIO_Port, GPIO_OUT1_Pin)){
						DC1(0);
		        		DisplayIcon(0x6B,0x10,0);
					}
					else{
						DC1(1);
		        		DisplayIcon(0x6B,0x10,1);
					}
					break;
				case 0x0A :
					if(HAL_GPIO_ReadPin(GPIO_OUT2_GPIO_Port, GPIO_OUT2_Pin)){
						DC2(0);
		        		DisplayIcon(0x6B,0x20,0);
					}
					else{
						DC2(1);
		        		DisplayIcon(0x6B,0x20,1);
					}
					break;
				case 0x0B :
					if(HAL_GPIO_ReadPin(GPIO_OUT3_GPIO_Port, GPIO_OUT3_Pin)){
						DC3(0);
		        		DisplayIcon(0x6B,0x30,0);
					}
					else{
						DC3(1);
		        		DisplayIcon(0x6B,0x30,1);
					}
					break;
				case 0x0C :
					if(HAL_GPIO_ReadPin(GPIO_OUT4_GPIO_Port, GPIO_OUT4_Pin)){
						DC4(0);
		        		DisplayIcon(0x6B,0x40,0);
					}
					else{
						DC4(1);
		        		DisplayIcon(0x6B,0x40,1);
					}
					break;
				case 0x0D :
					if(HAL_GPIO_ReadPin(GPIO_OUT5_GPIO_Port, GPIO_OUT5_Pin)){
						DC5(0);
		        		DisplayIcon(0x6B,0x50,0);
					}
					else{
						DC5(1);
		        		DisplayIcon(0x6B,0x50,1);
					}
					break;
				case 0x0E :
					if(HAL_GPIO_ReadPin(GPIO_OUT6_GPIO_Port, GPIO_OUT6_Pin)){
						DC6(0);
		        		DisplayIcon(0x6B,0x60,0);
					}
					else{
						DC6(1);
		        		DisplayIcon(0x6B,0x60,1);
					}
					break;
				case 0x0F :
					if(HAL_GPIO_ReadPin(GPIO_OUT7_GPIO_Port, GPIO_OUT7_Pin)){
						DC7(0);
		        		DisplayIcon(0x6B,0x70,0);
					}
					else{
						DC7(1);
		        		DisplayIcon(0x6B,0x70,1);
					}
					break;
				case 0x10 :
					if(HAL_GPIO_ReadPin(GPIO_OUT8_GPIO_Port, GPIO_OUT8_Pin)){
						DC8(0);
		        		DisplayIcon(0x6B,0x80,0);
					}
					else{
						DC8(1);
		        		DisplayIcon(0x6B,0x80,1);
					}
					break;
				case 0x11 :
					if(HAL_GPIO_ReadPin(GPIO_OUT26_GPIO_Port, GPIO_OUT26_Pin)){
						//페리펌프
		        		DisplayIcon(0x6B,0x90,0);
					}
					else{
						//페리펌프
		        		DisplayIcon(0x6B,0x90,1);
					}
					break;
			}
			break;
		case 0x01 : // Cycle 선택
			break;

	}

	//Display60page();
}


void DoActionButton(int key){	//00xx
    switch(key) {
    	case 0:
            break;
        case 1: // goto login page
        	DisplayPage(LCD_LOGIN_PAGE);
        	Display02page();
        	//fortest
/*
        	DisplayPage(LCD_MONITOR_PAGE);
        	PressureData[0]=750;
        	TemperatureData[0]=50;
        	for(int i=1;i<=270;i++){
        		TemperatureData[i]=i;
        		if(TemperatureData[i]>=80){
        			TemperatureData[i]=80;
        		}
        		PressureData[i]=650;
        	}
        	DisplayInitTempGraph();
        	DisplayInitVacuumGraph();

        	DisplayTempGraph(270-1,0);
        	DisplayVacuumGraph(270-1,1);
*/
			//DisplayVacuumGraph(200,1);
        	//DisplayVacuumGraph(10,1);


        	//DisplayDot(0x40,0x00,1,1);

        	/*
        	for(int i=0;i<300;i++){
        		DisplayDot(0x40,0x00,130+i,130+i);
        	}
        	*/
        	//DisplayDot(0x40,0x00,300,300);

            break;
        case 2:	//LOGIN BUTTON
        	DisplayPage(LCD_HOME_PAGE);
        	break;

        case 0x11:
        	DisplayPage(LCD_INFO_INFORMATION_PAGE);
        	Display21page();
        	break;
        case 0x12:
        	DisplayPage(LCD_INFO_STERILANT_PAGE);
        	Display22page();
        	break;
        case 0x13:
        	DisplayPage(LCD_INFO_HISTORY_PAGE);
        	inputyear=0;
        	inputmonth=0;
        	inputday=0;
        	DisplayPageValue(0x23,0xB0,inputyear);
        	DisplayPageValue(0x23,0xB5,inputmonth);
        	DisplayPageValue(0x23,0xB9,inputday);
        	DisplaySelectIcon(1,0);
			DisplaySelectIcon(2,0);
			DisplaySelectIcon(3,0);
			DisplaySelectIcon(4,0);
			DisplaySelectIcon(5,0);

        	Display23page();
        	break;
        case 0x21:
        	DisplayPage(LCD_USER_SETTING_PAGE);
        	Display24page();
        	break;
        case 0x22:
        	TestCompleteflag=0;
        	Display25page();
        	break;
        case 0x23:
        	TestCompleteflag=0;
        	Display26page();
        	break;
        case 0x24:
        	TestCompleteflag=0;
        	Display27page();
			break;

        case 0x31:
        	DisplayPage(LCD_ADMIN_PMSCHEDULE_PAGE);
        	Display28page();
        	break;
        case 0x32:
			DisplayPage(LCD_ADMIN_PARTSTEST_PAGE);
			DisplayRFIDNumber();


			DisplayPartsTESTIcon(1,VacuumPump_flag);
			DisplayPartsTESTIcon(2,VacuumValve_flag);
			DisplayPartsTESTIcon(3,VentValve_flag);
			DisplayPartsTESTIcon(4,InjectionValve_flag);

			/*
			VacuumPump(0);
			InjectionValve(0);
			VacuumValve(0);
			VentValve(0);


        	VacuumPump_flag=0;
        	VacuumValve_flag=0;
        	InjectionValve_flag=0;
        	VentValve_flag=0;

        	화면이동간 확인 필요
*/
			break;


        case 0x41:
        	Display30page();
        	DisplayPage(LCD_FACTORY_INFOSETTING_PAGE);
        	break;
        case 0x42:
            Display31page();
        	break;

        case 0x43:
        	DisplayPage(LCD_FACTORY_TESTMODE_PAGE);
        	break;


        case 0x44:	//LCD_FACTORY_TEMPSETTING_PAGE
        	//여기
        	Read_Flash();
        	DisplayTempValues();
        	Display33page();
        	break;



        case 0x45:
        	Display34page();
        	DisplayPage(LCD_FACTORY_TESTMODE_PROCESSSETTINGS1_PAGE);
        	CycleName=SHORT;
        	ProcessNum=1;
        	StepNum=1;
        	Read_Flash();
        	DisplayProcessSettingValues();
        	break;

        case 0x46:
        	DisplayPage(LCD_FACTORY_TESTMODE_PROCESSTEST1_PAGE);
        	CycleName=SHORT;
        	ProcessNum=1;
        	StepNum=1;
        	CurrentProcess=1;
        	CurrentStep=1;
        	Read_Flash();
        	ReadProcessTime();
        	ReadCycleTime();
        	ReadStepTime();
        	DisplayProcessTestValues();
        	break;

        case 0x47:
        	HeaterControlMode=0;
        	Inithardware();
        	VacuumPump_flag=0;
        	Plasma_flag=0;
        	DoorLatch_flag=0;
        	VacuumValve_flag=0;
        	CirculationValve_flag=0;
        	InhalationValve_flag=0;
        	VentValve_flag=0;
        	InjectionValve_flag=0;
        	Fan_flag=0;
        	//DisplayPage(LCD_FACTORY_CONTROLTEST_PAGE);
        	//여기 테스트중
        	DisplayPage(60);

        	break;

        case 0x99:
        	HeaterControlMode=1;
        	DisplayPage(LCD_FACTORY_TESTMODE_PAGE);
        	Inithardware();
        	VacuumPump_flag=0;
        	Plasma_flag=0;

        	DoorHeater_flag=0;
        	ChamberHeater_flag=0;
        	ChamberBackHeater_flag=0;
        	VaporizerHeater_flag=0;
        	CirculationHeater_flag=0;

        	DoorLatch_flag=0;
        	VacuumValve_flag=0;
        	CirculationValve_flag=0;
        	InhalationValve_flag=0;
        	VentValve_flag=0;
        	InjectionValve_flag=0;
        	Fan_flag=0;

        	break;

        case 0xFF:
        	if(Running_Flag==0){
        		DoorLatch_flag=1;
        	}
        	break;


    }
    //HAL_UART_Receive_IT(LCD_USART, (uint8_t*)LCD_rx_data, 9);
}



//Process Setting Page
void ProcessSettingButton(int key){	//03XX
    switch(key) {
        case 0x55:
			//testbit=CycleData[ProcessNum][StepNum].PartsSetting;
        	//센서 세팅 값에서 저장된 값 저장시
        	if(FlashSettingTemp[0][0]!=0){
				for(int i=0;i<3;i++){
					DoorSettingTemp[i]=FlashSettingTemp[0][i];
					ChamberSettingTemp[i]=FlashSettingTemp[1][i];
					ChamberBackSettingTemp[i]=FlashSettingTemp[2][i];
					VaporizerSettingTemp[i]=FlashSettingTemp[3][i];
				}
        	}
        	if(FlashPreesureCondition[0]!=0){
        		PreesureCondition[0]=FlashPreesureCondition[0];
        		PreesureCondition[1]=FlashPreesureCondition[1];
        		PreesureCondition[2]=FlashPreesureCondition[2];
        	}
        	if(FlashPlasmaTime[0]!=0){
        		PlasmaTime[0]=FlashPlasmaTime[0];
        		PlasmaTime[1]=FlashPlasmaTime[1];
        	}
        	Write_Flash();
        	DisplayProcessSettingValues();
        	DisplayPage(current_page);
			break;
        case 0x60:	//TEST START
        	ReadProcessTime();
        	ReadCycleTime();
        	ReadStepTime();
        	Start();
        	DisplayPage(LCD_FACTORY_TESTMODE_PROCESSTEST3_PAGE);
        	ReadProcessTime();
        	ReadStepTime();
        	DisplayProcessTestValues();
			break;

        case 0x61:
        	DisplayPage(LCD_FACTORY_TESTMODE_PROCESSTEST1_PAGE);
        	StepNum=0;
        	DisplayProcessSettingValues();
        	break;

        case 0x62:
        	DisplayPage(LCD_FACTORY_TESTMODE_PROCESSTEST2_PAGE);
        	StepNum=11;
        	DisplayProcessSettingValues();
			break;

        case 0x65:	//TEST STOP
        	ReadProcessTime();
        	ReadCycleTime();
        	ReadStepTime();
        	Stop();
        	DisplayPage(LCD_FACTORY_TESTMODE_PROCESSTEST1_PAGE);
        	DisplayProcessTestValues();
			break;

    }
    DisplayProcessSettingValues();
    //HAL_UART_Receive_IT(LCD_USART, (uint8_t*)LCD_rx_data, 9);
}



//ICON Display
unsigned char   icon_display[8] = {0x5A, 0xA5, 0x05, 0x82, 0x00, 0x00, 0x00, 0x01};

unsigned char   icon_process_index[7] = {0x00,0x10,0x20,0x30,0x40,0x50,0x60};
unsigned char   icon_step_index[11] = {0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,0x80,0x90,0xA0};
unsigned char   icon_parts_index[10] = {0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,0x80,0x90};
unsigned char   icon_select_index[6] = {0x00,0x10,0x20,0x30,0x40,0x50};
/*
 * 1.Vacuum Valve
 * 2.Vent Valve
 * 3.Injection Valve
 * 4.Peri Pump
 * 5.Plasma
 *
 * Vacuum Check
 * 6.No check
 * 7.Value1
 * 8.Value2
 * 9.Value3
 * */


unsigned char   icon_partstest_index[10] = {0x00,0x10,0x20,0x30,0x40,0x50,0x60,0x70,0x80,0x90};

/*
* 1.Vacuum Pump
* 2.Vacuum Valve
* 3.Vent Valve
* 4.Injection Valve

5. Door handle
6. Door Latch
7. Bottle
8. Bottle Door
9. Liquid Level
*/

void DisplayProcessIcon(int index, int value){
	icon_display[4] = 0x6F;
	icon_display[5] = icon_process_index[index];
    icon_display[7] = value;
    HAL_UART_Transmit(LCD_USART, icon_display, 8, 10);
}
void DisplayStepIcon(int index, int value){
	icon_display[4] = 0x6E;
	icon_display[5] = icon_step_index[index];
	icon_display[7] = value;
	HAL_UART_Transmit(LCD_USART, icon_display, 8, 10);
}
void DisplayPartsIcon(int index, int value){
	icon_display[4] = 0x6D;
	icon_display[5] = icon_parts_index[index];
	icon_display[7] = value;
	HAL_UART_Transmit(LCD_USART, icon_display, 8, 10);
}
void DisplaySelectIcon(int index, int value){
	icon_display[4] = 0x21;
	icon_display[5] = icon_select_index[index];
	icon_display[7] = value;
	HAL_UART_Transmit(LCD_USART, icon_display, 8, 10);
}
void DisplayIcon(int page, int index, int value){
	unsigned char   icondisplay[8] = {0x5A, 0xA5, 0x05, 0x82, 0x00, 0x00, 0x00, 0x01};
	icondisplay[4] = page;
	icondisplay[5] = index;
	icondisplay[7] = value;
	HAL_UART_Transmit(LCD_USART, icondisplay, 8, 10);
}



void DisplayPartsTESTIcon(int index, int value){
	icon_display[4] = 0x5F;
	icon_display[5] = icon_partstest_index[index];
	icon_display[7] = value;
	HAL_UART_Transmit(LCD_USART, icon_display, 8, 10);
}
void DisplayProcessIcons(int index){
	for(int i=1;i<7;i++){
		DisplayProcessIcon(i,0);
	}
	DisplayProcessIcon(index,1);
}
void DisplayStepIcons(int index){
	for(int i=1;i<11;i++){
		DisplayStepIcon(i,0);
	}
	if(index>10){
		DisplayStepIcon(index-10,1);
	}
	else{
		DisplayStepIcon(index,1);
	}
}
void DisplayPartsIcons(){
	DisplayPartsIcon(1,(CycleData[ProcessNum][StepNum].PartsSetting&0x01)==0x01);
	DisplayPartsIcon(2,(CycleData[ProcessNum][StepNum].PartsSetting&0x02)==0x02);
	DisplayPartsIcon(3,(CycleData[ProcessNum][StepNum].PartsSetting&0x04)==0x04);
	DisplayPartsIcon(4,(CycleData[ProcessNum][StepNum].PartsSetting&0x08)==0x08);
	DisplayPartsIcon(5,(CycleData[ProcessNum][StepNum].PartsSetting&0x10)==0x10);
	if((CycleData[ProcessNum][StepNum].PartsSetting&0x60)==0x60){
		DisplayPartsIcon(6,0);
		DisplayPartsIcon(7,0);
		DisplayPartsIcon(8,0);
		DisplayPartsIcon(9,1);
	}
	else if((CycleData[ProcessNum][StepNum].PartsSetting&0x60)==0x40){
		DisplayPartsIcon(6,0);
		DisplayPartsIcon(7,0);
		DisplayPartsIcon(8,1);
		DisplayPartsIcon(9,0);
	}
	else if((CycleData[ProcessNum][StepNum].PartsSetting&0x60)==0x20){
		DisplayPartsIcon(6,0);
		DisplayPartsIcon(7,1);
		DisplayPartsIcon(8,0);
		DisplayPartsIcon(9,0);
	}
	else if((CycleData[ProcessNum][StepNum].PartsSetting&0x60)==0x00){
		DisplayPartsIcon(6,1);
		DisplayPartsIcon(7,0);
		DisplayPartsIcon(8,0);
		DisplayPartsIcon(9,0);
	}
}


void DisplayProcessSettingValues(){
	DisplayCycleValue(CycleName);
	DisplayProcessIcons(ProcessNum);
	DisplayStepIcons(StepNum);
	DisplayPartsIcons();
	DisplayTimeValues();
}

void DisplayProcessTestValues(){
	DisplayCycleValue(CycleName);
	DisplayProcessIcons(CurrentProcess);
	DisplayStepIcons(CurrentStep);
	DisplayPartsIcons();
	DisplayTimeValues();
}
void DisplayNormalValues(){

	if(CycleName==1){
		DisplayPage8Char(0x11,0x01," SHORT  ");
	}
	else if(CycleName==2){
			DisplayPage8Char(0x11,0x01,"STANDARD");
	}
	else if(CycleName==3){
			DisplayPage8Char(0x11,0x01,"ADVANCED");
	}
	DisplayTime(0x11,0x10,CycleTime);
	if(CurrentProcess==1){
		DisplayPage8Char(0x11,0x30,"VACCUM1 ");
	}
	else if(CurrentProcess==2){
		DisplayPage8Char(0x11,0x30,"INJECET1");
	}
	else if(CurrentProcess==3){
		DisplayPage8Char(0x11,0x30,"VACCUM2 ");
	}
	else if(CurrentProcess==4){
		DisplayPage8Char(0x11,0x30,"INJECET2");
	}
	else if(CurrentProcess==5){
		DisplayPage8Char(0x11,0x30,"VACCUM3 ");
	}
	else if(CurrentProcess==6){
		DisplayPage8Char(0x11,0x30,"VENT    ");
	}
	else{
		DisplayPage8Char(0x11,0x30,"        ");
	}


	if(Running_Flag){
		DisplayTime(0x11,0x40,ProcessTime[CurrentProcess]);
		DisplayTime(0x11,0x10,CycleTime);
		DisplayPageValue(0x11,0x45,Pressure*10);
		DisplayPageValue(0x11,0x49,Temperature[1]*10);
	}
	else{
		DisplayTime(0x11,0x40,0);
		DisplayTime(0x11,0x10,TotalTime);
		DisplayPageValue(0x11,0x45,Pressure*10);
		DisplayPageValue(0x11,0x49,Temperature[1]*10);
	}
}







//Time display
unsigned char   time_display[9] = {0x5A, 0xA5, 0x06, 0x82, 0x02, 0x00, 0x00, 0x00, 0x00};

void DisplayTime(int page, int index, unsigned int icentisecond){
	time_display[4] = page;
    time_display[5] = index;
    unsigned int iMinute = icentisecond / 6000;
    unsigned int iHour = iMinute / 60;
    iMinute = iMinute - (iHour * 60);
    time_display[6] = hex2bcd(iHour);   // Hour
    time_display[7] = hex2bcd(iMinute); // Minute
    time_display[8] = hex2bcd((icentisecond / 100) % 60);   // Second
    HAL_UART_Transmit(LCD_USART, time_display, 9, 10);
}


#define first_steptime_index 0x54
#define first_processtime_index 0x72
unsigned char   steptime_index[11] = {0x00, first_steptime_index, first_steptime_index+0x03,first_steptime_index+0x06, first_steptime_index+0x09, first_steptime_index+0x0c,
								first_steptime_index+0x0f,first_steptime_index+0x12, first_steptime_index+0x15, first_steptime_index+0x18, first_steptime_index+0x1b};
unsigned char   processtime_index[7] = {0x00, first_processtime_index, first_processtime_index+0x03,first_processtime_index+0x06, first_processtime_index+0x09, first_processtime_index+0x0c,
								first_processtime_index+0x0f};
void DisplayTimeValues(){
	if(current_page==LCD_FACTORY_TESTMODE_PROCESSSETTINGS1_PAGE){
		DisplayTime(0x34,0x51,CycleData[ProcessNum][StepNum].Time*100);
		for(int i=1;i<11;i++){
			DisplayTime(0x34,steptime_index[i],CycleData[ProcessNum][i].Time*100);
		}
		CycleTime=0;
		for(int i=1;i<7;i++){
			float ptime=0;
			for(int j=1;j<21;j++){
				ptime+=CycleData[i][j].Time;
			}
			DisplayTime(0x34,processtime_index[i],ptime*100);
			CycleTime+=ptime*100;
		}
		DisplayTime(0x34,0x89,CycleTime);
	}
	else if(current_page==LCD_FACTORY_TESTMODE_PROCESSSETTINGS2_PAGE){
		DisplayTime(0x34,0x51,CycleData[ProcessNum][StepNum].Time*100);
		for(int i=1;i<11;i++){
			DisplayTime(0x34,steptime_index[i],CycleData[ProcessNum][i+10].Time*100);
		}
		CycleTime=0;
		for(int i=1;i<7;i++){
			float ptime=0;
			for(int j=1;j<21;j++){
				ptime+=CycleData[i][j].Time;
			}
			DisplayTime(0x34,processtime_index[i],ptime*100);
			CycleTime+=ptime*100;
		}
		DisplayTime(0x34,0x89,CycleTime);
	}
	else if(current_page==LCD_FACTORY_TESTMODE_PROCESSTEST1_PAGE){
		for(int i=1;i<11;i++){	//step time display
			DisplayTime(0x34,steptime_index[i],StepTime[i]);
		}
		for(int i=1;i<7;i++){	//process time display
			DisplayTime(0x34,processtime_index[i],ProcessTime[i]);
		}
		DisplayTime(0x34,0x89,CycleTime);
	}
	else if(current_page==LCD_FACTORY_TESTMODE_PROCESSTEST2_PAGE){
		for(int i=1;i<11;i++){	//step time display
			DisplayTime(0x34,steptime_index[i],StepTime[i+10]);
		}
		for(int i=1;i<7;i++){	//process time display
			DisplayTime(0x34,processtime_index[i],ProcessTime[i]);
		}
		DisplayTime(0x34,0x89,CycleTime);
	}
	else if(current_page==LCD_FACTORY_TESTMODE_PROCESSTEST3_PAGE){
		for(int i=1;i<11;i++){	//step time display
			DisplayTime(0x34,steptime_index[i],StepTime[i]);
		}
		for(int i=1;i<7;i++){	//process time display
			DisplayTime(0x34,processtime_index[i],ProcessTime[i]);
		}
		DisplayTime(0x34,0x89,CycleTime);
	}
	else if(current_page==LCD_FACTORY_TESTMODE_PROCESSTEST4_PAGE){
		for(int i=1;i<11;i++){	//step time display
			DisplayTime(0x34,steptime_index[i],StepTime[i+10]);
		}
		for(int i=1;i<7;i++){	//process time display
			DisplayTime(0x34,processtime_index[i],ProcessTime[i]);
		}
		DisplayTime(0x34,0x89,CycleTime);
	}
}



void ReadStepTime(){
	for(int i=1;i<21;i++){
		StepTime[i]=CycleData[CurrentProcess][i].Time*100;
	}
}

void ReadProcessTime(){
	for(int i=1;i<7;i++){
		ProcessTime[i]=0;
		for(int j=1;j<21;j++){
			ProcessTime[i]+=(CycleData[i][j].Time*100);
		}
	}
}

void ReadCycleTime(){
	CycleTime=0;
	for(int i=1;i<7;i++){
		CycleTime+=ProcessTime[i];
	}

}





//Value display
unsigned char   value_display[10] = {0x5A, 0xA5, 0x05, 0x82, 0x02, 0x00, 0x00, 0x00, 0x00};
unsigned char   Temp_display[10] = {0x5A, 0xA5, 0x05, 0x82, 0x01, 0x00, 0x00, 0x00, 0x00};

void DisplayValue(int index, float value){
    unsigned int uivalue;
    uivalue = (value * 10);
    value_display[2] = 0x05;
    value_display[5] = index;
    value_display[6] = uivalue >> 8;
    value_display[7] = uivalue & 0xff;
    HAL_UART_Transmit(LCD_USART, value_display, 8, 10);
}
void DisplayCycleValue(int value){
	if(value==1){
		DisplayPage8Char(0x34,0x01,"SHORT   ");
	}
	else if(value==2){
			DisplayPage8Char(0x34,0x01,"STANDARD");
	}
	else if(value==3){
			DisplayPage8Char(0x34,0x01,"ADVANCED");
	}
}

void DisplayTemp(int index, float value){
    unsigned int uivalue;
    uivalue = (value * 10);
    Temp_display[2] = 0x05;
    Temp_display[5] = index;
    Temp_display[6] = uivalue >> 8;
    Temp_display[7] = uivalue & 0xff;
    HAL_UART_Transmit(LCD_USART, Temp_display, 8, 10);
}

void DisplayTempValues(){
	DisplayPageValue(0x33,0x01,DoorSettingTemp[0]*10);
	DisplayPageValue(0x33,0x05,DoorSettingTemp[1]*10);
	DisplayPageValue(0x33,0x09,DoorSettingTemp[2]*10);

	DisplayPageValue(0x33,0x11,ChamberSettingTemp[0]*10);
	DisplayPageValue(0x33,0x15,ChamberSettingTemp[1]*10);
	DisplayPageValue(0x33,0x19,ChamberSettingTemp[2]*10);

	DisplayPageValue(0x33,0x21,ChamberBackSettingTemp[0]*10);
	DisplayPageValue(0x33,0x25,ChamberBackSettingTemp[1]*10);
	DisplayPageValue(0x33,0x29,ChamberBackSettingTemp[2]*10);

	DisplayPageValue(0x33,0x31,VaporizerSettingTemp[0]*10);
	DisplayPageValue(0x33,0x35,VaporizerSettingTemp[1]*10);
	DisplayPageValue(0x33,0x39,VaporizerSettingTemp[2]*10);

	DisplayPageValue(0x33,0x41,PreesureCondition[0]*10);
	DisplayPageValue(0x33,0x45,PreesureCondition[1]*10);
	DisplayPageValue(0x33,0x49,PreesureCondition[2]*10);

	DisplayPageValue(0x33,0x51,PlasmaTime[0]);
	DisplayPageValue(0x33,0x55,PlasmaTime[1]);

	for(int i=0;i<3;i++){
		FlashSettingTemp[0][i]=DoorSettingTemp[i];
		FlashSettingTemp[1][i]=ChamberSettingTemp[i];
		FlashSettingTemp[2][i]=ChamberBackSettingTemp[i];
		FlashSettingTemp[3][i]=VaporizerSettingTemp[i];
	}
	FlashPreesureCondition[0]=PreesureCondition[0];
	FlashPreesureCondition[1]=PreesureCondition[1];
	FlashPreesureCondition[2]=PreesureCondition[2];

	FlashPlasmaTime[0]=PlasmaTime[0];
	FlashPlasmaTime[1]=PlasmaTime[1];
}


void DisplayVacuumSensor(){
	DisplayPageValue(0x31,0x31,Pressure*10);
}

void DisplayRFIDNumber(){	// RFID Display
	unsigned char   H2O2RFID_log_display[9] = {0x5A, 0xA5, 0x06, 0x82, 0x02, 0x01, 0x00, 0x00, 0x00};

	H2O2RFID_log_display[6]='0';
	H2O2RFID_log_display[7]=(RFIDData.production_number/10)+'0';
	H2O2RFID_log_display[8]=(RFIDData.production_number%10)+'0';
	HAL_UART_Transmit(LCD_USART, H2O2RFID_log_display, 9, 10);
}

void Display02page(){
	DisplayPage10Char(0x02,0x10,"");
	DisplayPage10Char(0x02,0x20,"");
}



//3 페이지
void Display03page(){
	DisplayPage10Char(0x03,0x10,"");
	DisplayPage10Char(0x03,0x20,"");
	DisplayPage10Char(0x03,0x30,"");
}

void Display04page(){
	DisplayPage10Char(0x04,0x10,flash_ID[0]);
	DisplayPage10Char(0x04,0x20,flash_ID[1]);
	DisplayPage10Char(0x04,0x30,flash_ID[2]);
	DisplayPage10Char(0x04,0x40,flash_ID[3]);
	DisplayPage10Char(0x04,0x50,flash_ID[4]);
	DisplayPage10Char(0x04,0x60,flash_ID[Select_ID]);
}


//페이지 별 데이터 입출력

void Display05page(){
	//도어 상태 표기
	//아이디 표기
	if(CurrentUser==10){
		DisplayPage10Char(0x05,0x10,"CBT          ");
	}
	else{
		DisplayPage10Char(0x05,0x10,flash_ID[CurrentUser]);
	}
}

void Display06page(){
	 DisplayCycleValue(CycleName);
}
void Display07page(){

}
void Display08page(){

}
void Display09page(){

}
void Display10page(){

}
void Display11page(){
	DisplayPageValue(0x11,0x51,totalCount);
	DisplayPageValue(0x11,0x55,dailyCount);
	DisplayPageValue(0x11,0x20,flash_sterilant_volume[RFIDData.currentID]);
}
void Display12page(){

}

void Display14page(){

}

void Display21page(){
	DisplayPage10Char(0x30,0x30,flash_MODEL_NAME);
	DisplayPage10Char(0x30,0x40,flash_SERIAL_NUMBER);
	DisplayPage10Char(0x30,0x50,flash_DEPARTMENT_NAME);
	DisplayPage10Char(0x30,0x60,flash_FACILITY_NAME);
	DisplayPage10Char(0x30,0x70,flash_HARDWARE_VERSION);
	DisplayPage10Char(0x30,0x80,flash_SOFTWARE_VERSION);
	DisplayPage10Char(0x30,0x90,flash_LANGUAGE);

	DisplayPageValue(0x21,0x10,totalCount);
	DisplayPageValue(0x21,0x20,dailyCount);

	DisplayPageValue(0x28,0x40,CarbonFilter);
	DisplayPageValue(0x28,0x50,HEPAFilter);
	DisplayPageValue(0x28,0x60,PlasmaAssy);
}

void Display22page(){
	if(flash_sterilant_open_year[RFIDData.currentID]==0){
		DisplayPageValue(0x22,0x10,0);
		//RFIDData.open_year,RFIDData.open_month,RFIDData.open_day;
		DisplayPage10Char(0x22,0x20,"");
		DisplayPageValue(0x22,0x30,0);
		DisplayPage10Char(0x22,0x40,"");
		DisplayPage10Char(0x22,0x50,"00/50");
	}
	else{
		DisplayPageValue(0x22,0x10,flash_sterilant_volume[RFIDData.currentID]);
		char msg[10];
		sprintf(msg,"20%2d-%02d-%02d",
				(int)flash_sterilant_open_year[RFIDData.currentID],(int)flash_sterilant_open_month[RFIDData.currentID],(int)flash_sterilant_open_day[RFIDData.currentID]);
		DisplayPage10Char(0x22,0x20,msg);
		GetTime();
		int elapsed_days=calculateElapsedDays(bcd2bin(today_date.year),bcd2bin(today_date.month),bcd2bin(today_date.day),(int)flash_sterilant_open_year[RFIDData.currentID],(int)flash_sterilant_open_month[RFIDData.currentID],(int)flash_sterilant_open_day[RFIDData.currentID]);
		RFIDData.elapsed_days=elapsed_days;

		DisplayPageValue(0x22,0x30,30-elapsed_days);

		//시리얼넘버
		sprintf(msg,"%02d%02d%02d%02d  ",
					(int)flash_sterilant_production_year[RFIDData.currentID],(int)flash_sterilant_production_month[RFIDData.currentID],(int)flash_sterilant_production_day[RFIDData.currentID], (int)flash_sterilant_production_number[RFIDData.currentID]);
		DisplayPage10Char(0x22,0x40,msg);
		sprintf(msg,"%02d/50",flash_sterilant_volume[RFIDData.currentID]);
		DisplayPage10Char(0x22,0x50,msg);
	}
}

void Display24page(){
	//프린트 토글
	DisplayIcon(0x24,0x01,autoprint);
	DisplayIcon(0x24,0x02,1);
	DisplayIcon(0x24,0x03,1);
}

void Display23page(){

/*
	extern unsigned char loadflash_ID[10];
	extern unsigned char flash_load_index;
	extern struct data_format load_data;
	extern float loadPressureData[300];
	extern float loadTemperatureData[300];

	int cyclename;
	unsigned char year, month, day;
	unsigned char start_hour, start_minute, start_second;
	unsigned char end_hour, end_minute, end_second;
	float	tempmax[7],pressuremax[7],pressuremin[7];
	int status;
*/

	if(load_data.cyclename==1){
		DisplayPage10Char(0x23,0x10,"  SHORT   ");
	}
	else if(load_data.cyclename==2){
		DisplayPage10Char(0x23,0x10," STANDARD ");
	}
	else if(load_data.cyclename==3){
		DisplayPage10Char(0x23,0x10," ADVANCED ");
	}
	else{
		DisplayPage10Char(0x23,0x10,"  NODATA  ");
	}

	if(load_data.status==11){
		DisplayPage10Char(0x23,0x20," COMPELTE ");
	}
	else if(load_data.status==1){
		DisplayPage10Char(0x23,0x20," ERROR01  ");
	}
	else if(load_data.status==2){
		DisplayPage10Char(0x23,0x20," ERROR02  ");
	}
	else if(load_data.status==3){
		DisplayPage10Char(0x23,0x20," ERROR03  ");
	}
	else if(load_data.status==4){
		DisplayPage10Char(0x23,0x20," ERROR04  ");
	}
	else if(load_data.status==5){
		DisplayPage10Char(0x23,0x20," ERROR05  ");
	}
	else if(load_data.status==6){
		DisplayPage10Char(0x23,0x20," ERROR06  ");
	}
	else if(load_data.status==7){
		DisplayPage10Char(0x23,0x20," ERROR07  ");
	}
	else if(load_data.status==8){
		DisplayPage10Char(0x23,0x20," ERROR08  ");
	}
	else if(load_data.status==9){
		DisplayPage10Char(0x23,0x20," ERROR09  ");
	}
	else if(load_data.status==10){
		DisplayPage10Char(0x23,0x20," ERROR10  ");
	}
	else{
		DisplayPage10Char(0x23,0x20,"  NODATA  ");
	}

	char msg[10];
	if(load_data.start_hour==0){
		DisplayPage10Char(0x23,0x30,"  NODATA  ");
	}
	else{
		sprintf(msg," %02d:%02d:%02d ",
				bcd2bin(load_data.start_hour),bcd2bin(load_data.start_minute),bcd2bin(load_data.start_second));
		DisplayPage10Char(0x23,0x30,msg);
	}

	if(load_data.end_hour==0){
		DisplayPage10Char(0x23,0x40,"  NODATA  ");
	}
	else{
		sprintf(msg," %02d:%02d:%02d ",
				bcd2bin(load_data.end_hour),bcd2bin(load_data.end_minute),bcd2bin(load_data.end_second));
		DisplayPage10Char(0x23,0x40,msg);
	}


	DisplayPageValue(0x23,0x50,load_data.tempmax[1]*10);
	DisplayPageValue(0x23,0x55,load_data.pressuremin[1]*10);

	DisplayPageValue(0x23,0x60,load_data.tempmax[2]*10);
	DisplayPageValue(0x23,0x65,load_data.pressuremin[2]*10);

	DisplayPageValue(0x23,0x70,load_data.tempmax[3]*10);
	DisplayPageValue(0x23,0x75,load_data.pressuremin[3]*10);

	DisplayPageValue(0x23,0x80,load_data.tempmax[4]*10);
	DisplayPageValue(0x23,0x85,load_data.pressuremin[4]*10);

	DisplayPageValue(0x23,0x90,load_data.tempmax[5]*10);
	DisplayPageValue(0x23,0x95,load_data.pressuremin[5]*10);

	DisplayPageValue(0x23,0xA0,load_data.tempmax[6]*10);
	DisplayPageValue(0x23,0xA5,load_data.pressuremin[6]*10);

	char msg1[18]={};
	if(temptotalcycle[1]==0){
		DisplayPage10Char(0x21,0x60,"");
	}
	else{
		sprintf(msg1,"Cycle #%03d",temptotalcycle[1]);
		DisplayPage10Char(0x21,0x60,msg1);
	}
	if(temptotalcycle[2]==0){
		DisplayPage10Char(0x21,0x70,"");
	}
	else{
		sprintf(msg1,"Cycle #%03d",temptotalcycle[2]);
		DisplayPage10Char(0x21,0x70,msg1);
	}
	if(temptotalcycle[3]==0){
		DisplayPage10Char(0x21,0x80,"");
	}
	else{
		sprintf(msg1,"Cycle #%03d",temptotalcycle[3]);
		DisplayPage10Char(0x21,0x80,msg1);
	}
	if(temptotalcycle[4]==0){
		DisplayPage10Char(0x21,0x90,"");
	}
	else{
		sprintf(msg1,"Cycle #%03d",temptotalcycle[4]);
		DisplayPage10Char(0x21,0x90,msg1);
	}
	if(temptotalcycle[5]==0){
		DisplayPage10Char(0x21,0xA0,"");
	}
	else{
		sprintf(msg1,"Cycle #%03d",temptotalcycle[5]);
		DisplayPage10Char(0x21,0xA0,msg1);
	}

}


void Display25page(){
	if(TestCompleteflag==0){
		DisplayPageValue(0x25,0x01,TestVacuumValue*10);
		DisplayPageValue(0x25,0x05,TestLeakValue*10);

		DisplayPageValue(0x25,0x10,0);
		DisplayPageValue(0x25,0x14,0);
		DisplayPageValue(0x25,0x18,0);
		DisplayPageValue(0x25,0x1C,0);
		DisplayPageValue(0x25,0x20,0);

		DisplayPageValue(0x25,0x24,0);
		DisplayPageValue(0x25,0x28,0);
		DisplayPageValue(0x25,0x2C,0);
		DisplayPageValue(0x25,0x30,0);
		DisplayPageValue(0x25,0x34,0);

		DisplayPage4Char(0x25,0x40,"");
		DisplayPage4Char(0x25,0x44,"");
	}
	DisplayPage(LCD_USER_LEAKTEST_PAGE);

	//DisplayTempValues();
	//출력값
	/*
	테스트 시간
	에러레이트
	공정 세팅 온도 값
	현재 온도 값
	결과 각각 PASS or FAIL
	종합 PASS or FAIL
	 */
}


void Display26page(){
	if(TestCompleteflag==0){
		DisplayPage4Char(0x26,0x10,"");
		DisplayPage4Char(0x26,0x14,"");
		DisplayPage4Char(0x26,0x18,"");
		DisplayPage4Char(0x26,0x1C,"");
		DisplayPage4Char(0x26,0x20,"");
	}
	DisplayTempValues();

	DisplayPage(LCD_USER_HEATINGTEST_PAGE);

	//출력값
	/*
	테스트 시간
	에러레이트
	공정 세팅 온도 값
	현재 온도 값
	결과 각각 PASS or FAIL
	종합 PASS or FAIL


	 */

}

void Display27page(){
	if(TestCompleteflag==0){
		DisplayPage4Char(0x27,0x20,"");
		DisplayPage4Char(0x27,0x24,"");
		DisplayPage4Char(0x27,0x28,"");
		DisplayPage4Char(0x27,0x2C,"");
	}
	DisplayPage(LCD_USER_PARTTEST_PAGE);

	//출력값
	/*
	INJECTION VALVE RESULT
	VENT VALVE RESULT
	VACCUM VALVE RESULT

	RESULT
	 */
}

void Display28page(){
	tempCarbonFilterMax=CarbonFilterMax;
	tempHEPAFilterMax=HEPAFilterMax;
	tempPlasmaAssyMax=PlasmaAssyMax;
	tempCarbonFilter=CarbonFilter;
	tempHEPAFilter=HEPAFilter;
	tempPlasmaAssy=PlasmaAssy;
	DisplayPageValue(0x28,0x10,CarbonFilterMax);
	DisplayPageValue(0x28,0x20,HEPAFilterMax);
	DisplayPageValue(0x28,0x30,PlasmaAssyMax);

	DisplayPageValue(0x28,0x40,CarbonFilter);
	DisplayPageValue(0x28,0x50,HEPAFilter);
	DisplayPageValue(0x28,0x60,PlasmaAssy);

}


//30 페이지
void Display30page(){
	DisplayPageValue(0x30,0x01,TestVacuumValue*10);
	DisplayPageValue(0x30,0x05,TestLeakValue*10);
	DisplayPageValue(0x30,0x09,TestTempErrorValue);

	DisplayPage10Char(0x30,0x30,flash_MODEL_NAME);
	DisplayPage10Char(0x30,0x40,flash_SERIAL_NUMBER);
	DisplayPage10Char(0x30,0x50,flash_DEPARTMENT_NAME);
	DisplayPage10Char(0x30,0x60,flash_FACILITY_NAME);
	DisplayPage10Char(0x30,0x70,flash_HARDWARE_VERSION);
	DisplayPage10Char(0x30,0x80,flash_SOFTWARE_VERSION);
	DisplayPage10Char(0x30,0x90,flash_LANGUAGE);
}

//31 페이지
void Display31page(){
	DisplayPage(LCD_FACTORY_CALIBRATION_PAGE);
	//출력값
	/*
	현재 온도 값
	스팬 값
	제로 값
	 */
	DisplayPageValue(0x31,0x01,Temperature[0]*10);
	DisplayPageValue(0x31,0x05,Temperature[1]*10);
	DisplayPageValue(0x31,0x09,Temperature[2]*10);
	DisplayPageValue(0x31,0x0D,Temperature[3]*10);



	DisplayPageValue(0x31,0x21,CalibrationTemp[0]*10);
	DisplayPageValue(0x31,0x25,CalibrationTemp[1]*10);
	DisplayPageValue(0x31,0x29,CalibrationTemp[2]*10);
	DisplayPageValue(0x31,0x2D,CalibrationTemp[3]*10);
	DisplayPageValue(0x31,0x35,CalibrationVacuum*10);


	DisplayVacuumSensor();

}
void Display33page(){
	DisplayPage(LCD_FACTORY_CONTROLSETTING_PAGE);
	DisplayPageValue(0x33,0x01,DoorSettingTemp[0]*10);
	DisplayPageValue(0x33,0x05,DoorSettingTemp[1]*10);
	DisplayPageValue(0x33,0x09,DoorSettingTemp[2]*10);

	DisplayPageValue(0x33,0x11,ChamberSettingTemp[0]*10);
	DisplayPageValue(0x33,0x15,ChamberSettingTemp[1]*10);
	DisplayPageValue(0x33,0x19,ChamberSettingTemp[2]*10);

	DisplayPageValue(0x33,0x21,ChamberBackSettingTemp[0]*10);
	DisplayPageValue(0x33,0x25,ChamberBackSettingTemp[1]*10);
	DisplayPageValue(0x33,0x29,ChamberBackSettingTemp[2]*10);

	DisplayPageValue(0x33,0x31,VaporizerSettingTemp[0]*10);
	DisplayPageValue(0x33,0x35,VaporizerSettingTemp[1]*10);
	DisplayPageValue(0x33,0x39,VaporizerSettingTemp[2]*10);

	DisplayPageValue(0x33,0x41,PreesureCondition[0]*10);
	DisplayPageValue(0x33,0x45,PreesureCondition[1]*10);
	DisplayPageValue(0x33,0x49,PreesureCondition[2]*10);

	DisplayPageValue(0x33,0x51,PlasmaTime[0]);
	DisplayPageValue(0x33,0x55,PlasmaTime[1]);

	for(int i=0;i<3;i++){
		FlashSettingTemp[0][i]=DoorSettingTemp[i];
		FlashSettingTemp[1][i]=ChamberSettingTemp[i];
		FlashSettingTemp[2][i]=ChamberBackSettingTemp[i];
		FlashSettingTemp[3][i]=VaporizerSettingTemp[i];
	}
	FlashPreesureCondition[0]=PreesureCondition[0];
	FlashPreesureCondition[1]=PreesureCondition[1];
	FlashPreesureCondition[2]=PreesureCondition[2];

	FlashPlasmaTime[0]=PlasmaTime[0];
	FlashPlasmaTime[1]=PlasmaTime[1];
}

void Display34page(){
	DisplayPageValue(0x33,0x41,PreesureCondition[0]*10);
	DisplayPageValue(0x33,0x45,PreesureCondition[1]*10);
	DisplayPageValue(0x33,0x49,PreesureCondition[2]*10);
	DisplayCycleValue(CycleName);
	DisplayProcessIcons(ProcessNum);
	DisplayStepIcons(StepNum);
	DisplayPartsIcons();
	DisplayTimeValues();
	//DisplayPage(LCD_FACTORY_TESTMODE_PROCESSSETTINGS1_PAGE);
}
void Display35page(){
	DisplayCycleValue(CycleName);
	DisplayProcessIcons(ProcessNum);
	DisplayStepIcons(StepNum);
	DisplayPartsIcons();
	DisplayTimeValues();
	//DisplayPage(LCD_FACTORY_TESTMODE_PROCESSSETTINGS1_PAGE);
}
void Display36page(){
	DisplayCycleValue(CycleName);
	DisplayProcessIcons(ProcessNum);
	DisplayStepIcons(StepNum);
	DisplayPartsIcons();
	DisplayTimeValues();
	//DisplayPage(LCD_FACTORY_TESTMODE_PROCESSSETTINGS1_PAGE);
}
void Display37page(){
	DisplayCycleValue(CycleName);
	DisplayProcessIcons(ProcessNum);
	DisplayStepIcons(StepNum);
	DisplayPartsIcons();
	DisplayTimeValues();
	//DisplayPage(LCD_FACTORY_TESTMODE_PROCESSSETTINGS1_PAGE);
}
void Display38page(){
	DisplayCycleValue(CycleName);
	DisplayProcessIcons(ProcessNum);
	DisplayStepIcons(StepNum);
	DisplayPartsIcons();
	DisplayTimeValues();
	//DisplayPage(LCD_FACTORY_TESTMODE_PROCESSSETTINGS1_PAGE);
}
void Display39page(){
	DisplayCycleValue(CycleName);
	DisplayProcessIcons(ProcessNum);
	DisplayStepIcons(StepNum);
	DisplayPartsIcons();
	DisplayTimeValues();
	//DisplayPage(LCD_FACTORY_TESTMODE_PROCESSSETTINGS1_PAGE);
}

void Display44page(){
	//DisplayPage(LCD_FACTORY_TESTMODE_SELECT_CYCLE_PAGE);
}


void DisplayPageValue(int page ,int index, int value){
	unsigned char   PageValue[8] = {0x5A, 0xA5, 0x05, 0x82, 0x00, 0x00, 0x00, 0x00};
	PageValue[4] = page;
	PageValue[5] = index;
	PageValue[6] = value >> 8;
	PageValue[7] = value & 0xff;
    HAL_UART_Transmit(LCD_USART, PageValue, 8, 10);
}



//문자 출력
void DisplayPage4Char(int page ,int index, char *msg){
	unsigned char   PageChar[10] = {0x5A, 0xA5, 0x05, 0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	PageChar[2]=7; //주소1+주소2+Data	lenth
	PageChar[4]=page;
	PageChar[5]=index;
	PageChar[6]=msg[0];
	PageChar[7]=msg[1];
	PageChar[8]=msg[2];
	PageChar[9]=msg[3];
    HAL_UART_Transmit(LCD_USART, PageChar, 10, 10);
}

void DisplayPage8Char(int page ,int index, char *msg){
	unsigned char   PageChar[14] = {0x5A, 0xA5, 0x0b, 0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	PageChar[2]=11; //주소1+주소2+Data	lenth
	PageChar[4]=page;
	PageChar[5]=index;
	PageChar[6]=msg[0];
	PageChar[7]=msg[1];
	PageChar[8]=msg[2];
	PageChar[9]=msg[3];
	PageChar[10]=msg[4];
	PageChar[11]=msg[5];
	PageChar[12]=msg[6];
	PageChar[13]=msg[7];
    HAL_UART_Transmit(LCD_USART, PageChar, 14, 10);
}

void DisplayPage10Char(int page ,int index, unsigned char *msg){
	unsigned char   PageChar[16] = {0x5A, 0xA5, 0x0d, 0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	PageChar[2]=13; //주소1+주소2+Data	lenth
	PageChar[4]=page;
	PageChar[5]=index;
	PageChar[6]=msg[0];
	PageChar[7]=msg[1];
	PageChar[8]=msg[2];
	PageChar[9]=msg[3];
	PageChar[10]=msg[4];
	PageChar[11]=msg[5];
	PageChar[12]=msg[6];
	PageChar[13]=msg[7];
	PageChar[14]=msg[8];
	PageChar[15]=msg[9];
    HAL_UART_Transmit(LCD_USART, PageChar, 16, 10);
}

void DisplayPage20Char(int page ,int index, unsigned char *msg){
	unsigned char   PageChar[16] = {0x5A, 0xA5, 0x0d, 0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	PageChar[2]=23; //주소1+주소2+Data	lenth
	PageChar[4]=page;
	PageChar[5]=index;
	PageChar[6]=msg[0];
	PageChar[7]=msg[1];
	PageChar[8]=msg[2];
	PageChar[9]=msg[3];
	PageChar[10]=msg[4];
	PageChar[11]=msg[5];
	PageChar[12]=msg[6];
	PageChar[13]=msg[7];
	PageChar[14]=msg[8];
	PageChar[15]=msg[9];
	PageChar[16]=msg[10];
	PageChar[17]=msg[11];
	PageChar[18]=msg[12];
	PageChar[19]=msg[13];
	PageChar[20]=msg[14];
	PageChar[21]=msg[15];
	PageChar[22]=msg[16];
	PageChar[23]=msg[17];
	PageChar[24]=msg[18];
	PageChar[25]=msg[19];
    HAL_UART_Transmit(LCD_USART, PageChar, 26, 10);
}

void DisplayMsg(int page ,int index, char *msg){
	unsigned char   error_display[100] = {0x5a, 0xa5, 0x53, 0x82, 0x01, 0x00, 0x4e, 0x75, 0x72, 0x69, 0x20, 0x53, 0x79, 0x73, 0x74, 0x65, 0x6d};
	error_display[4]=page;
	error_display[5]=index;
	__disable_irq();
    memset(error_display + 6, 0, 80);
    for(int i = 0; i < 80; i++) {
        error_display[6 + i] = msg[i];
    }
    HAL_UART_Transmit(LCD_USART, error_display, 86, 10);
    memset(error_display + 6, 0, 80);
    __enable_irq();
}

void GetTime(void){
	unsigned char week;
	ReadRTC( &today_date.year, &today_date.month, &today_date.day, &week,
			&today_date.hour, &today_date.minute, &today_date.second);
}

void DisplayTemprature(){
	DisplayPageValue(0x60,0x01,Temperature[0]*10);
	DisplayPageValue(0x60,0x05,Temperature[1]*10);
	DisplayPageValue(0x60,0x09,Temperature[2]*10);
	DisplayPageValue(0x60,0x0D,Temperature[3]*10);

}

void ReadRTC(unsigned char *year, unsigned char *month, unsigned char *day, unsigned char *week, unsigned char *hour, unsigned char *minute, unsigned char *second){

    __disable_irq();
    huart1.RxState= HAL_UART_STATE_READY;
    memset(LCD_rx_data, 0, 30);
    HAL_UART_Transmit(LCD_USART, (uint8_t*)rtc_date_get, 6, 10);
    HAL_UART_Receive(LCD_USART, (uint8_t*)LCD_rx_data, 9, 10);
    *year = LCD_rx_data[6];
    *month = LCD_rx_data[7];
    *day = LCD_rx_data[8];
    *week = LCD_rx_data[9];


    memset(LCD_rx_data, 0, 30);
    HAL_UART_Transmit(LCD_USART, (uint8_t*)rtc_time_get, 6, 10);
    HAL_UART_Receive(LCD_USART, (uint8_t*)LCD_rx_data, 9, 10);
    *hour = LCD_rx_data[6];
    *minute = LCD_rx_data[7];
    *second = LCD_rx_data[8];

    UART_Receive_Flag = 0;
    __enable_irq();
    HAL_UART_Receive_IT(LCD_USART, (uint8_t*)LCD_rx_data, 9);
}

void SetRTCFromLCD(){
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
    ReadRTC(&sDate.Year, &sDate.Month, &sDate.Date, &sDate.WeekDay, &sTime.Hours, &sTime.Minutes, &sTime.Seconds);

    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;
    if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
    {
        Error_Handler();
    }
}

void ReadInforDataFromLCD(){
    __disable_irq();
    huart1.RxState= HAL_UART_STATE_READY;
    unsigned char get_lcd_data[7] = {0x5A, 0xA5, 0x04, 0x83, 0x30, 0x30, 0x05};
    memset(LCD_rx_data, 0, 30);
    get_lcd_data[5]=0x30;//Model_Name
    get_lcd_data[6]=0x05;//Size
    HAL_UART_Transmit(LCD_USART, (uint8_t*)get_lcd_data, 7, 10);
    HAL_UART_Receive(LCD_USART, (uint8_t*)LCD_rx_data, 17, 10);

    for(int i=0;i<10;i++){
    	flash_MODEL_NAME[i]=LCD_rx_data[i+7];
    }

	get_lcd_data[5]=0x40;//SERIAL NUMBER
    HAL_UART_Transmit(LCD_USART, (uint8_t*)get_lcd_data, 7, 10);
    HAL_UART_Receive(LCD_USART, (uint8_t*)LCD_rx_data, 17, 10);

	for(int i=0;i<10;i++){
		flash_SERIAL_NUMBER[i]=LCD_rx_data[i+7];
	}

    get_lcd_data[5]=0x50;//DEPARTMENT
	HAL_UART_Transmit(LCD_USART, (uint8_t*)get_lcd_data, 7, 10);
	HAL_UART_Receive(LCD_USART, (uint8_t*)LCD_rx_data, 17, 10);

	for(int i=0;i<10;i++){
		flash_DEPARTMENT_NAME[i]=LCD_rx_data[i+7];
	}

    get_lcd_data[5]=0x60;//FACILITY
	HAL_UART_Transmit(LCD_USART, (uint8_t*)get_lcd_data, 7, 10);
	HAL_UART_Receive(LCD_USART, (uint8_t*)LCD_rx_data, 17, 10);

	for(int i=0;i<10;i++){
		flash_FACILITY_NAME[i]=LCD_rx_data[i+7];
	}

    get_lcd_data[5]=0x70;//Model_Name
    HAL_UART_Transmit(LCD_USART, (uint8_t*)get_lcd_data, 7, 10);
    HAL_UART_Receive(LCD_USART, (uint8_t*)LCD_rx_data, 17, 10);

    for(int i=0;i<10;i++){
    	flash_HARDWARE_VERSION[i]=LCD_rx_data[i+7];
    }

    get_lcd_data[5]=0x80;//Model_Name
    HAL_UART_Transmit(LCD_USART, (uint8_t*)get_lcd_data, 7, 10);
    HAL_UART_Receive(LCD_USART, (uint8_t*)LCD_rx_data, 17, 10);

    for(int i=0;i<10;i++){
    	flash_SOFTWARE_VERSION[i]=LCD_rx_data[i+7];
    }

    get_lcd_data[5]=0x90;//Model_Name
    HAL_UART_Transmit(LCD_USART, (uint8_t*)get_lcd_data, 7, 10);
    HAL_UART_Receive(LCD_USART, (uint8_t*)LCD_rx_data, 17, 10);

    for(int i=0;i<10;i++){
    	flash_LANGUAGE[i]=LCD_rx_data[i+7];
    }

    UART_Receive_Flag = 0;
    __enable_irq();
    HAL_UART_Receive_IT(LCD_USART, (uint8_t*)LCD_rx_data, 9);
}


unsigned char hex2bcd (unsigned char x)
{
    unsigned char y;
    y = (x / 10) << 4;
    y = y | (x % 10);
    return (y);
}

unsigned char bcd2bin (unsigned char x)
{
    return 	(x&0x0f)+(x>>4)*10;
}
