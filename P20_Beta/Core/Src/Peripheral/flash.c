/*
 * flash.c
 *
 *  Created on: 2023. 1. 11.
 *      Author: CBT
 */

#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "hardware.h"
#include "sensor.h"
#include "peripheral.h"


__attribute__((__section__(".user_data"))) const char userConfig[1024];

uint32_t startTick, endTick, duration;
/*
startTick = HAL_GetTick(); // 기능 실행 전 타임스탬프 캡처
// 여기에 측정하고자 하는 기능 혹은 함수 호출
endTick = HAL_GetTick(); // 기능 실행 후 타임스탬프 캡처
duration = endTick - startTick; // 실행 시간(밀리초 단위)
*/


#define USER_FLASH_LAST_PAGE_ADDRESS 0x080FFFFF
#define FLASH_PAGE_SIZE               0x40000  // 256KB, F4 시리즈에 따라 다를 수 있음
//#define USER_DATA_FLASH_ADDRESS      (USER_FLASH_LAST_PAGE_ADDRESS + 1 - FLASH_PAGE_SIZE)
#define USER_DATA_FLASH_ADDRESS      0x08040000

char userdata[1024];
//extern struct data_format	CycleData[3][7][21];
extern struct Process_data_format	CycleData[7][21];
extern int CycleName;

extern int DoorSettingTemp[3], ChamberSettingTemp[3], ChamberBackSettingTemp[3], VaporizerSettingTemp[3];
extern int PreesureCondition[3];
extern int PlasmaTime[2];

extern int CalibrationTemp[4];
extern float CalibrationVacuum;

extern int TestVacuumValue;
extern int TestLeakValue;
extern int TestTempErrorValue;


extern struct RFID_format RFIDData;
extern struct date_format today_date;

unsigned char flash_sterilant_production_year[3],flash_sterilant_production_month[3],flash_sterilant_production_day[3];
unsigned char flash_sterilant_production_number[3];
unsigned char flash_sterilant_open_year[3], flash_sterilant_open_month[3], flash_sterilant_open_day[3];
unsigned char flash_sterilant_volume[3];

unsigned char flash_MODEL_NAME[10];
unsigned char flash_SERIAL_NUMBER[10];
unsigned char flash_DEPARTMENT_NAME[10];
unsigned char flash_FACILITY_NAME[10];
unsigned char flash_HARDWARE_VERSION[10];
unsigned char flash_SOFTWARE_VERSION[10];
unsigned char flash_LANGUAGE[10];

unsigned char flash_ID[5][10];
unsigned char flash_PW[5][10];
unsigned char flashuserCount;

#define	NONE				0x00
#define	VACUUMVALVE			0x01
#define	VENTVALVE			0x02
#define	INJECTIONVALVE		0x04
#define	PERIPUMP			0x08
#define	PLASMA				0x10
#define	PRESSURE1			0x20
#define	PRESSURE2			0x40
#define	PRESSURE3			0x60

#define CALIBRATIONVACUUM_DATA		0//int	[1]

#define	DOORSETTINGTEMP_DATA		5//int [3]
#define CHAMBERSETTINGTEMP_DATA		8//int [3]
#define CHAMBERBACKSETTINGTEMP_DATA	11//int [3]
#define VAPORIZERSETTINGTEMP_DATA	14//int [3]

#define PRESSURECONDITION_DATA		17//int [3]
#define	PLASMATIME_DATA				20//int [2]

#define CALIBRATIONTEMP_DATA		22//int [4]


#define TESTVACUUMVALUE_DATA		27//int	[1]
#define TESTLEAKVALUE_DATA			28//int	[1]
#define TESTTEMPERRORVALUE_DATA		29//int	[1]

#define PRODUCTION_YEAR		32//char[1]
#define PRODUCTION_MONTH	33//char[1]
#define PRODUCTION_DAY		34//char[1]
#define PRODUCTION_NUMBER	35//char[1]
#define OPEN_YEAR			36//char[1]
#define OPEN_MONTH			37//char[1]
#define OPEN_DAY			38//char[1]
#define VOLUME				39//char[1]

//32~39
//40~47
//48~55

#define MODEL_NAME			60//char[10](문자입력)
#define SERIAL_NUMBER		70//char[10](문자입력)
#define DEPARTMENT_NAME		80//char[10](문자입력)
#define FACILITY_NAME		90//char[10](문자입력)
#define HARDWARE_VERSION	100//char[10](숫자입력)
#define SOFTWARE_VERSION	110//char[10](숫자입력)
#define FLASHLANGUAGE		120//char[10](숫자입력)


#define	CARBONFILTERMAX 	160//char[2](숫자입력)
#define	HEPAFILTERMAX		162//char[2](숫자입력)
#define	PLASMAASSYMAX		164//char[2](숫자입력)

#define	CARBONFILTER		166//char[2](숫자입력)
#define	HEPAFILTER			168//char[2](숫자입력)
#define	PLASMAASSY			170//char[2](숫자입력)

#define	TOTALCOUNT			172//char[2](숫자입력)
#define	DAILYCOUNT			174//char[2](숫자입력)

#define	FLASHUSERCOUNT		176//char[1](숫자입력)

#define	FLASHID				180//char[5][10](숫자입력)
#define	FLASHPW				230//char[5][10](숫자입력)

#define CYCLEDATA1			280	//[240]
#define CYCLEDATA2			520	//[240]
#define CYCLEDATA3			760	//[240]



/*
 *
RFID 저장 형식
unsigned char 생산년,월,일[3]
unsigned char 생산 넘버[1]
unsigned char 인식년,월,일[3]
unsigned char 잔량[1]
총[8]

위 변수들을 20개 저장

1.비교
 - RFID 읽을때 저장된 생산 년,월,일 비교
 - 그리고 생산 넘버 비교

2. 값 처리
 - 이전값이 있으면 인식 년,월,일, 잔량 불러오기
 - 이전값이 없으면 인식 년,월,일, 잔량 부여 후 저장

3. 사용 후 저장
 - 사용 후 1번으로 저장
 - 다른 용액통이 인식될 경우 2번에 저장...
 - 이런식으로 20번까지 저장 후
 - 새로운 용액이 인식 될 경우 1번 삭제후 2번을 1번으로 ....20번을 19번으로 저장
 - 새로운 용액은 20번에 저장.
 - 기존 인식된 용액통을 불러오고 저장할때는 해당 순서에 저장
 *
 *
 *
순서
1. 리드
 - RFID 리드
 - Flash 데이터 리드
2. 비교
 - RFID 태그 데이터와 현재 저장 데이터 비교
3. 저장
 - 없으면 기존 IndexRFIDFlash에 따라 저장
 - 있으면 기IndexRFIDFlash++
*/

