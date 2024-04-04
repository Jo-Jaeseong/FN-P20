/*
 * lcd.h
 *
 *  Created on: Oct 25, 2022
 *      Author: CBT
 */

#ifndef SRC_PERIPHERAL_LCD_H_
#define SRC_PERIPHERAL_LCD_H_

#define LCD_FIRST_PAGE									1
#define LCD_LOGIN_PAGE	 								2
#define LCD_CREATE_ID	 								3
#define LCD_MANAGEMENT_ID	 							4
#define LCD_HOME_PAGE									5
#define LCD_STANDBY_PAGE								6
#define LCD_STANDBY_ALRAM_PAGE							7
#define LCD_STANDBY_OK_PAGE								8
#define LCD_RUNNING_PAGE								9
#define LCD_SELECT_STOP_PAGE							10
#define LCD_MONITOR_PAGE								11
#define LCD_RESULT_PAGE									12
#define LCD_EORROR_POPUP_PAGE							13
#define LCD_EORROR_WAIT_PAGE							14



#define LCD_INFO_INFORMATION_PAGE						21
#define LCD_INFO_STERILANT_PAGE							22
#define LCD_INFO_HISTORY_PAGE							23

#define LCD_USER_SETTING_PAGE							24
#define LCD_USER_LEAKTEST_PAGE							25
#define LCD_USER_HEATINGTEST_PAGE						26
#define LCD_USER_PARTTEST_PAGE							27

#define LCD_ADMIN_PMSCHEDULE_PAGE						28
#define LCD_ADMIN_PARTSTEST_PAGE						29

#define LCD_FACTORY_INFOSETTING_PAGE					30
#define LCD_FACTORY_CALIBRATION_PAGE					31
#define LCD_FACTORY_TESTMODE_PAGE						32
#define LCD_FACTORY_CONTROLSETTING_PAGE					33
#define LCD_FACTORY_TESTMODE_PROCESSSETTINGS1_PAGE		34
#define LCD_FACTORY_TESTMODE_PROCESSSETTINGS2_PAGE		35
#define LCD_FACTORY_TESTMODE_PROCESSTEST1_PAGE			36
#define LCD_FACTORY_TESTMODE_PROCESSTEST2_PAGE			37
#define LCD_FACTORY_TESTMODE_PROCESSTEST3_PAGE			38
#define LCD_FACTORY_TESTMODE_PROCESSTEST4_PAGE			39

#define LCD_FACTORY_TESTMODE_SELECT_CYCLE_PAGE			44

#define LCD_LOGIN_POPUP_PAGE							45
#define LCD_CREATE_ID_POPUP_PAGE						46
#define LCD_CREATE_ID_POPUP_PAGE2						47
#define LCD_MANAGEMENT_CHANGE_PW_POPUP1					48
#define LCD_MANAGEMENT_CHANGE_PW_POPUP2					49
#define LCD_MANAGEMENT_DELET_ID_POPUP					50
#define LCD_MANAGEMENT_WRONG_PW_POPUP					51
#define LCD_LOGOUT_POPUP								52

#define LCD_FACTORY_CONTROLTEST_PAGE					80
#define LCD_LOADING_PAGE								90

struct Process_data_format {
	unsigned int PartsSetting;
	unsigned int	Time;
};

struct date_format {
	unsigned char year, month, day;
	unsigned char hour, minute, second;
};




void InitLCD();
void DisplayFirstPage();
void DisplayPage();

void LCD_Process();
void LCD_Function_Process(int index, int value);
void LCD_02(int index, int value);
void LCD_03(int index, int value);
void LCD_04(int index, int value);
void LCD_05(int index, int value);
void LCD_06(int index, int value);
void LCD_07(int index, int value);
void LCD_08(int index, int value);
void LCD_09(int index, int value);
void LCD_10(int index, int value);
void LCD_11(int index, int value);
void LCD_12(int index, int value);
void LCD_13(int index, int value);
void LCD_14(int index, int value);

void LCD_21(int index, int value);
void LCD_22(int index, int value);
void LCD_23(int index, int value);
void LCD_24(int index, int value);
void LCD_25(int index, int value);
void LCD_26(int index, int value);
void LCD_27(int index, int value);
void LCD_28(int index, int value);

void LCD_30(int index, int value);
void LCD_31(int index, int value);
void LCD_33(int index, int value);
void LCD_34(int index, int value);
void LCD_35(int index, int value);
void LCD_36(int index, int value);
void LCD_37(int index, int value);
void LCD_38(int index, int value);
void LCD_39(int index, int value);
void LCD_44(int index, int value);
void LCD_60(int index, int value);

void DoActionButton(int key);

void ProcessSettingButton(int key);

void DisplayProcessIcon(int index, int value);
void DisplayStepIcon(int index, int value);
void DisplayPartsIcon(int index, int value);
void DisplayPartsTESTIcon(int index, int value);
void DisplayProcessIcons(int index);
void DisplayStepIcons(int index);
void DisplayPartsIcons(void);
void DisplaySelectIcon(int index, int value);
void DisplayIcon(int page, int index, int value);

void DisplayTime(int page, int index, unsigned int icentisecond);
void DisplayTimeValues();
void ReadStepTime();
void ReadProcessTime();
void ReadCycleTime();

void DisplayValue(int index, float value);
void DisplayCycleValue(int value);
void DisplayTempValues();
void DisplayProcessSettingValues();
void DisplayProcessTestValues();
void DisplayNormalValues();
void DisplayVacuumSensor();
void DisplayRFIDNumber();

//페이지별 디스플레이 구현
void Display02page();
void Display03page();
void Display04page();
void Display05page();

void Display06page();
void Display07page();
void Display08page();
void Display09page();
void Display10page();
void Display11page();
void Display12page();

void Display21page();
void Display22page();
void Display23page();
void Display24page();
void Display25page();
void Display26page();
void Display27page();
void Display28page();
void Display30page();
void Display31page();
void Display33page();
void Display34page();
void Display35page();
void Display36page();
void Display37page();
void Display38page();
void Display39page();
void Display44page();


void DisplayPageValue(int page ,int index, int value);
void DisplayPage4Char(int page ,int index, char *msg);
void DisplayPage8Char(int page ,int index, char *msg);
void DisplayPage10Char(int page ,int index, unsigned char *msg);
void DisplayPage20Char(int page ,int index, unsigned char *msg);

void DisplayMsg(int page ,int index, char *msg);


//GET_TIME&DATA
void GetTime();


//RTC
void ReadRTC(unsigned char *year, unsigned char *month, unsigned char *day, unsigned char *week,
				unsigned char *hour, unsigned char *minute, unsigned char *second);
void SetRTCFromLCD();

void ReadInforDataFromLCD();

void DisplayTemprature();


unsigned char hex2bcd (unsigned char x);
unsigned char bcd2bin (unsigned char x);

//Graph
void DisplayDot(int vp1,int vp2,int x, int y);
void DisplayInitTempGraph();
void DisplayInitVacuumGraph();
void DisplayTempGraph(int number, int color);
void DisplayVacuumGraph(int number, int color);

#endif /* SRC_PERIPHERAL_LCD_H_ */
