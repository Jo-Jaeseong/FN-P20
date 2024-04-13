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
#include "peripheral.h"
#include "sensor.h"



__attribute__((__section__(".user_data"))) const char userConfig[1024];

uint32_t startTick, endTick, duration;
/*
startTick = HAL_GetTick(); // 기능 실행 전 타임스탬프 캡처
// 여기에 측정하고자 하는 기능 혹은 함수 호출
endTick = HAL_GetTick(); // 기능 실행 후 타임스탬프 캡처
duration = endTick - startTick; // 실행 시간(밀리초 단위)
*/

#define USER_DATA_FLASH_ADDRESS      0x08040000

char userdata[1024];

//-----------------------------------------------------------------------------------------------------------------------//
/*장비 정보*///(70)
unsigned char flash_MODEL_NAME[10];
unsigned char flash_SERIAL_NUMBER[10];
unsigned char flash_DEPARTMENT_NAME[10];
unsigned char flash_FACILITY_NAME[10];
unsigned char flash_HARDWARE_VERSION[10];
unsigned char flash_SOFTWARE_VERSION[10];
unsigned char flash_LANGUAGE[10];

#define MODEL_NAME					0//char[10](문자입력)
#define SERIAL_NUMBER				10//char[10](문자입력)
#define DEPARTMENT_NAME				20//char[10](문자입력)
#define FACILITY_NAME				30//char[10](문자입력)
#define HARDWARE_VERSION			40//char[10](숫자입력)
#define SOFTWARE_VERSION			50//char[10](숫자입력)
#define FLASHLANGUAGE				60//char[10](숫자입력)

//-----------------------------------------------------------------------------------------------------------------------//
/*계정 정보*///(51)
unsigned char flash_ID[5][10];
unsigned char flash_PW[5][10];
unsigned char flashuserCount;

#define	FLASHID						70//char[5][10](숫자입력)70~119
#define	FLASHPW						120//char[5][10](숫자입력)120~169
#define	FLASHUSERCOUNT				170//char[1](숫자입력)

//-----------------------------------------------------------------------------------------------------------------------//
/*세팅*///(39)
//기능 플래그 데이터
#define ALARMCHECKFLAG_DATA			175//[1]
#define ERRORCHECKFLAG_DATA			176//[1]
#define RESERVATIONFLAG_DATA		177//[1]
#define AUTOPRINTFLAG_DATA			178//[1]
#define PRINTCOPY_DATA				179//[1]
#define PRINTGRAPHFLAG_DATA			180//[1]

//온도 세팅
#define DOORSETTINGTEMP_DATA		181//[3]181~3
#define CHAMBERSETTINGTEMP_DATA		184//[3]4~6
#define CHAMBERBACKSETTINGTEMP_DATA	187//[3]7~9
#define VAPORIZERSETTINGTEMP_DATA	190//[3]190~192

//진공 조건 세팅
#define PRESSURECONDITION_DATA		193//[3]13~15

//페리 속도 세팅
#define PERISPEED_DATA				196//[3]16~18

//캘리브레이션
#define CALIBRATIONTEMP_DATA		199//[4]19~22
#define CALIBRATIONVACUUM_DATA		203//[1]203
#define VACUUMSLOPE_DATA			204//[2]24~25	//소수점 단위 환산 필요(소수점 3쨰자리)
#define VACUUMINTERCEPT_DATA		206//[2]26~27	//소수점 단위 환산 필요(정수 3자리+소수점 1자리)

//셀프 테스트 오차
#define TESTVACUUMVALUE_DATA		208//[1]
#define TESTLEAKVALUE_DATA			209//[1]
#define TESTTEMPERRORVALUE_DATA		210//[1]

//과수 기한 설정
#define STERILANTCHECKDAY_DATA		211//[1]

//도어 오픈 압력 설정
#define DOOROPENPRESSURE_DATA		212//[2] 3자리 정수


//-----------------------------------------------------------------------------------------------------------------------//
/*PM 정보*///(22)
//사용 횟수 카운트
#define	TOTALCOUNT_DATA				214//int[2](숫자입력)	//4자리 환산 필요
#define	DAILYCOUNT_DATA				216//int[1](숫자입력)
#define	BEFOREDAY_DATA				217//int[1](숫자입력)//beforeday