void Write_Flash(){

	startTick = HAL_GetTick();
	DisplayPage(LCD_LOADING_PAGE);
	unsigned char ucData[1024]={};

	//ucData[CALIBRATIONVACUUM_DATA]=CalibrationVacuum;
	float2char(CalibrationVacuum, ucData + CALIBRATIONVACUUM_DATA);

	ucData[DOORSETTINGTEMP_DATA]=DoorSettingTemp[0];
	ucData[DOORSETTINGTEMP_DATA+1]=DoorSettingTemp[1];
	ucData[DOORSETTINGTEMP_DATA+2]=DoorSettingTemp[2];

	ucData[CHAMBERSETTINGTEMP_DATA]=ChamberSettingTemp[0];
	ucData[CHAMBERSETTINGTEMP_DATA+1]=ChamberSettingTemp[1];
	ucData[CHAMBERSETTINGTEMP_DATA+2]=ChamberSettingTemp[2];

	ucData[CHAMBERBACKSETTINGTEMP_DATA]=ChamberBackSettingTemp[0];
	ucData[CHAMBERBACKSETTINGTEMP_DATA+1]=ChamberBackSettingTemp[1];
	ucData[CHAMBERBACKSETTINGTEMP_DATA+2]=ChamberBackSettingTemp[2];

	ucData[VAPORIZERSETTINGTEMP_DATA]=VaporizerSettingTemp[0];
	ucData[VAPORIZERSETTINGTEMP_DATA+1]=VaporizerSettingTemp[1];
	ucData[VAPORIZERSETTINGTEMP_DATA+2]=VaporizerSettingTemp[2];

	ucData[PRESSURECONDITION_DATA]=PreesureCondition[0];
	ucData[PRESSURECONDITION_DATA+1]=PreesureCondition[1];
	ucData[PRESSURECONDITION_DATA+2]=PreesureCondition[2];

	ucData[PLASMATIME_DATA]=PlasmaTime[0];
	ucData[PLASMATIME_DATA+1]=PlasmaTime[1];

	ucData[CALIBRATIONTEMP_DATA]=CalibrationTemp[0];
	ucData[CALIBRATIONTEMP_DATA+1]=CalibrationTemp[1];
	ucData[CALIBRATIONTEMP_DATA+2]=CalibrationTemp[2];
	ucData[CALIBRATIONTEMP_DATA+3]=CalibrationTemp[3];


	ucData[TESTVACUUMVALUE_DATA]=TestVacuumValue;
	ucData[TESTLEAKVALUE_DATA]=TestLeakValue;
	ucData[TESTTEMPERRORVALUE_DATA]=TestTempErrorValue;



	for(int i=0;i<3;i++){
		ucData[PRODUCTION_YEAR+8*i]=flash_sterilant_production_year[i];
		ucData[PRODUCTION_MONTH+8*i]=flash_sterilant_production_month[i];
		ucData[PRODUCTION_DAY+8*i]=flash_sterilant_production_day[i];
		ucData[PRODUCTION_NUMBER+8*i]=flash_sterilant_production_number[i];

		ucData[OPEN_YEAR+8*i]=flash_sterilant_open_year[i];
		ucData[OPEN_MONTH+8*i]=flash_sterilant_open_month[i];
		ucData[OPEN_DAY+8*i]=flash_sterilant_open_day[i];

		ucData[VOLUME+8*i]=flash_sterilant_volume[i];
	}

	for(int i=0;i<10;i++){
		ucData[MODEL_NAME+i]=flash_MODEL_NAME[i];
		ucData[SERIAL_NUMBER+i]=flash_SERIAL_NUMBER[i];
		ucData[DEPARTMENT_NAME+i]=flash_DEPARTMENT_NAME[i];
		ucData[FACILITY_NAME+i]=flash_FACILITY_NAME[i];
		ucData[HARDWARE_VERSION+i]=flash_HARDWARE_VERSION[i];
		ucData[SOFTWARE_VERSION+i]=flash_SOFTWARE_VERSION[i];
		ucData[FLASHLANGUAGE+i]=flash_LANGUAGE[i];
	}

	ucData[CARBONFILTERMAX]=(int)(CarbonFilterMax/100);
	ucData[CARBONFILTERMAX+1]=(int)(CarbonFilterMax%100);
	ucData[HEPAFILTERMAX]=(int)(HEPAFilterMax/100);
	ucData[HEPAFILTERMAX+1]=(int)(HEPAFilterMax%100);
	ucData[PLASMAASSYMAX]=(int)(PlasmaAssyMax/100);
	ucData[PLASMAASSYMAX+1]=(int)(PlasmaAssyMax%100);

	ucData[CARBONFILTER]=(int)(CarbonFilter/100);
	ucData[CARBONFILTER+1]=(int)(CarbonFilter%100);
	ucData[HEPAFILTER]=(int)(HEPAFilter/100);
	ucData[HEPAFILTER+1]=(int)(HEPAFilter%100);
	ucData[PLASMAASSY]=(int)(PlasmaAssy/100);
	ucData[PLASMAASSY+1]=(int)(PlasmaAssy%100);

	ucData[TOTALCOUNT]=(int)(totalCount/100);
	ucData[TOTALCOUNT+1]=(int)(totalCount%100);
	ucData[DAILYCOUNT]=dailyCount;
	//ucData[DAILYCOUNT+1]=(int)(dailyCount%100);

	ucData[FLASHUSERCOUNT]=flashuserCount;

	for(int j=0;j<5;j++){
		for(int i=0;i<10;i++){
			ucData[FLASHID+i+10*j]=flash_ID[j][i];
			ucData[FLASHPW+i+10*j]=flash_PW[j][i];
		}
	}

	int j=0;

	j=CYCLEDATA1;
	for(int i2=1;i2<7;i2++){
		for(int i3=1;i3<21;i3++){
			ucData[j]=userdata[j];
			ucData[j+1]=userdata[j+1];
			j+=2;
		}
	}
	j=CYCLEDATA2;
	for(int i2=1;i2<7;i2++){
		for(int i3=1;i3<21;i3++){
			ucData[j]=userdata[j];
			ucData[j+1]=userdata[j+1];
			j+=2;
		}
	}
	j=CYCLEDATA3;
	for(int i2=1;i2<7;i2++){
		for(int i3=1;i3<21;i3++){
			ucData[j]=userdata[j];
			ucData[j+1]=userdata[j+1];
			j+=2;
		}
	}
	if(CycleName==1){
		j=CYCLEDATA1;
	}
	else if(CycleName==2){
		j=CYCLEDATA2;
	}
	else if(CycleName==3){
		j=CYCLEDATA3;
	}
	else{
		j=CYCLEDATA1;
	}
	for(int i2=1;i2<7;i2++){
		for(int i3=1;i3<21;i3++){
			ucData[j]=CycleData[i2][i3].PartsSetting;
			ucData[j+1]=CycleData[i2][i3].Time;
			j+=2;
		}
	}


	HAL_FLASH_Unlock();
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGSERR );
	FLASH_Erase_Sector(FLASH_SECTOR_6, VOLTAGE_RANGE_3);

	for(int i = 0; i < 1024; i++) {
	 HAL_FLASH_Program(TYPEPROGRAM_BYTE, (uint32_t)&userConfig[0] + i, ucData[i]);
	}

	HAL_FLASH_Lock();
	Read_Flash();
	endTick = HAL_GetTick(); // 기능 실행 후 타임스탬프 캡처
	duration = endTick - startTick; // 실행 시간(밀리초 단위)
}

void Read_Flash(){

   for (int i = 0; i < 1024; i++) {
		// Flash 메모리의 주소에서 직접 데이터를 읽어와 ReadTest 배열에 저장합니다.
	   userdata[i] = Flash_Read_Int(USER_DATA_FLASH_ADDRESS + i);
	}
	DoorSettingTemp[0]=userdata[DOORSETTINGTEMP_DATA];
	DoorSettingTemp[1]=userdata[DOORSETTINGTEMP_DATA+1];
	DoorSettingTemp[2]=userdata[DOORSETTINGTEMP_DATA+2];

	ChamberSettingTemp[0]=userdata[CHAMBERSETTINGTEMP_DATA];
	ChamberSettingTemp[1]=userdata[CHAMBERSETTINGTEMP_DATA+1];
	ChamberSettingTemp[2]=userdata[CHAMBERSETTINGTEMP_DATA+2];

	ChamberBackSettingTemp[0]=userdata[CHAMBERBACKSETTINGTEMP_DATA];
	ChamberBackSettingTemp[1]=userdata[CHAMBERBACKSETTINGTEMP_DATA+1];
	ChamberBackSettingTemp[2]=userdata[CHAMBERBACKSETTINGTEMP_DATA+2];

	VaporizerSettingTemp[0]=userdata[VAPORIZERSETTINGTEMP_DATA];
	VaporizerSettingTemp[1]=userdata[VAPORIZERSETTINGTEMP_DATA+1];
	VaporizerSettingTemp[2]=userdata[VAPORIZERSETTINGTEMP_DATA+2];

	if(DoorSettingTemp[0]==0){
		DoorSettingTemp[0]=55;
	}
	if(DoorSettingTemp[1]==0){
		DoorSettingTemp[1]=55;
	}
	if(ChamberSettingTemp[0]==0){
		ChamberSettingTemp[0]=55;
	}
	if(ChamberSettingTemp[1]==0){
		ChamberSettingTemp[1]=55;
	}
	if(ChamberBackSettingTemp[0]==0){
		ChamberBackSettingTemp[0]=55;
	}
	if(ChamberBackSettingTemp[1]==0){
		ChamberBackSettingTemp[1]=55;
	}
	if(VaporizerSettingTemp[0]==0){
		VaporizerSettingTemp[0]=80;
	}
	if(VaporizerSettingTemp[1]==0){
		VaporizerSettingTemp[1]=130;
	}

	PreesureCondition[0]=userdata[PRESSURECONDITION_DATA];
	PreesureCondition[1]=userdata[PRESSURECONDITION_DATA+1];
	PreesureCondition[2]=userdata[PRESSURECONDITION_DATA+2];

	if(PreesureCondition[0]==0){
			PreesureCondition[0]=50;
	}
	if(PreesureCondition[1]==0){
			PreesureCondition[1]=25;
	}
	if(PreesureCondition[2]==0){
			PreesureCondition[2]=25;
	}


	PlasmaTime[0]=userdata[PLASMATIME_DATA];
	if(PlasmaTime[0]==0){
		PlasmaTime[0]=1;
	}
	PlasmaTime[1]=userdata[PLASMATIME_DATA+1];

	CalibrationTemp[0]=userdata[CALIBRATIONTEMP_DATA];
	CalibrationTemp[1]=userdata[CALIBRATIONTEMP_DATA+1];
	CalibrationTemp[2]=userdata[CALIBRATIONTEMP_DATA+2];
	CalibrationTemp[3]=userdata[CALIBRATIONTEMP_DATA+3];

	for(int i=0;i<4;i++){
		if(CalibrationTemp[i]==0){
			CalibrationTemp[i]=10;
		}
	}
	//CalibrationVacuum=userdata[CALIBRATIONVACUUM_DATA];
	CalibrationVacuum = char2float((unsigned char *)(userdata + CALIBRATIONVACUUM_DATA));
	if(CalibrationVacuum==0){
		CalibrationVacuum=100;
	}

	TestVacuumValue=userdata[TESTVACUUMVALUE_DATA];
	if(TestVacuumValue==0){
		TestVacuumValue=10;
	}
	TestLeakValue=userdata[TESTLEAKVALUE_DATA];
	if(TestLeakValue==0){
		TestLeakValue=2;
	}
	TestTempErrorValue=userdata[TESTTEMPERRORVALUE_DATA];
	if(TestTempErrorValue==0){
		TestTempErrorValue=2;
	}



	for(int i=0;i<3;i++){
		flash_sterilant_production_year[i]=userdata[PRODUCTION_YEAR+8*i];
		flash_sterilant_production_month[i]=userdata[PRODUCTION_MONTH+8*i];
		flash_sterilant_production_day[i]=userdata[PRODUCTION_DAY+8*i];
		flash_sterilant_production_number[i]=userdata[PRODUCTION_NUMBER+8*i];

		flash_sterilant_open_year[i]=userdata[OPEN_YEAR+8*i];
		flash_sterilant_open_month[i]=userdata[OPEN_MONTH+8*i];
		flash_sterilant_open_day[i]=userdata[OPEN_DAY+8*i];

		flash_sterilant_volume[i]=userdata[VOLUME+8*i];
	}

	for(int i=0;i<10;i++){
		flash_MODEL_NAME[i]=userdata[MODEL_NAME+i];
		flash_HARDWARE_VERSION[i]=userdata[HARDWARE_VERSION+i];
		flash_SOFTWARE_VERSION[i]=userdata[SOFTWARE_VERSION+i];
		flash_LANGUAGE[i]=userdata[FLASHLANGUAGE+i];
		flash_SERIAL_NUMBER[i]=userdata[SERIAL_NUMBER+i];
		flash_DEPARTMENT_NAME[i]=userdata[DEPARTMENT_NAME+i];
		flash_FACILITY_NAME[i]=userdata[FACILITY_NAME+i];
	}


	if(flash_MODEL_NAME[0]==0){
		sprintf(flash_MODEL_NAME,"FN-P20    ");
	}
	if(flash_SERIAL_NUMBER[0]==0){
		sprintf(flash_SERIAL_NUMBER,"CBTP221201");
	}
	if(flash_DEPARTMENT_NAME[0]==0){
		sprintf(flash_DEPARTMENT_NAME,"Clean-Hosp");
	}
	if(flash_FACILITY_NAME[0]==0){
		sprintf(flash_FACILITY_NAME,"DevlopPart");
	}
	if(flash_HARDWARE_VERSION[0]==0){
		sprintf(flash_HARDWARE_VERSION,"1.0.0     ");
	}
	if(flash_SOFTWARE_VERSION[0]==0){
		sprintf(flash_SOFTWARE_VERSION,"1.0.0     ");
	}
	if(flash_LANGUAGE[0]==0||flash_LANGUAGE[0]=="E"){
		sprintf(flash_LANGUAGE,"English   ");
	}

	CarbonFilterMax=(userdata[CARBONFILTERMAX]*100)+(userdata[CARBONFILTERMAX+1]);
	if(CarbonFilterMax==0){
		CarbonFilterMax=4000;
	}
	HEPAFilterMax=(userdata[HEPAFILTERMAX]*100)+(userdata[HEPAFILTERMAX+1]);
	if(HEPAFilterMax==0){
		HEPAFilterMax=4000;
	}
	PlasmaAssyMax=(userdata[PLASMAASSYMAX]*100)+(userdata[PLASMAASSYMAX+1]);
	if(PlasmaAssyMax==0){
		PlasmaAssyMax=4000;
	}

	CarbonFilter=(userdata[CARBONFILTER]*100)+(userdata[CARBONFILTER+1]);
	if(CarbonFilter==0){
		CarbonFilter=4000;
	}
	HEPAFilter=(userdata[HEPAFILTER]*100)+(userdata[HEPAFILTER+1]);
	if(HEPAFilter==0){
		HEPAFilter=4000;
	}
	PlasmaAssy=(userdata[PLASMAASSY]*100)+(userdata[PLASMAASSY+1]);
	if(PlasmaAssy==0){
		PlasmaAssy=4000;
	}


	totalCount=(userdata[TOTALCOUNT]*100)+(userdata[TOTALCOUNT+1]);
	//dailyCount=(userdata[DAILYCOUNT]*100)+(userdata[DAILYCOUNT+1]);
	dailyCount=userdata[DAILYCOUNT];

	flashuserCount=userdata[FLASHUSERCOUNT];
	if(flashuserCount<=0){
		flashuserCount=0;
	}

	for(int j=0;j<5;j++){
		for(int i=0;i<10;i++){
			flash_ID[j][i]=userdata[FLASHID+i+10*j];
			flash_PW[j][i]=userdata[FLASHPW+i+10*j];
		}
	}


	int j=0;

	if(CycleName==1){
		j=CYCLEDATA1;
	}
	else if(CycleName==2){
		j=CYCLEDATA2;

	}
	else if(CycleName==3){
		j=CYCLEDATA3;
	}
	for(int i2=1;i2<7;i2++){
		for(int i3=1;i3<21;i3++){
			CycleData[i2][i3].PartsSetting=userdata[j];
			CycleData[i2][i3].Time=(userdata[j+1]);
			j+=2;
		}
	}
	if(CycleName==1){
		if(CycleData[1][1].Time==0){
			ShortCycle();
		}
		else if(CycleData[1][1].Time==1){
			TestCycle();
		}
	}
	else if(CycleName==2){
		if(CycleData[1][1].Time==0){
			StandardCycle();
		}
	}
	else if(CycleName==3){
		if(CycleData[1][1].Time==0){
			AdvancedCycle();
		}
	}
}
void TestCycle(){
	CycleData[1][1].PartsSetting=VACUUMVALVE;
	CycleData[1][1].Time=1;
	CycleData[1][2].PartsSetting=VACUUMVALVE;
	CycleData[1][2].Time=1;
	CycleData[1][3].PartsSetting=VACUUMVALVE+PRESSURE1;
	CycleData[1][3].Time=1;
	CycleData[1][4].PartsSetting=VENTVALVE+INJECTIONVALVE;
	CycleData[1][4].Time=1;
	CycleData[1][5].PartsSetting=NONE;
	CycleData[1][5].Time=1;
	CycleData[1][6].PartsSetting=NONE;
	CycleData[1][6].Time=1;
	CycleData[1][7].PartsSetting=VACUUMVALVE;
	CycleData[1][7].Time=1;
	CycleData[1][8].PartsSetting=VACUUMVALVE;
	CycleData[1][8].Time=1;
	CycleData[1][9].PartsSetting=VACUUMVALVE;
	CycleData[1][9].Time=1;
	CycleData[1][10].PartsSetting=VACUUMVALVE;
	CycleData[1][10].Time=1;
	CycleData[1][11].PartsSetting=VACUUMVALVE+PRESSURE2;
	CycleData[1][11].Time=1;
	CycleData[1][12].PartsSetting=VACUUMVALVE+PERIPUMP;
	CycleData[1][12].Time=1;

	CycleData[2][1].PartsSetting=INJECTIONVALVE;
	CycleData[2][1].Time=1;
	CycleData[2][2].PartsSetting=PRESSURE3;
	CycleData[2][2].Time=1;
	CycleData[2][3].PartsSetting=NONE;
	CycleData[2][3].Time=1;
	CycleData[2][4].PartsSetting=NONE;
	CycleData[2][4].Time=1;
	CycleData[2][5].PartsSetting=NONE;
	CycleData[2][5].Time=1;
	CycleData[2][6].PartsSetting=NONE;
	CycleData[2][6].Time=1;
	CycleData[2][7].PartsSetting=NONE;
	CycleData[2][7].Time=1;
	CycleData[2][8].PartsSetting=VENTVALVE+INJECTIONVALVE;
	CycleData[2][8].Time=1;
	CycleData[2][9].PartsSetting=VACUUMVALVE;
	CycleData[2][9].Time=1;
	CycleData[2][10].PartsSetting=NONE;
	CycleData[2][10].Time=1;
	CycleData[2][11].PartsSetting=VENTVALVE;
	CycleData[2][11].Time=1;
	CycleData[2][12].PartsSetting=VACUUMVALVE;
	CycleData[2][12].Time=1;
	CycleData[2][13].PartsSetting=VENTVALVE;
	CycleData[2][13].Time=1;

	CycleData[3][1].PartsSetting=VACUUMVALVE+PLASMA;
	CycleData[3][1].Time=1;
	CycleData[3][2].PartsSetting=VACUUMVALVE;
	CycleData[3][2].Time=1;
	CycleData[3][3].PartsSetting=VACUUMVALVE+PLASMA;
	CycleData[3][3].Time=1;
	CycleData[3][4].PartsSetting=VACUUMVALVE;
	CycleData[3][4].Time=1;
	CycleData[3][5].PartsSetting=VACUUMVALVE;
	CycleData[3][5].Time=1;
	CycleData[3][6].PartsSetting=VACUUMVALVE;
	CycleData[3][6].Time=1;
	CycleData[3][7].PartsSetting=VACUUMVALVE;
	CycleData[3][7].Time=1;
	CycleData[3][8].PartsSetting=VACUUMVALVE;
	CycleData[3][8].Time=1;
	CycleData[3][9].PartsSetting=VACUUMVALVE+PRESSURE2;
	CycleData[3][9].Time=1;
	CycleData[3][10].PartsSetting=VACUUMVALVE+PERIPUMP;
	CycleData[3][10].Time=1;

	CycleData[4][1].PartsSetting=INJECTIONVALVE;
	CycleData[4][1].Time=1;
	CycleData[4][2].PartsSetting=NONE;
	CycleData[4][2].Time=1;
	CycleData[4][3].PartsSetting=PRESSURE3;
	CycleData[4][3].Time=1;
	CycleData[4][4].PartsSetting=NONE;
	CycleData[4][4].Time=1;
	CycleData[4][5].PartsSetting=NONE;
	CycleData[4][5].Time=1;
	CycleData[4][6].PartsSetting=NONE;
	CycleData[4][6].Time=1;
	CycleData[4][7].PartsSetting=NONE;
	CycleData[4][7].Time=1;
	CycleData[4][8].PartsSetting=VENTVALVE+INJECTIONVALVE;
	CycleData[4][8].Time=1;
	CycleData[4][9].PartsSetting=VACUUMVALVE;
	CycleData[4][9].Time=1;
	CycleData[4][10].PartsSetting=NONE;
	CycleData[4][10].Time=1;
	CycleData[4][11].PartsSetting=VENTVALVE;
	CycleData[4][11].Time=1;
	CycleData[4][12].PartsSetting=VACUUMVALVE;
	CycleData[4][12].Time=1;
	CycleData[4][13].PartsSetting=VENTVALVE;
	CycleData[4][13].Time=1;

	CycleData[5][1].PartsSetting=VACUUMVALVE+PLASMA;
	CycleData[5][1].Time=1;
	CycleData[5][2].PartsSetting=VACUUMVALVE;
	CycleData[5][2].Time=1;
	CycleData[5][3].PartsSetting=VACUUMVALVE+PLASMA;
	CycleData[5][3].Time=1;
	CycleData[5][4].PartsSetting=VACUUMVALVE;
	CycleData[5][4].Time=1;
	CycleData[5][5].PartsSetting=VACUUMVALVE+INJECTIONVALVE;
	CycleData[5][5].Time=1;
	CycleData[5][6].PartsSetting=VACUUMVALVE;
	CycleData[5][6].Time=1;
	CycleData[5][7].PartsSetting=VACUUMVALVE;
	CycleData[5][7].Time=1;
	CycleData[5][8].PartsSetting=VACUUMVALVE;
	CycleData[5][8].Time=1;
	CycleData[5][9].PartsSetting=VACUUMVALVE;
	CycleData[5][9].Time=1;
	CycleData[5][10].PartsSetting=VACUUMVALVE;
	CycleData[5][10].Time=1;

	CycleData[6][1].PartsSetting=VENTVALVE+INJECTIONVALVE;
	CycleData[6][1].Time=1;
	CycleData[6][2].PartsSetting=VACUUMVALVE;
	CycleData[6][2].Time=1;
	CycleData[6][3].PartsSetting=VENTVALVE+INJECTIONVALVE;
	CycleData[6][3].Time=1;
	CycleData[6][4].PartsSetting=VACUUMVALVE;
	CycleData[6][4].Time=1;
	CycleData[6][5].PartsSetting=VENTVALVE;
	CycleData[6][5].Time=1;
	CycleData[6][6].PartsSetting=NONE;
	CycleData[6][6].Time=1;
}