//추후 변경 이름 _DATA로 통일
#define PRODUCTION_YEAR_DATA		220//int[1]
#define PRODUCTION_MONTH_DATA		221//int[1]
#define PRODUCTION_DAY				222//int[1]
#define PRODUCTION_NUMBER			223//int[1]
#define OPEN_YEAR					224//int[1]
#define OPEN_MONTH					225//int[1]
#define OPEN_DAY					226//int[1]
#define EXPIRY_YEAR					227//int[1]
#define EXPIRY_MONTH				228//int[1]
#define EXPIRY_DAY					229//int[1]
#define VOLUME						230//int[1]
#define VOLUMEMAX					231//int[1]

#define	CARBONFILTERMAX 			239//char[2](숫자입력)
#define	HEPAFILTERMAX				241//char[2](숫자입력)
#define	PLASMAASSYMAX				243//char[2](숫자입력)
#define	CARBONFILTER				245//char[2](숫자입력)
#define	HEPAFILTER					247//char[2](숫자입력)
#define	PLASMAASSY					249//char[2](숫자입력)249~250
//-----------------------------------------------------------------------------------------------------------------------//
/*공정*///(720)
#define CYCLEDATA1					280	//[240]
#define CYCLEDATA2					520	//[240]
#define CYCLEDATA3					760	//[240]

#define	NONE				0x00
#define	VACUUMVALVE			0x01
#define	VENTVALVE			0x02
#define	INJECTIONVALVE		0x04
#define	PERIPUMP			0x08
#define	PLASMA				0x10
#define	PRESSURE1			0x20
#define	PRESSURE2			0x40
#define	PRESSURE3			0x60
//-----------------------------------------------------------------------------------------------------------------------//