void ShortCycle(){
	CycleData[1][1].PartsSetting=VACUUMVALVE;
	CycleData[1][1].Time=30;
	CycleData[1][2].PartsSetting=VACUUMVALVE;
	CycleData[1][2].Time=30;
	CycleData[1][3].PartsSetting=VACUUMVALVE+PRESSURE1;
	CycleData[1][3].Time=30;
	CycleData[1][4].PartsSetting=VENTVALVE+INJECTIONVALVE;
	CycleData[1][4].Time=5;
	CycleData[1][5].PartsSetting=NONE;
	CycleData[1][5].Time=25;
	CycleData[1][6].PartsSetting=NONE;
	CycleData[1][6].Time=30;
	CycleData[1][7].PartsSetting=VACUUMVALVE;
	CycleData[1][7].Time=30;
	CycleData[1][8].PartsSetting=VACUUMVALVE;
	CycleData[1][8].Time=30;
	CycleData[1][9].PartsSetting=VACUUMVALVE;
	CycleData[1][9].Time=30;
	CycleData[1][10].PartsSetting=VACUUMVALVE;
	CycleData[1][10].Time=30;
	CycleData[1][11].PartsSetting=VACUUMVALVE+PRESSURE2;
	CycleData[1][11].Time=30;
	CycleData[1][12].PartsSetting=VACUUMVALVE+PERIPUMP;
	CycleData[1][12].Time=60;

	CycleData[2][1].PartsSetting=INJECTIONVALVE;
	CycleData[2][1].Time=1;
	CycleData[2][2].PartsSetting=PRESSURE3;
	CycleData[2][2].Time=29;
	CycleData[2][3].PartsSetting=NONE;
	CycleData[2][3].Time=30;
	CycleData[2][4].PartsSetting=NONE;
	CycleData[2][4].Time=30;
	CycleData[2][5].PartsSetting=NONE;
	CycleData[2][5].Time=30;
	CycleData[2][6].PartsSetting=NONE;
	CycleData[2][6].Time=30;
	CycleData[2][7].PartsSetting=NONE;
	CycleData[2][7].Time=30;
	CycleData[2][8].PartsSetting=VENTVALVE+INJECTIONVALVE;
	CycleData[2][8].Time=7;
	CycleData[2][9].PartsSetting=VACUUMVALVE;
	CycleData[2][9].Time=30;
	CycleData[2][10].PartsSetting=NONE;
	CycleData[2][10].Time=43;
	CycleData[2][11].PartsSetting=VENTVALVE;
	CycleData[2][11].Time=5;
	CycleData[2][12].PartsSetting=VACUUMVALVE;
	CycleData[2][12].Time=30;
	CycleData[2][13].PartsSetting=VENTVALVE;
	CycleData[2][13].Time=5;

	CycleData[3][1].PartsSetting=VACUUMVALVE+PLASMA;
	CycleData[3][1].Time=3;
	CycleData[3][2].PartsSetting=VACUUMVALVE;
	CycleData[3][2].Time=5;
	CycleData[3][3].PartsSetting=VACUUMVALVE+PLASMA;
	CycleData[3][3].Time=3;
	CycleData[3][4].PartsSetting=VACUUMVALVE;
	CycleData[3][4].Time=5;
	CycleData[3][5].PartsSetting=VACUUMVALVE+PLASMA;
	CycleData[3][5].Time=2;
	CycleData[3][6].PartsSetting=VACUUMVALVE;
	CycleData[3][6].Time=5;
	CycleData[3][7].PartsSetting=VACUUMVALVE+PLASMA;
	CycleData[3][7].Time=2;
	CycleData[3][8].PartsSetting=VACUUMVALVE;
	CycleData[3][8].Time=30;
	CycleData[3][9].PartsSetting=VACUUMVALVE;
	CycleData[3][9].Time=35;
	CycleData[3][10].PartsSetting=VACUUMVALVE;
	CycleData[3][10].Time=60;
	CycleData[3][11].PartsSetting=VACUUMVALVE+PRESSURE2;
	CycleData[3][11].Time=30;
	CycleData[3][12].PartsSetting=VACUUMVALVE+PERIPUMP;
	CycleData[3][12].Time=60;

	CycleData[4][1].PartsSetting=INJECTIONVALVE;
	CycleData[4][1].Time=1;
	CycleData[4][2].PartsSetting=PRESSURE3;
	CycleData[4][2].Time=29;
	CycleData[4][3].PartsSetting=NONE;
	CycleData[4][3].Time=30;
	CycleData[4][4].PartsSetting=NONE;
	CycleData[4][4].Time=30;
	CycleData[4][5].PartsSetting=NONE;
	CycleData[4][5].Time=30;
	CycleData[4][6].PartsSetting=NONE;
	CycleData[4][6].Time=30;
	CycleData[4][7].PartsSetting=NONE;
	CycleData[4][7].Time=30;
	CycleData[4][8].PartsSetting=VENTVALVE+INJECTIONVALVE;
	CycleData[4][8].Time=7;
	CycleData[4][9].PartsSetting=VACUUMVALVE;
	CycleData[4][9].Time=30;
	CycleData[4][10].PartsSetting=NONE;
	CycleData[4][10].Time=43;
	CycleData[4][11].PartsSetting=VENTVALVE;
	CycleData[4][11].Time=5;
	CycleData[4][12].PartsSetting=VACUUMVALVE;
	CycleData[4][12].Time=30;
	CycleData[4][13].PartsSetting=VENTVALVE;
	CycleData[4][13].Time=5;

	CycleData[5][1].PartsSetting=VACUUMVALVE+PLASMA;
	CycleData[5][1].Time=3;
	CycleData[5][2].PartsSetting=VACUUMVALVE;
	CycleData[5][2].Time=5;
	CycleData[5][3].PartsSetting=VACUUMVALVE+PLASMA;
	CycleData[5][3].Time=3;
	CycleData[5][4].PartsSetting=VACUUMVALVE;
	CycleData[5][4].Time=5;
	CycleData[5][5].PartsSetting=VACUUMVALVE+PLASMA+INJECTIONVALVE;
	CycleData[5][5].Time=2;
	CycleData[5][6].PartsSetting=VACUUMVALVE;
	CycleData[5][6].Time=5;
	CycleData[5][7].PartsSetting=VACUUMVALVE+PLASMA;
	CycleData[5][7].Time=2;
	CycleData[5][8].PartsSetting=VACUUMVALVE;
	CycleData[5][8].Time=30;
	CycleData[5][9].PartsSetting=VACUUMVALVE;
	CycleData[5][9].Time=5;
	CycleData[5][10].PartsSetting=VACUUMVALVE;
	CycleData[5][10].Time=30;
	CycleData[5][11].PartsSetting=VACUUMVALVE;
	CycleData[5][11].Time=30;
	CycleData[5][12].PartsSetting=VACUUMVALVE;
	CycleData[5][12].Time=60;

	CycleData[6][1].PartsSetting=VENTVALVE+INJECTIONVALVE;
	CycleData[6][1].Time=5;
	CycleData[6][2].PartsSetting=VACUUMVALVE;
	CycleData[6][2].Time=50;
	CycleData[6][3].PartsSetting=VENTVALVE+INJECTIONVALVE;
	CycleData[6][3].Time=5;
	CycleData[6][4].PartsSetting=VACUUMVALVE;
	CycleData[6][4].Time=50;
	CycleData[6][5].PartsSetting=VENTVALVE;
	CycleData[6][5].Time=9;
	CycleData[6][6].PartsSetting=NONE;
	CycleData[6][6].Time=1;
}


void StandardCycle(){
	CycleData[1][1].PartsSetting=VACUUMVALVE;
	CycleData[1][1].Time=60;
	CycleData[1][2].PartsSetting=VENTVALVE+INJECTIONVALVE;
	CycleData[1][2].Time=7;
	CycleData[1][3].PartsSetting=VACUUMVALVE;
	CycleData[1][3].Time=30;
	CycleData[1][4].PartsSetting=VACUUMVALVE;
	CycleData[1][4].Time=30;
	CycleData[1][5].PartsSetting=VACUUMVALVE+PRESSURE1;
	CycleData[1][5].Time=30;
	CycleData[1][6].PartsSetting=VENTVALVE;
	CycleData[1][6].Time=5;
	CycleData[1][7].PartsSetting=NONE;
	CycleData[1][7].Time=48;
	CycleData[1][8].PartsSetting=NONE;
	CycleData[1][8].Time=30;
	CycleData[1][9].PartsSetting=NONE;
	CycleData[1][9].Time=30;
	CycleData[1][10].PartsSetting=VACUUMVALVE;
	CycleData[1][10].Time=30;
	CycleData[1][11].PartsSetting=VACUUMVALVE;
	CycleData[1][11].Time=30;
	CycleData[1][12].PartsSetting=VACUUMVALVE;
	CycleData[1][12].Time=30;
	CycleData[1][13].PartsSetting=VACUUMVALVE;
	CycleData[1][13].Time=30;
	CycleData[1][14].PartsSetting=VACUUMVALVE+PRESSURE2;
	CycleData[1][14].Time=30;
	CycleData[1][15].PartsSetting=VACUUMVALVE+PERIPUMP;
	CycleData[1][15].Time=60;

	CycleData[2][1].PartsSetting=INJECTIONVALVE;
	CycleData[2][1].Time=1;
	CycleData[2][2].PartsSetting=PRESSURE3;
	CycleData[2][2].Time=29;
	CycleData[2][3].PartsSetting=NONE;
	CycleData[2][3].Time=30;
	CycleData[2][4].PartsSetting=NONE;
	CycleData[2][4].Time=60;
	CycleData[2][5].PartsSetting=NONE;
	CycleData[2][5].Time=60;
	CycleData[2][6].PartsSetting=NONE;
	CycleData[2][6].Time=60;
	CycleData[2][7].PartsSetting=VENTVALVE+INJECTIONVALVE;
	CycleData[2][7].Time=3;
	CycleData[2][8].PartsSetting=VACUUMVALVE;
	CycleData[2][8].Time=30;
	CycleData[2][9].PartsSetting=NONE;
	CycleData[2][9].Time=47;
	CycleData[2][10].PartsSetting=NONE;
	CycleData[2][10].Time=60;
	CycleData[2][11].PartsSetting=NONE;
	CycleData[2][11].Time=58;
	CycleData[2][12].PartsSetting=VENTVALVE+INJECTIONVALVE;
	CycleData[2][12].Time=7;
	CycleData[2][13].PartsSetting=VACUUMVALVE;
	CycleData[2][13].Time=30;
	CycleData[2][14].PartsSetting=VENTVALVE;
	CycleData[2][14].Time=5;

	CycleData[3][1].PartsSetting=VACUUMVALVE+PLASMA;
	CycleData[3][1].Time=3;
	CycleData[3][2].PartsSetting=VACUUMVALVE;
	CycleData[3][2].Time=5;
	CycleData[3][3].PartsSetting=VACUUMVALVE+PLASMA;
	CycleData[3][3].Time=3;
	CycleData[3][4].PartsSetting=VACUUMVALVE;
	CycleData[3][4].Time=5;
	CycleData[3][5].PartsSetting=VACUUMVALVE+PLASMA;
	CycleData[3][5].Time=2;
	CycleData[3][6].PartsSetting=VACUUMVALVE;
	CycleData[3][6].Time=5;
	CycleData[3][7].PartsSetting=VACUUMVALVE+PLASMA;
	CycleData[3][7].Time=2;
	CycleData[3][8].PartsSetting=VACUUMVALVE;
	CycleData[3][8].Time=5;
	CycleData[3][9].PartsSetting=VACUUMVALVE;
	CycleData[3][9].Time=30;
	CycleData[3][10].PartsSetting=VACUUMVALVE;
	CycleData[3][10].Time=30;
	CycleData[3][11].PartsSetting=VACUUMVALVE;
	CycleData[3][11].Time=60;
	CycleData[3][12].PartsSetting=VACUUMVALVE+PRESSURE2;
	CycleData[3][12].Time=30;
	CycleData[3][13].PartsSetting=VACUUMVALVE+PERIPUMP;
	CycleData[3][13].Time=60;

	CycleData[4][1].PartsSetting=INJECTIONVALVE;
	CycleData[4][1].Time=1;
	CycleData[4][2].PartsSetting=PRESSURE3;
	CycleData[4][2].Time=29;
	CycleData[4][3].PartsSetting=NONE;
	CycleData[4][3].Time=30;
	CycleData[4][4].PartsSetting=NONE;
	CycleData[4][4].Time=60;
	CycleData[4][5].PartsSetting=NONE;
	CycleData[4][5].Time=60;
	CycleData[4][6].PartsSetting=NONE;
	CycleData[4][6].Time=60;
	CycleData[4][7].PartsSetting=VENTVALVE+INJECTIONVALVE;
	CycleData[4][7].Time=3;
	CycleData[4][8].PartsSetting=VACUUMVALVE;
	CycleData[4][8].Time=30;
	CycleData[4][9].PartsSetting=NONE;
	CycleData[4][9].Time=47;
	CycleData[4][10].PartsSetting=NONE;
	CycleData[4][10].Time=60;
	CycleData[4][11].PartsSetting=NONE;
	CycleData[4][11].Time=58;
	CycleData[4][12].PartsSetting=VENTVALVE+INJECTIONVALVE;
	CycleData[4][12].Time=7;
	CycleData[4][13].PartsSetting=VACUUMVALVE;
	CycleData[4][13].Time=30;
	CycleData[4][14].PartsSetting=VENTVALVE;
	CycleData[4][14].Time=5;

	CycleData[5][1].PartsSetting=VACUUMVALVE+PLASMA;
	CycleData[5][1].Time=3;
	CycleData[5][2].PartsSetting=VACUUMVALVE;
	CycleData[5][2].Time=5;
	CycleData[5][3].PartsSetting=VACUUMVALVE+PLASMA;
	CycleData[5][3].Time=3;
	CycleData[5][4].PartsSetting=VACUUMVALVE;
	CycleData[5][4].Time=5;
	CycleData[5][5].PartsSetting=VACUUMVALVE+PLASMA+INJECTIONVALVE;
	CycleData[5][5].Time=2;
	CycleData[5][6].PartsSetting=VACUUMVALVE;
	CycleData[5][6].Time=5;
	CycleData[5][7].PartsSetting=VACUUMVALVE+PLASMA;
	CycleData[5][7].Time=2;
	CycleData[5][8].PartsSetting=VACUUMVALVE;
	CycleData[5][8].Time=5;
	CycleData[5][9].PartsSetting=VACUUMVALVE;
	CycleData[5][9].Time=30;
	CycleData[5][10].PartsSetting=VACUUMVALVE;
	CycleData[5][10].Time=30;
	CycleData[5][11].PartsSetting=VACUUMVALVE;
	CycleData[5][11].Time=60;
	CycleData[5][12].PartsSetting=VACUUMVALVE;
	CycleData[5][12].Time=30;
	CycleData[5][13].PartsSetting=VACUUMVALVE;
	CycleData[5][13].Time=60;

	CycleData[6][1].PartsSetting=VENTVALVE+INJECTIONVALVE;
	CycleData[6][1].Time=7;
	CycleData[6][2].PartsSetting=VACUUMVALVE;
	CycleData[6][2].Time=78;
	CycleData[6][3].PartsSetting=VENTVALVE+INJECTIONVALVE;
	CycleData[6][3].Time=7;
	CycleData[6][4].PartsSetting=VACUUMVALVE;
	CycleData[6][4].Time=78;
	CycleData[6][5].PartsSetting=VENTVALVE+INJECTIONVALVE;
	CycleData[6][5].Time=9;
	CycleData[6][6].PartsSetting=NONE;
	CycleData[6][6].Time=1;
}