void Write_Flash(){
	startTick = HAL_GetTick();
	DisplayPage(LCD_LOADING_PAGE);
	unsigned char ucData[1024]={};

	/*장비 정보*///(70)
	for(int i=0;i<10;i++){
		ucData[MODEL_NAME+i]=flash_MODEL_NAME[i];
		ucData[SERIAL_NUMBER+i]=flash_SERIAL_NUMBER[i];
		ucData[DEPARTMENT_NAME+i]=flash_DEPARTMENT_NAME[i];
		ucData[FACILITY_NAME+i]=flash_FACILITY_NAME[i];
		ucData[HARDWARE_VERSION+i]=flash_HARDWARE_VERSION[i];
		ucData[SOFTWARE_VERSION+i]=flash_SOFTWARE_VERSION[i];
		ucData[FLASHLANGUAGE+i]=flash_LANGUAGE[i];
	}

	/*계정 정보*///(51)
	for(int j=0;j<5;j++){
		for(int i=0;i<10;i++){
			ucData[FLASHID+i+10*j]=flash_ID[j][i];
			ucData[FLASHPW+i+10*j]=flash_PW[j][i];
		}
	}
	ucData[FLASHUSERCOUNT]=flashuserCount;

	/*세팅*///(39)
	//기능 플래그 저장
	ucData[ALARMCHECKFLAG_DATA]=AlarmCheckFlag;
	ucData[ERRORCHECKFLAG_DATA]=ErrorCheckFlag;
	ucData[RESERVATIONFLAG_DATA]=reservationFlag;
	ucData[AUTOPRINTFLAG_DATA]=autoprintFlag;
	ucData[PRINTCOPY_DATA]=printcopy;
	ucData[PRINTGRAPHFLAG_DATA]=printgraphFlag;

	//온도세팅 저장
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

	//진공조건 저장
	ucData[PRESSURECONDITION_DATA]=PreesureCondition[0];
	ucData[PRESSURECONDITION_DATA+1]=PreesureCondition[1];
	ucData[PRESSURECONDITION_DATA+2]=PreesureCondition[2];

	//페리 스피드 저장
	ucData[PERISPEED_DATA]=perispeed[0];
	ucData[PERISPEED_DATA+1]=perispeed[1];
	ucData[PERISPEED_DATA+2]=perispeed[2];

	//캘리브레이션 데이터 저장
	ucData[CALIBRATIONTEMP_DATA]=CalibrationTemp[0];
	ucData[CALIBRATIONTEMP_DATA+1]=CalibrationTemp[1];
	ucData[CALIBRATIONTEMP_DATA+2]=CalibrationTemp[2];
	ucData[CALIBRATIONTEMP_DATA+3]=CalibrationTemp[3];
	ucData[CALIBRATIONVACUUM_DATA]=CalibrationVacuum;

	ucData[VACUUMSLOPE_DATA]=(int)(vacuumsplope*1000)/100;
	ucData[VACUUMSLOPE_DATA+1]=(int)(vacuumsplope*1000)%100;
	ucData[VACUUMINTERCEPT_DATA]=(int)(-(vacuumintercept*10)/100);
	ucData[VACUUMINTERCEPT_DATA+1]=(-((int)(vacuumintercept*10))%100);

	//셀프 테스트 벨류 및 오차 저장
	ucData[TESTVACUUMVALUE_DATA]=TestVacuumValue;
	ucData[TESTLEAKVALUE_DATA]=TestLeakValue;
	ucData[TESTTEMPERRORVALUE_DATA]=TestTempErrorValue;

	//과수 기한 설정
	ucData[STERILANTCHECKDAY_DATA]=SterilantCheckDay;
	ucData[DOOROPENPRESSURE_DATA]=(int)(DoorOpenPressure/100);

	//도어 오픈 압력 설정
	ucData[DOOROPENPRESSURE_DATA+1]=(int)(DoorOpenPressure%100);


	/*PM 정보*///(22)
	//사용 횟수 카운트
	ucData[TOTALCOUNT_DATA]=(int)(totalCount/100);
	ucData[TOTALCOUNT_DATA+1]=(int)(totalCount%100);
	ucData[DAILYCOUNT_DATA]=dailyCount;
	ucData[BEFOREDAY_DATA]=beforeday;

	//멸균제 데이터 저장
	ucData[PRODUCTION_YEAR_DATA]=RFIDData.production_year;
	ucData[PRODUCTION_MONTH_DATA]=RFIDData.production_month;
	ucData[PRODUCTION_DAY]=RFIDData.production_day;
	ucData[PRODUCTION_NUMBER]=RFIDData.production_number;
	ucData[OPEN_YEAR]=RFIDData.open_year;
	ucData[OPEN_MONTH]=RFIDData.open_month;
	ucData[OPEN_DAY]=RFIDData.open_day;

	ucData[EXPIRY_YEAR]=RFIDData.expiry_year;
	ucData[EXPIRY_MONTH]=RFIDData.expiry_month;
	ucData[EXPIRY_DAY]=RFIDData.expiry_day;

	ucData[VOLUME]=RFIDData.volume;
	ucData[VOLUMEMAX]=RFIDData.volumemax;

	//필터, 플라즈마 데이터 저장
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


	/*공정*///(720)
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

	/*장비 정보*///(70)
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

	/*계정 정보*///(51)
	for(int j=0;j<5;j++){
		for(int i=0;i<10;i++){
			flash_ID[j][i]=userdata[FLASHID+i+10*j];
			flash_PW[j][i]=userdata[FLASHPW+i+10*j];
		}
	}
	if(flash_ID[0][0]==0){
		sprintf(flash_ID[0],"USER1");
		sprintf(flash_PW[0],"1234");
	}
	flashuserCount=userdata[FLASHUSERCOUNT];
	if(flashuserCount<=0){
		flashuserCount=1;
	}

	/*세팅*///(39)
	//기능 플래그 저장
	AlarmCheckFlag=userdata[ALARMCHECKFLAG_DATA];
	if(AlarmCheckFlag==0){
		AlarmCheckFlag=1;
	}
	ErrorCheckFlag=userdata[ERRORCHECKFLAG_DATA];
	if(ErrorCheckFlag==0){
		ErrorCheckFlag=1;
	}
	reservationFlag=userdata[RESERVATIONFLAG_DATA];
	if(reservationFlag==0){
		reservationFlag=1;
	}
	autoprintFlag=userdata[AUTOPRINTFLAG_DATA];
	if(autoprintFlag==0){
		autoprintFlag=1;
	}
	printcopy=userdata[PRINTCOPY_DATA];
	if(printcopy==0){
		printcopy=1;
	}
	printgraphFlag=userdata[PRINTGRAPHFLAG_DATA];
	if(printgraphFlag==0){
		printgraphFlag=1;
	}

	//온도세팅 저장
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
		VaporizerSettingTemp[1]=135;
	}

	//진공조건 저장
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

	//페리 스피드 저장
	perispeed[0]=userdata[PERISPEED_DATA];
	perispeed[1]=userdata[PERISPEED_DATA+1];
	perispeed[2]=userdata[PERISPEED_DATA+2];
	if(perispeed[0]==0){
		perispeed[0]=10;
	}

	//캘리브레이션 데이터 저장
	CalibrationTemp[0]=userdata[CALIBRATIONTEMP_DATA];
	CalibrationTemp[1]=userdata[CALIBRATIONTEMP_DATA+1];
	CalibrationTemp[2]=userdata[CALIBRATIONTEMP_DATA+2];
	CalibrationTemp[3]=userdata[CALIBRATIONTEMP_DATA+3];
	for(int i=0;i<4;i++){
		if(CalibrationTemp[i]==0){
			CalibrationTemp[i]=20;
		}
	}
	CalibrationVacuum=userdata[CALIBRATIONVACUUM_DATA];
	if(CalibrationVacuum==0){
		CalibrationVacuum=20;
	}
	vacuumsplope=(float)((userdata[VACUUMSLOPE_DATA]*100)+(userdata[VACUUMSLOPE_DATA+1]))/1000;
	if(vacuumsplope==0){
		vacuumsplope=0.265;
	}
	vacuumintercept=(-((float)(userdata[VACUUMINTERCEPT_DATA]*100)+(float)(userdata[VACUUMINTERCEPT_DATA+1]))/10);
	if(vacuumintercept==0){
		vacuumintercept=-216.1;
	}

	//셀프 테스트 벨류 및 오차 저장
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

	//과수 기한 설정
	SterilantCheckDay=userdata[STERILANTCHECKDAY_DATA];
	if(SterilantCheckDay==0){
		SterilantCheckDay=60;
	}

	//도어 오픈 압력 설정
	DoorOpenPressure=(userdata[DOOROPENPRESSURE_DATA]*100)+(userdata[DOOROPENPRESSURE_DATA+1]);
	if(DoorOpenPressure==0){
		DoorOpenPressure=720;
	}


	/*PM 정보*///(22)
	//사용 횟수 카운트
	totalCount=(userdata[TOTALCOUNT_DATA]*100)+(userdata[TOTALCOUNT_DATA+1]);
	dailyCount=userdata[DAILYCOUNT_DATA];
	beforeday=userdata[BEFOREDAY_DATA];

	//멸균제 데이터 저장
	RFIDData.production_year=userdata[PRODUCTION_YEAR_DATA];
	RFIDData.production_month=userdata[PRODUCTION_MONTH_DATA];
	RFIDData.production_day=userdata[PRODUCTION_DAY];
	RFIDData.production_number=userdata[PRODUCTION_NUMBER];

	RFIDData.open_year=userdata[OPEN_YEAR];
	RFIDData.open_month=userdata[OPEN_MONTH];
	RFIDData.open_day=userdata[OPEN_DAY];

	RFIDData.expiry_year=userdata[EXPIRY_YEAR];
	RFIDData.expiry_month=userdata[EXPIRY_MONTH];
	RFIDData.expiry_day=userdata[EXPIRY_DAY];

	RFIDData.volume=userdata[VOLUME];
	RFIDData.volumemax=userdata[VOLUMEMAX];

	//필터, 플라즈마 데이터 저장
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

	/*공정*///(720)
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

	CycleData[3][1].PartsSetting=VACUUMVALVE;
	CycleData[3][1].Time=1;
	CycleData[3][2].PartsSetting=VACUUMVALVE;
	CycleData[3][2].Time=1;
	CycleData[3][3].PartsSetting=VACUUMVALVE;
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

	CycleData[5][1].PartsSetting=VACUUMVALVE;
	CycleData[5][1].Time=1;
	CycleData[5][2].PartsSetting=VACUUMVALVE;
	CycleData[5][2].Time=1;
	CycleData[5][3].PartsSetting=VACUUMVALVE;
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
	CycleData[1][3].PartsSetting=VENTVALVE+INJECTIONVALVE;
	CycleData[1][3].Time=3;
	CycleData[1][4].PartsSetting=NONE;
	CycleData[1][4].Time=27;
	CycleData[1][5].PartsSetting=NONE;
	CycleData[1][5].Time=30;
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

	CycleData[3][1].PartsSetting=VACUUMVALVE;
	CycleData[3][1].Time=5;
	CycleData[3][2].PartsSetting=VACUUMVALVE;
	CycleData[3][2].Time=10;
	CycleData[3][3].PartsSetting=VACUUMVALVE;
	CycleData[3][3].Time=5;
	CycleData[3][4].PartsSetting=VACUUMVALVE;
	CycleData[3][4].Time=10;
	CycleData[3][5].PartsSetting=VACUUMVALVE;
	CycleData[3][5].Time=30;
	CycleData[3][6].PartsSetting=VACUUMVALVE;
	CycleData[3][6].Time=30;
	CycleData[3][7].PartsSetting=VACUUMVALVE;
	CycleData[3][7].Time=30;
	CycleData[3][8].PartsSetting=VACUUMVALVE;
	CycleData[3][8].Time=30;
	CycleData[3][9].PartsSetting=VACUUMVALVE;
	CycleData[3][9].Time=30;
	CycleData[3][10].PartsSetting=VACUUMVALVE+PERIPUMP+PRESSURE2;
	CycleData[3][10].Time=60;


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


	CycleData[5][1].PartsSetting=VACUUMVALVE+INJECTIONVALVE;
	CycleData[5][1].Time=1;
	CycleData[5][2].PartsSetting=VACUUMVALVE;
	CycleData[5][2].Time=14;
	CycleData[5][3].PartsSetting=VACUUMVALVE;
	CycleData[5][3].Time=5;
	CycleData[5][4].PartsSetting=VACUUMVALVE;
	CycleData[5][4].Time=10;
	CycleData[5][5].PartsSetting=VACUUMVALVE+INJECTIONVALVE;
	CycleData[5][5].Time=1;
	CycleData[5][6].PartsSetting=VACUUMVALVE;
	CycleData[5][6].Time=29;
	CycleData[5][7].PartsSetting=VACUUMVALVE;
	CycleData[5][7].Time=30;
	CycleData[5][8].PartsSetting=VACUUMVALVE;
	CycleData[5][8].Time=30;
	CycleData[5][9].PartsSetting=VACUUMVALVE;
	CycleData[5][9].Time=30;
	CycleData[5][10].PartsSetting=VACUUMVALVE;
	CycleData[5][10].Time=30;


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

	CycleData[3][1].PartsSetting=VACUUMVALVE;
	CycleData[3][1].Time=3;
	CycleData[3][2].PartsSetting=VACUUMVALVE;
	CycleData[3][2].Time=5;
	CycleData[3][3].PartsSetting=VACUUMVALVE;
	CycleData[3][3].Time=3;
	CycleData[3][4].PartsSetting=VACUUMVALVE;
	CycleData[3][4].Time=5;
	CycleData[3][5].PartsSetting=VACUUMVALVE;
	CycleData[3][5].Time=2;
	CycleData[3][6].PartsSetting=VACUUMVALVE;
	CycleData[3][6].Time=5;
	CycleData[3][7].PartsSetting=VACUUMVALVE;
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

	CycleData[5][1].PartsSetting=VACUUMVALVE;
	CycleData[5][1].Time=3;
	CycleData[5][2].PartsSetting=VACUUMVALVE;
	CycleData[5][2].Time=5;
	CycleData[5][3].PartsSetting=VACUUMVALVE;
	CycleData[5][3].Time=3;
	CycleData[5][4].PartsSetting=VACUUMVALVE;
	CycleData[5][4].Time=5;
	CycleData[5][5].PartsSetting=VACUUMVALVE+INJECTIONVALVE;
	CycleData[5][5].Time=2;
	CycleData[5][6].PartsSetting=VACUUMVALVE;
	CycleData[5][6].Time=5;
	CycleData[5][7].PartsSetting=VACUUMVALVE;
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

	CycleData[3][1].PartsSetting=VACUUMVALVE;
	CycleData[3][1].Time=3;
	CycleData[3][2].PartsSetting=VACUUMVALVE;
	CycleData[3][2].Time=5;
	CycleData[3][3].PartsSetting=VACUUMVALVE;
	CycleData[3][3].Time=3;
	CycleData[3][4].PartsSetting=VACUUMVALVE;
	CycleData[3][4].Time=5;
	CycleData[3][5].PartsSetting=VACUUMVALVE;
	CycleData[3][5].Time=2;
	CycleData[3][6].PartsSetting=VACUUMVALVE;
	CycleData[3][6].Time=5;
	CycleData[3][7].PartsSetting=VACUUMVALVE;
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

	CycleData[5][1].PartsSetting=VACUUMVALVE;
	CycleData[5][1].Time=3;
	CycleData[5][2].PartsSetting=VACUUMVALVE;
	CycleData[5][2].Time=5;
	CycleData[5][3].PartsSetting=VACUUMVALVE;
	CycleData[5][3].Time=3;
	CycleData[5][4].PartsSetting=VACUUMVALVE;
	CycleData[5][4].Time=5;
	CycleData[5][5].PartsSetting=VACUUMVALVE+INJECTIONVALVE;
	CycleData[5][5].Time=2;
	CycleData[5][6].PartsSetting=VACUUMVALVE;
	CycleData[5][6].Time=5;
	CycleData[5][7].PartsSetting=VACUUMVALVE;
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