void AdvancedCycle(){
	CycleData[1][1].PartsSetting=VACUUMVALVE;
	CycleData[1][1].Time=60;
	CycleData[1][2].PartsSetting=VENTVALVE+INJECTIONVALVE;
	CycleData[1][2].Time=7;
	CycleData[1][3].PartsSetting=VACUUMVALVE;
	CycleData[1][3].Time=30;
	CycleData[1][4].PartsSetting=VACUUMVALVE;
	CycleData[1][4].Time=30;
	CycleData[1][5].PartsSetting=VACUUMVALVE+PRESSURE1;
	CycleData[1][5].Time=30;
	CycleData[1][6].PartsSetting=VENTVALVE;
	CycleData[1][6].Time=5;
	CycleData[1][7].PartsSetting=NONE;
	CycleData[1][7].Time=48;
	CycleData[1][8].PartsSetting=NONE;
	CycleData[1][8].Time=60;
	CycleData[1][9].PartsSetting=NONE;
	CycleData[1][9].Time=60;
	CycleData[1][10].PartsSetting=NONE;
	CycleData[1][10].Time=60;
	CycleData[1][11].PartsSetting=NONE;
	CycleData[1][11].Time=60;
	CycleData[1][12].PartsSetting=VACUUMVALVE;
	CycleData[1][12].Time=60;
	CycleData[1][13].PartsSetting=VACUUMVALVE;
	CycleData[1][13].Time=60;
	CycleData[1][14].PartsSetting=VACUUMVALVE+PRESSURE2;
	CycleData[1][14].Time=30;
	CycleData[1][15].PartsSetting=VACUUMVALVE+PERIPUMP;
	CycleData[1][15].Time=60;

	CycleData[2][1].PartsSetting=INJECTIONVALVE;
	CycleData[2][1].Time=1;
	CycleData[2][2].PartsSetting=PRESSURE3;
	CycleData[2][2].Time=29;
	CycleData[2][3].PartsSetting=NONE;
	CycleData[2][3].Time=30;
	CycleData[2][4].PartsSetting=NONE;
	CycleData[2][4].Time=60;
	CycleData[2][5].PartsSetting=NONE;
	CycleData[2][5].Time=60;
	CycleData[2][6].PartsSetting=NONE;
	CycleData[2][6].Time=60;
	CycleData[2][7].PartsSetting=NONE;
	CycleData[2][7].Time=60;
	CycleData[2][8].PartsSetting=VENTVALVE+INJECTIONVALVE;
	CycleData[2][8].Time=3;
	CycleData[2][9].PartsSetting=VACUUMVALVE;
	CycleData[2][9].Time=30;
	CycleData[2][10].PartsSetting=NONE;
	CycleData[2][10].Time=45;
	CycleData[2][11].PartsSetting=NONE;
	CycleData[2][11].Time=60;
	CycleData[2][12].PartsSetting=NONE;
	CycleData[2][12].Time=60;
	CycleData[2][13].PartsSetting=NONE;
	CycleData[2][13].Time=60;
	CycleData[2][14].PartsSetting=NONE;
	CycleData[2][14].Time=60;
	CycleData[2][15].PartsSetting=VENTVALVE;
	CycleData[2][15].Time=7;
	CycleData[2][16].PartsSetting=VACUUMVALVE;
	CycleData[2][16].Time=30;
	CycleData[2][17].PartsSetting=VENTVALVE;
	CycleData[2][17].Time=5;

	CycleData[3][1].PartsSetting=VACUUMVALVE+PLASMA;
	CycleData[3][1].Time=3;
	CycleData[3][2].PartsSetting=VACUUMVALVE;
	CycleData[3][2].Time=5;
	CycleData[3][3].PartsSetting=VACUUMVALVE+PLASMA;
	CycleData[3][3].Time=3;
	CycleData[3][4].PartsSetting=VACUUMVALVE;
	CycleData[3][4].Time=5;
	CycleData[3][5].PartsSetting=VACUUMVALVE+PLASMA;
	CycleData[3][5].Time=2;
	CycleData[3][6].PartsSetting=VACUUMVALVE;
	CycleData[3][6].Time=5;
	CycleData[3][7].PartsSetting=VACUUMVALVE+PLASMA;
	CycleData[3][7].Time=2;
	CycleData[3][8].PartsSetting=VACUUMVALVE;
	CycleData[3][8].Time=5;
	CycleData[3][9].PartsSetting=VACUUMVALVE;
	CycleData[3][9].Time=30;
	CycleData[3][10].PartsSetting=VACUUMVALVE;
	CycleData[3][10].Time=30;
	CycleData[3][11].PartsSetting=VACUUMVALVE;
	CycleData[3][11].Time=60;
	CycleData[3][12].PartsSetting=VACUUMVALVE+PRESSURE2;
	CycleData[3][12].Time=30;
	CycleData[3][13].PartsSetting=VACUUMVALVE+PERIPUMP;
	CycleData[3][13].Time=60;

	CycleData[4][1].PartsSetting=INJECTIONVALVE;
	CycleData[4][1].Time=1;
	CycleData[4][2].PartsSetting=PRESSURE3;
	CycleData[4][2].Time=29;
	CycleData[4][3].PartsSetting=NONE;
	CycleData[4][3].Time=30;
	CycleData[4][4].PartsSetting=NONE;
	CycleData[4][4].Time=60;
	CycleData[4][5].PartsSetting=NONE;
	CycleData[4][5].Time=60;
	CycleData[4][6].PartsSetting=NONE;
	CycleData[4][6].Time=60;
	CycleData[4][7].PartsSetting=NONE;
	CycleData[4][7].Time=60;
	CycleData[4][8].PartsSetting=VENTVALVE+INJECTIONVALVE;
	CycleData[4][8].Time=3;
	CycleData[4][9].PartsSetting=VACUUMVALVE;
	CycleData[4][9].Time=30;
	CycleData[4][10].PartsSetting=NONE;
	CycleData[4][10].Time=45;
	CycleData[4][11].PartsSetting=NONE;
	CycleData[4][11].Time=60;
	CycleData[4][12].PartsSetting=NONE;
	CycleData[4][12].Time=60;
	CycleData[4][13].PartsSetting=NONE;
	CycleData[4][13].Time=60;
	CycleData[4][14].PartsSetting=NONE;
	CycleData[4][14].Time=60;
	CycleData[4][15].PartsSetting=VENTVALVE;
	CycleData[4][15].Time=7;
	CycleData[4][16].PartsSetting=VACUUMVALVE;
	CycleData[4][16].Time=30;
	CycleData[4][17].PartsSetting=VENTVALVE;
	CycleData[4][17].Time=5;

	CycleData[5][1].PartsSetting=VACUUMVALVE+PLASMA;
	CycleData[5][1].Time=3;
	CycleData[5][2].PartsSetting=VACUUMVALVE;
	CycleData[5][2].Time=5;
	CycleData[5][3].PartsSetting=VACUUMVALVE+PLASMA;
	CycleData[5][3].Time=3;
	CycleData[5][4].PartsSetting=VACUUMVALVE;
	CycleData[5][4].Time=5;
	CycleData[5][5].PartsSetting=VACUUMVALVE+PLASMA+INJECTIONVALVE;
	CycleData[5][5].Time=2;
	CycleData[5][6].PartsSetting=VACUUMVALVE;
	CycleData[5][6].Time=5;
	CycleData[5][7].PartsSetting=VACUUMVALVE+PLASMA;
	CycleData[5][7].Time=2;
	CycleData[5][8].PartsSetting=VACUUMVALVE;
	CycleData[5][8].Time=5;
	CycleData[5][9].PartsSetting=VACUUMVALVE;
	CycleData[5][9].Time=30;
	CycleData[5][10].PartsSetting=VACUUMVALVE;
	CycleData[5][10].Time=30;
	CycleData[5][11].PartsSetting=VACUUMVALVE;
	CycleData[5][11].Time=60;
	CycleData[5][12].PartsSetting=VACUUMVALVE;
	CycleData[5][12].Time=30;
	CycleData[5][13].PartsSetting=VACUUMVALVE;
	CycleData[5][13].Time=60;

	CycleData[6][1].PartsSetting=VENTVALVE+INJECTIONVALVE;
	CycleData[6][1].Time=7;
	CycleData[6][2].PartsSetting=VACUUMVALVE;
	CycleData[6][2].Time=70;
	CycleData[6][3].PartsSetting=VENTVALVE+INJECTIONVALVE;
	CycleData[6][3].Time=7;
	CycleData[6][4].PartsSetting=VACUUMVALVE;
	CycleData[6][4].Time=70;
	CycleData[6][5].PartsSetting=VENTVALVE+INJECTIONVALVE;
	CycleData[6][5].Time=7;
	CycleData[6][6].PartsSetting=VACUUMVALVE;
	CycleData[6][6].Time=70;
	CycleData[6][7].PartsSetting=VENTVALVE;
	CycleData[6][7].Time=8;
	CycleData[6][8].PartsSetting=NONE;
	CycleData[6][8].Time=1;
}

bool is_same(unsigned char a[], unsigned char b[], int length){
    for (int i = 0; i < length; i++) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}

void rfid_callback() {
    // Set production variables (you should read these values from the RFID)
    unsigned char currentoday_date[] = {RFIDData.production_year, RFIDData.production_month, RFIDData.production_day, RFIDData.production_number};

    bool found = false;
    for (int i = 0; i < 3; i++) {
        unsigned char flash_data[] = {flash_sterilant_production_year[i], flash_sterilant_production_month[i], flash_sterilant_production_day[i], flash_sterilant_production_number[i]};

        if (is_same(currentoday_date, flash_data, 4)) {
        	RFIDData.open_year = flash_sterilant_open_year[i];
        	RFIDData.open_month = flash_sterilant_open_month[i];
        	RFIDData.open_day = flash_sterilant_open_day[i];
        	RFIDData.volume = flash_sterilant_volume[i];
        	RFIDData.currentID = i;
            found = true;
            break;
        }
    }

    if (!found) {

    	shift_flash_data();
    	RFIDData.currentID = 2;
        // Copy the production data to the newest flash index
        flash_sterilant_production_year[2] = RFIDData.production_year;
        flash_sterilant_production_month[2] = RFIDData.production_month;
        flash_sterilant_production_day[2] = RFIDData.production_day;
        flash_sterilant_production_number[2] = RFIDData.production_number;

        // Update the newest data
        GetTime();
        flash_sterilant_open_year[2] = bcd2bin(today_date.year);
        flash_sterilant_open_month[2] = bcd2bin(today_date.month);
        flash_sterilant_open_day[2] = bcd2bin(today_date.day);
        flash_sterilant_volume[2] = 50;


    }
}

void shift_flash_data() {
    // Shift flash data
    for (int i = 0; i < 2; i++) {
        flash_sterilant_production_year[i] = flash_sterilant_production_year[i + 1];
        flash_sterilant_production_month[i] = flash_sterilant_production_month[i + 1];
        flash_sterilant_production_day[i] = flash_sterilant_production_day[i + 1];
        flash_sterilant_production_number[i] = flash_sterilant_production_number[i + 1];
        flash_sterilant_open_year[i] = flash_sterilant_open_year[i + 1];
        flash_sterilant_open_month[i] = flash_sterilant_open_month[i + 1];
        flash_sterilant_open_day[i] = flash_sterilant_open_day[i + 1];
        flash_sterilant_volume[i] = flash_sterilant_volume[i + 1];
    }
}
HAL_StatusTypeDef Flash_Write_Int(uint32_t address, uint32_t value)
{
    HAL_StatusTypeDef status;
    // 플래시 잠금 해제
    HAL_FLASH_Unlock();

    // 필요하다면 페이지 지우기
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t PAGEError = 0;
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
    EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
    EraseInitStruct.Sector = FLASH_SECTOR_11; // 마지막 섹터 번호, MCU에 따라 다름
    EraseInitStruct.NbSectors = 1;
    status = HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError);
    if (status != HAL_OK) return status;

    // 데이터 쓰기
    status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, value);

    // 플래시 잠금
    HAL_FLASH_Lock();

    return status;
}

uint32_t Flash_Read_Int(uint32_t address)
{
    return *(uint32_t*)address;
}


union fc {
	float fValue;
	char  cValue[4];
};

float char2float(const unsigned char *data)
{
	union fc	temp;
	for(int i = 0; i < 4; i++) {
		temp.cValue[i] = data[i];
	}
	return(temp.fValue);
}

void float2char(float fValue, char *data)
{
	union fc	temp;

	temp.fValue = fValue;
	for(int i = 0; i < 4; i++) {
		data[i] = temp.cValue[i];
	}
}
