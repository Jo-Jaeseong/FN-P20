/*
 * Process.c
 *
 *  Created on: Aug 10, 2022
 *      Author: CBT
 */

#include "main.h"
#include "stdbool.h"
#include "string.h"

#include "hardware.h"
#include "sensor.h"
#include "peripheral.h"

#define NORMAL		1
#define FACTORYTEST	0


int Select_NORMAL_MODE=1;
int LiquidFlag=0;
int TestMode=0;
int TestCompleteFlag=0;
int TestProcess=0;

float TestPressure[10]={};
int Pressuererror[2];
int Valveerror[3];

int devicealram[14]={};

int StopFlag=0;

int leaktesttime=60;
/*
알람1	장비 전원 정전	장비 전원 차단 됨
알람2	멸균제 사용 기간 지남	장비내 멸균제 삽입 후 60일 지남
알람3	멸균제 제조 만기일 지남	멸균제 제조 후 1년 지남
알람4	멸균제 S/N 오류	멸균제 S/N가 맞지 안음
알람5	챔버 도어 열림	도어가 열림, 스위치 동작 안됨
알람6	멸균제 도어 열림	도어 열림, 스위치 동작 안됨
알람7	멸균제 병 감지 안됨	멸균제 병 없거나, 스위치 동작 안됨
알람8	도어 히터 온도 확인	도어 히터 및 히터 전선, TC 및 TC선 확인
알람9	챔버 히터 온도 확인	챔버 히터 및 히터 전선, TC 및 TC선 확인
알람10	챔버 후면 히터 온도 확인	챔버 후면 히터 및 히터 전선, TC 및 TC선 확인
알람11	기화기 히터 온도 확인	기화기 히터 및 히터 전선, TC 및 TC선 확인
알람12	PM 촉매제 필터 교체	촉매제 설정값 0임, 교체 후 재설정
알람13	PM 헤파필터 교체	헤파필터 설정값 0임, 교체 후 재설정
알람14	PM 플라즈마 전극 교체	플라즈마 전극 설정값 0임, 교체 후 재설정
 */



unsigned char deviceerror[11]={};
int errorcode=0;

/*
에러1	공정 취소	공정 중 취소 버튼 누름
에러2	챔버 온도 도달 안됨	챔버 설정 온도 도달 안됨
에러3	압력1 도달 안됨	압력1 설정 시간내 도달 안됨
에러4	압력2 도달 안됨	압력2 설정 시간내 도달 안됨
에러5	기화기 히터 온도 확인	기화기 온도 설정 시간내 도달 안됨 (V3P2까지 온도 확인)
에러6	멸균제1 감지 안됨	멸균제1 부족 및 공급 불량
에러7	멸균제 주입 후 압력 도달 안됨	멸균제 주입 불량 및 압력 센서 문제
에러8	압력3 도달 안됨	압력3 설정 시간내 도달 안됨
에러9	멸균제2 감지 안됨	멸균제2 부족 및 공급 불량
에러10	멸균제 주입 후 압력 도달 안됨	멸균제 주입 불량 및 압력 센서 문제
 */

int TestVacuumValue=20;
int TestLeakValue=5;


int Solenoid_Flag=0;
int LED1=0;
int printmode=0;

float PressureData[300];
float TemperatureData[300];



unsigned int TensecondCounter=0;
unsigned int DataCounter=0;
unsigned char ProcessFlag=0 ,CurrentProcess=0, CurrentStep=0, ProcessStep=0;

unsigned int TotalTime;
unsigned int fProcessTime[7];
unsigned int CycleTime=0;
unsigned int EndTime=0;
unsigned int ProcessTime[7]={};
unsigned int StepTime[21]={};

int Vaccum_Check=0;

struct Process_data_format	CycleData[7][21];

struct data_format p_data;
struct data_format load_data;


int Vacuum_Check_Count=0;

int HeaterControlMode=1;

#define SHORT		1
#define STANDARD	2
#define ADVANCED	3

int CycleName=SHORT;
int ProcessNum,StepNum;


void loop(){

	if(UART_Receive_Flag) {
		UART_Receive_Flag = 0;
		LCD_Process();
	}
	if(Timer_DeliSecond_Flag) {
	  Timer_DeliSecond_Flag=0;
	  DeliSecondProcess();
	}
	if(Timer_Half_1s_Flag){
	  Timer_Half_1s_Flag=0;
	  HalfSecondProcess();
	}
	if(Timer_1s_Flag) {
	  Timer_1s_Flag=0;
	  OneSecondProcess();
	}
	if(EndTimer_Flag) {
		ProcessEndTimer();
	}
}

void DeliSecondProcess(void){
	GetValue();
	//printf("max:4095, min:3500, data1:%d, data2:%d, data3:%d  \n",data1/10, Pressure, Pressure2);
}
int tempcnt=0;

void HalfSecondProcess(void){
	//온도 측정
	Check_Temp(tempcnt);
	if(tempcnt>=3){
		tempcnt=0;
	}
	else{
		tempcnt++;
	}

	//압력 보정
	ValueFilter();
	DisplayVacuumSensor();

	if(Running_Flag==0){
		DisplayIcons();

		//도어 오픈
		if(DoorOpenFlag==1){
			if(DoorLatchCheck()){
				DoorLatch(1);
				DoorOpenFlag=0;
			}
			else{
				DoorLatch(0);
				DoorOpenFlag=0;
			}

	    }
		else if(DoorOpenFlag==0){
			DoorLatch(0);
			DoorOpenFlag=0;
		}
		DoorOpenFlag=DoorOpenProcess();
		if(DoorOpenVentFlag==1){
			if(DoorOpenVentCnt>=0){
				DoorOpenVentCnt--;
				VentValve(1);
			}
			else{
				DoorOpenVentCnt=0;
				DoorOpenVentFlag=0;
				VentValve(0);
				DoorOpenFlag=1;
	        	DisplayPage(beforepage);
			}
		}
	}

	if(LevelSensor2Check()){
		LiquidFlag=2;
		TurnOffPeristalticPump();
	}

	if(Select_NORMAL_MODE){
		DisplayNormalValues();
	}
	else{
		if(Running_Flag){
			DisplayProcessTestValues();
		}
	}
	if(TestMode==1||TestMode==9){
		DisplayTime(0x25,0x50,EndTestTimeCounter);
	}
	if(Running_Flag){
		ReadLCD();
	}
}


void OneSecondProcess(void){
	if(Running_Flag){
		TensecondCounter++;
		deviceerror[2]=ChamberHeaterCheck();
		GetData();
		if(TensecondCounter==10){
			DataCounter++;
			PressureData[DataCounter]=Pressure;
			TemperatureData[DataCounter]=Temperature[1];
			TensecondCounter=0;
        	//DisplayInitTempGraph();
        	//DisplayInitVacuumGraph();
			DisplayTempGraph(DataCounter-1,0);
			DisplayVacuumGraph(DataCounter-1,1);
		}
	}

	//스탠바이 페이지에서 스타트 버튼 활성화
	if(currentpage==LCD_STANDBY_OK_PAGE){
		if(DoorLatchCheck()==0){
			DisplayPage(LCD_STANDBY_PAGE);
		}
	}
	else if(currentpage==LCD_STANDBY_PAGE){
		if(DoorLatchCheck()==1){
			DisplayPage(LCD_STANDBY_OK_PAGE);
		}
	}
	if(currentpage==LCD_EORROR_WAIT_PAGE){
		if(CurrentProcess==5){
			DisplayTime(0x14,0x01,ProcessTime[5]+ProcessTime[6]);
		}
		else if(CurrentProcess==6){
			DisplayTime(0x14,0x01,ProcessTime[6]);
		}
	}



	//Read_Vacuumsensor();
	if(TestMode==1){
		if(TestProcess==2){
			DisplayPageValue(0x25,0x10,Pressure*10);
		}
		else if(TestProcess==3){
			DisplayPageValue(0x25,0x14,Pressure*10);
		}
		else if(TestProcess==4){
			DisplayPageValue(0x25,0x18,Pressure*10);
		}
		else if(TestProcess==5){
			DisplayPageValue(0x25,0x1C,Pressure*10);
		}
		else if(TestProcess==6){
			DisplayPageValue(0x25,0x20,Pressure*10);
		}

		//
		else if(TestProcess==7){
			DisplayPageValue(0x25,0x24,Pressure*10);
		}
		else if(TestProcess==8){
			DisplayPageValue(0x25,0x28,Pressure*10);
		}
		else if(TestProcess==9){
			DisplayPageValue(0x25,0x2C,Pressure*10);
		}
		else if(TestProcess==10){
			DisplayPageValue(0x25,0x30,Pressure*10);
		}
		else if(TestProcess==11){
			DisplayPageValue(0x25,0x34,Pressure*10);
		}


	}
	if(HeaterControlMode==1){
		HeaterControl();
	}
	else if(HeaterControlMode==2){
		HeaterControl();
	}
}

void Init_Device(){
	//hardware reset
	Inithardware();
	InitADC();
    Read_Flash();
    //Read_Setting_Data_Flash();
	InitLCD();
	RFIDCheck();
}

void Inithardware(){
	DoorHeater(0);
	ChamberHeater(0);
	ChamberBackHeater(0);
	VaporizerHeater(0);
	VacuumPump(0);
	Plasma(0);

	DoorLatch(0);
	VacuumValve(0);
	VentValve(0);
	InjectionValve(0);
	CirculationHeater(0);

	CirculationValve(0);
	InhalationValve(0);
	Fan(0);
	PeristalticSpeed();
	TurnOffPeristalticPump();

	HeaterControlMode=1;
}


/*
 * Process
 */

void Start(){
	//Data save-related variables
	memset(PressureData, 0, sizeof(PressureData));
	memset(TemperatureData, 0, sizeof(TemperatureData));
	DataCounter=0;
	TensecondCounter=0;
	PressureData[0]=Pressure;
	TemperatureData[0]=Temperature[1];

	Select_NORMAL_MODE=1;
	Running_Flag=1;

	HeaterControlMode=2;

	CurrentProcess=1;
	CurrentStep=1;
	EndTimer_Flag=1;
	Vacuum_Check_Count=0;
	StopFlag=0;
	TotalTime=0;
	LiquidFlag=0;
    memset(deviceerror, 0, sizeof(deviceerror));

	VacuumPump(1);
	Fan(1);
	VacuumValve(0);
	VentValve(0);
	InjectionValve(0);
	Plasma(0);
	PeriPump(0);
	p_data.cyclename=CycleName;
	GetStartTime();

	//데일리 카운트 초기화
	if(beforeday!=bcd2bin(p_data.day)){
		beforeday=bcd2bin(p_data.day);
		dailyCount=0;
	}

	DailyCyle_Count();
	TotalCyle_Count();
	DisplayPageValue(0x21,0x20,dailyCount);
	DisplayPageValue(0x21,0x10,totalCount);

	DisplayInitTempGraph();
	DisplayInitVacuumGraph();

	InitData();
	DisplayNormalValues();
	Write_Flash();//테스트중
}

void Stop(){
	deviceerror[1]=1;
	DisplayPage8Char(0x13,0x01,"ERROR01");
	DisplayPage8Char(0x12,0x01,"ERROR01");
	DisplayPage4Char(0x14,0x10,"01  ");
	DisplayPage(LCD_EORROR_POPUP_PAGE);
	ErrorEndProcess();
	deviceerror[0]=0;
	StopFlag=1;
}


void FactoryTestStart(){
	Select_NORMAL_MODE=0;
	Running_Flag=1;
	HeaterControlMode=2;

	CurrentProcess=1;
	CurrentStep=1;
	EndTimer_Flag=1;

	Vacuum_Check_Count=0;
	StopFlag=0;
	TotalTime=0;
	LiquidFlag=0;
    memset(deviceerror, 0, sizeof(deviceerror));

	VacuumPump(1);
	Fan(1);
	VacuumValve(0);
	VentValve(0);
	InjectionValve(0);
	Plasma(0);
	PeriPump(0);
	GetStartTime();

	//데일리 카운트 초기화
	if(beforeday!=bcd2bin(p_data.day)){
		beforeday=bcd2bin(p_data.day);
		dailyCount=0;
	}
	TotalCyle_Count();
	DailyCyle_Count();
	DisplayPageValue(0x21,0x10,totalCount);
	DisplayPageValue(0x21,0x20,dailyCount);
	InitData();
	Write_Flash();//테스트중
}

void FactoryTestStop(){
	deviceerror[1]=1;
	ErrorEndProcess();
	deviceerror[0]=0;
	StopFlag=1;
}
void TestModeStart(int mode){
	TestMode=mode;
	TestProcess=1;
	EndTimer_Flag=1;
	TestCompleteFlag=0;
}

void TestModeStop(int mode){
	TestMode=9;
	EndTestTimeCounter=10*100;
	DisplayTime(0x25,0x50,EndTestTimeCounter);
	TestProcess=1;
	EndTimer_Flag=1;
}


void Endtime_Check_Process(){
	if(Vaccum_Check==0){

	}
	else if(Vaccum_Check==1){
		if(PreesureCondition[0]<=Pressure){
			deviceerror[3]=1;
		}
	}
	else if(Vaccum_Check==2){
		if(PreesureCondition[1]<=Pressure){
			if(Vacuum_Check_Count<=3){
				EndTimer_Flag = 0;
				Vacuum_Check_Count++;
				EndTimeCounter=CycleData[CurrentProcess][CurrentStep].Time;
				CurrentStep--;
			}
			else{
				if(CurrentProcess==1){
					deviceerror[4]=1;
				}
				else if(CurrentProcess==3){
					deviceerror[8]=1;
				}
			}

		}
		else{
			//기화기 온도 체크
			deviceerror[5]=VaporizerHeaterCheck();
		}
	}
	else if(Vaccum_Check==3){
		if(CurrentProcess==2){
			if(PreesureCondition[2]>=Pressure){
				deviceerror[7]=1;
			}
		}
		else if(CurrentProcess==4){
			if(PreesureCondition[2]>=Pressure){
				deviceerror[10]=1;
			}
		}
	}
	if((CycleData[CurrentProcess][CurrentStep].PartsSetting&0x08)==0x08){
		if(CurrentProcess==1){
			if(LiquidFlag==1){
				deviceerror[6]=1;
			}
		}
		else if(CurrentProcess==3){
			if(LiquidFlag==1){
				deviceerror[9]=1;
			}
		}
	}

}
void ErrorEndProcess(){	//에러 후 종료 프로세스
	if(CurrentProcess==1){
		if(LiquidFlag==1){
			CycleTime=ProcessTime[5]+ProcessTime[6];
			CurrentProcess=5;
			CurrentStep=1;
			EndTimer_Flag=1;
			LiquidFlag=0;
		}
		else if(LiquidFlag==2){
			CycleTime=ProcessTime[5]+ProcessTime[6];
			CurrentProcess=5;
			CurrentStep=1;
			EndTimer_Flag=1;
			LiquidFlag=0;
		}
		else{
			CycleTime=ProcessTime[6];
			CurrentProcess=6;
			CurrentStep=1;
			EndTimer_Flag=1;
		}

	}
	else if(CurrentProcess==2){
		CycleTime=ProcessTime[5]+ProcessTime[6];
		CurrentProcess=5;
		CurrentStep=1;
		EndTimer_Flag=1;
	}
	else if(CurrentProcess==3){
		if(LiquidFlag==1){
			CycleTime=ProcessTime[5]+ProcessTime[6];
			CurrentProcess=5;
			CurrentStep=1;
			EndTimer_Flag=1;
			LiquidFlag=0;
		}
		else if(LiquidFlag==2){
			CycleTime=ProcessTime[5]+ProcessTime[6];
			CurrentProcess=5;
			CurrentStep=1;
			EndTimer_Flag=1;
			LiquidFlag=0;
		}
		else{
			CycleTime=ProcessTime[6];
			CurrentProcess=6;
			CurrentStep=1;
			EndTimer_Flag=1;
		}
	}
	else if(CurrentProcess==4){
		CycleTime=ProcessTime[5]+ProcessTime[6];
		CurrentProcess=5;
		CurrentStep=1;
		EndTimer_Flag=1;
	}
	ReadStepTime();
	ReadProcessTime();
}


void NormalMode(){
	if(CurrentStep>20){
		CurrentProcess++;
		CurrentStep=1;
		ReadStepTime();
	}
	while(CycleData[CurrentProcess][CurrentStep].Time==0){	//Time 0 Skip
		CurrentStep++;
		if(CurrentStep>20){
			CurrentProcess++;
			CurrentStep=1;
			ReadStepTime();
		}
		if(CurrentProcess>6){
			break;
		}

	}
	if(CurrentProcess>6){	//종료 시 초기화
		EndTimeCounter=100;

		VacuumPump(0);
		Fan(0);
		VacuumValve(0);
		VentValve(0);
		InjectionValve(0);
		Plasma(0);
		PeriPump(0);

		HeaterControlMode=1;

		if(StopFlag==0){			//정상 종료
			DisplayPage8Char(0x12,0x01,"COMPLETE");
		}
		else{
			p_data.status=errorcode;
			//DisplayPage8Char(0x12,0x01,"Canceled");
		}

		//비정상 종료
		//DisplayPage8Char(0x12,0x01," ERROR  ");

		//필터 카운트
		CarbonFilter_Count();
		HEPAFilter_Count();
		PlasmaAssy_Count();

		GetEndTime();
		if(autoprintFlag==1){
			CyclePrint();
		}
		SaveCycle();	//여기 확인
		//Write_Setting_Data_Flash();
		HAL_Delay(100);
		//Write_Data_Flash();


		Write_Flash();

		Running_Flag=0;
		ProcessNum=1;
		CurrentProcess=1;
		StepNum=1;
		CurrentStep=1;

		DisplayPage(LCD_RESULT_PAGE);

		ReadStepTime();
		ReadProcessTime();

		DisplayNormalValues();
	}
	else{
		//정상 동작

		ProcessNum=CurrentProcess;
		StepNum=CurrentStep;
		Fan(1);
		VacuumValve((CycleData[CurrentProcess][CurrentStep].PartsSetting&0x01)==0x01);
		VentValve((CycleData[CurrentProcess][CurrentStep].PartsSetting&0x02)==0x02);
		InjectionValve((CycleData[CurrentProcess][CurrentStep].PartsSetting&0x04)==0x04);
		if((CycleData[CurrentProcess][CurrentStep].PartsSetting&0x04)==0x04){
			LiquidFlag=0;
		}
		if((CycleData[CurrentProcess][CurrentStep].PartsSetting&0x08)==0x08){
			LiquidFlag=1;
			PeriPump(1);
			//볼륨 카은트
			RFIDData.volume-=2;
			if(RFIDData.volume<=0){
				RFIDData.volume=0;
			}
			DisplaySterilantData();
		}
		else{
			PeriPump(0);
		}
		Plasma((CycleData[CurrentProcess][CurrentStep].PartsSetting&0x10)==0x10);

		if((CycleData[ProcessNum][StepNum].PartsSetting&0x60)==0x00){
			Vaccum_Check=0;
		}
		else if((CycleData[ProcessNum][StepNum].PartsSetting&0x60)==0x20){
			Vaccum_Check=1;
		}
		else if((CycleData[ProcessNum][StepNum].PartsSetting&0x60)==0x40){
			Vaccum_Check=2;
		}
		else if((CycleData[ProcessNum][StepNum].PartsSetting&0x60)==0x60){
			Vaccum_Check=3;
		}

		if(CurrentProcess==5){
			HeaterControlMode=1;
		}
		else if(CurrentProcess==6){
			HeaterControlMode=1;
		}
		EndTimeCounter = CycleData[CurrentProcess][CurrentStep].Time*100;;//공정 시간(초)
	}
}

void FactoryTestMode(){
	if(CurrentStep>20){
		CurrentProcess++;
		CurrentStep=1;
		ReadStepTime();
	}
	while(CycleData[CurrentProcess][CurrentStep].Time==0){	//Time 0 Skip
		CurrentStep++;
		if(CurrentStep>20){
			CurrentProcess++;
			CurrentStep=1;
			ReadStepTime();
		}
		if(CurrentProcess>6){
			break;
		}

	}
	if(CurrentProcess>6){	//종료 시 초기화
		EndTimeCounter=100;
		Running_Flag=0;
		ProcessNum=1;
		CurrentProcess=1;
		StepNum=1;
		CurrentStep=1;

		VacuumPump(0);
		Fan(0);
		VacuumValve(0);
		VentValve(0);
		InjectionValve(0);
		Plasma(0);
		PeriPump(0);

		HeaterControlMode=1;

		if(StopFlag==0){			//정상 종료
			p_data.status=11;
		}
		else{
			p_data.status=errorcode;
		}

		CarbonFilter_Count();
		HEPAFilter_Count();
		PlasmaAssy_Count();


		GetEndTime();
		if(autoprintFlag==1){
			CyclePrint();
		}
		SaveCycle();	//여기 확인

		Write_Flash();


		ReadStepTime();
		ReadProcessTime();

		DisplayProcessTestValues();
		DisplayPage(LCD_FACTORY_TESTMODE_PROCESSTEST1_PAGE);
	}
	else{	//정상 동작
		if(CurrentStep>10){
			DisplayPage(LCD_FACTORY_TESTMODE_PROCESSTEST4_PAGE);
		}
		else{
			DisplayPage(LCD_FACTORY_TESTMODE_PROCESSTEST3_PAGE);
		}

		ProcessNum=CurrentProcess;
		StepNum=CurrentStep;
		Fan(1);
		VacuumValve((CycleData[CurrentProcess][CurrentStep].PartsSetting&0x01)==0x01);
		VentValve((CycleData[CurrentProcess][CurrentStep].PartsSetting&0x02)==0x02);
		InjectionValve((CycleData[CurrentProcess][CurrentStep].PartsSetting&0x04)==0x04);
		if((CycleData[CurrentProcess][CurrentStep].PartsSetting&0x08)==0x08){
			LiquidFlag=1;
			PeriPump(1);
			//볼륨 카은트
			RFIDData.volume-=2;
			if(RFIDData.volume<=0){
				RFIDData.volume=0;
			}
			DisplaySterilantData();
		}
		else{
			PeriPump(0);
		}
		Plasma((CycleData[CurrentProcess][CurrentStep].PartsSetting&0x10)==0x10);


		if((CycleData[ProcessNum][StepNum].PartsSetting&0x60)==0x00){
			Vaccum_Check=0;
		}
		else if((CycleData[ProcessNum][StepNum].PartsSetting&0x60)==0x20){
			Vaccum_Check=1;
		}
		else if((CycleData[ProcessNum][StepNum].PartsSetting&0x60)==0x40){
			Vaccum_Check=2;
		}
		else if((CycleData[ProcessNum][StepNum].PartsSetting&0x60)==0x60){
			Vaccum_Check=3;
		}

		if(CurrentProcess==5){
			HeaterControlMode=1;
		}
		else if(CurrentProcess==6){
			HeaterControlMode=1;
		}
		EndTimeCounter = CycleData[CurrentProcess][CurrentStep].Time*100;;//공정 시간(초)
	}
}



void ProcessEndTimer(void){
	if(Running_Flag) {
		if(Select_NORMAL_MODE==1){
			NormalMode();
		}
		else{
			FactoryTestMode();
		}
	}

	//진공테스트
	if(TestMode==1) {
		if(TestProcess==1){
			EndTestTimeCounter=(leaktesttime*10+15)*100;
			TestProcess=2;
			VacuumPump(1);
			VacuumValve(1);
			DisplayPage4Char(0x25,0x40,"TEST");	//결과값
			DisplayPage4Char(0x25,0x44,"TEST");	//결과값
			EndTimeCounter=leaktesttime*100;
		}
		else if(TestProcess==2){
			TestPressure[0]=Pressure;
			DisplayPageValue(0x25,0x10,Pressure*10);
			TestProcess=3;
			EndTimeCounter=leaktesttime*100;
		}
		else if(TestProcess==3){
			TestPressure[1]=Pressure;
			DisplayPageValue(0x25,0x14,Pressure*10);
			TestProcess=4;
			EndTimeCounter=leaktesttime*100;
		}
		else if(TestProcess==4){
			TestPressure[2]=Pressure;
			DisplayPageValue(0x25,0x18,Pressure*10);
			TestProcess=5;
			EndTimeCounter=leaktesttime*100;
		}
		else if(TestProcess==5){
			TestPressure[3]=Pressure;
			DisplayPageValue(0x25,0x1C,Pressure*10);
			TestProcess=6;
			EndTimeCounter=leaktesttime*100;
		}
		else if(TestProcess==6){
			VacuumPump(0);
			VacuumValve(0);
			TestPressure[4]=Pressure;
			DisplayPageValue(0x25,0x20,Pressure*10);
			if(TestPressure[4]>TestVacuumValue){
				Pressuererror[0]=1;
				DisplayPage4Char(0x25,0x40,"FAIL");	//결과값
			}
			else{
				Pressuererror[0]=0;
				DisplayPage4Char(0x25,0x40,"PASS");	//결과값
			}
			if(Pressure==0){
				Pressuererror[0]=1;
				DisplayPage4Char(0x25,0x40,"FAIL");	//결과값
			}
			TestProcess=7;
			EndTimeCounter=leaktesttime*100;
		}
		else if(TestProcess==7){
			TestPressure[5]=Pressure;
			if((TestPressure[5]-TestPressure[4])>TestLeakValue){
				Pressuererror[1]=1;
			}
			else{
				Pressuererror[1]=0;
			}
			DisplayPageValue(0x25,0x24,Pressure*10);
			TestProcess=8;
			EndTimeCounter=leaktesttime*100;
		}
		else if(TestProcess==8){
			TestPressure[6]=Pressure;
			if((TestPressure[6]-TestPressure[5])>TestLeakValue){
				Pressuererror[1]=1;
			}
			else{
				Pressuererror[1]=0;
			}
			DisplayPageValue(0x25,0x28,Pressure*10);
			TestProcess=9;
			EndTimeCounter=leaktesttime*100;
		}
		else if(TestProcess==9){
			TestPressure[7]=Pressure;
			if((TestPressure[7]-TestPressure[6])>TestLeakValue){
				Pressuererror[1]=1;
			}
			else{
				Pressuererror[1]=0;
			}
			DisplayPageValue(0x25,0x2C,Pressure*10);
			TestProcess=10;
			EndTimeCounter=leaktesttime*100;
		}
		else if(TestProcess==10){
			TestPressure[8]=Pressure;
			if((TestPressure[8]-TestPressure[7])>TestLeakValue){
				Pressuererror[1]=1;
			}
			else{
				Pressuererror[1]=0;
			}
			DisplayPageValue(0x25,0x30,Pressure*10);
			TestProcess=11;
			EndTimeCounter=leaktesttime*100;
		}
		else if(TestProcess==11){
			TestPressure[9]=Pressure;
			if((TestPressure[9]-TestPressure[8])>TestLeakValue){
				Pressuererror[1]=1;
			}
			else{
				Pressuererror[1]=0;
			}
			DisplayPageValue(0x25,0x34,Pressure*10);
			VentValve(1);
			TestProcess=12;
			EndTimeCounter=15*100;
		}
		else if(TestProcess==12){
			VentValve(0);
			EndTimeCounter=1*100;
			TestMode=0;
			if(Pressuererror[1]==0){
				DisplayPage4Char(0x25,0x44,"PASS");	//결과값
			}
			else{
				DisplayPage4Char(0x25,0x44,"FAIL");	//결과값
			}
			if(Pressure==0){
				Pressuererror[1]=1;
				DisplayPage4Char(0x25,0x44,"FAIL");	//결과값
			}
			TestCompleteFlag=1;
        	if(CurrentUser==10){
        		DisplayPage(LCD_USER_LEAKTEST_PAGE);
    		}
        	else if(CurrentUser==9){
        		DisplayPage(LCD_USER_LEAKTEST_PAGE+50);
        	}
        	else{
        		DisplayPage(LCD_USER_LEAKTEST_PAGE+60);
        	}
		}
	}

	//히터 테스트
	else if(TestMode==2) {
		if(TestProcess==1){
			temperror[0]=0;
			temperror[1]=0;
			temperror[2]=0;
			temperror[3]=0;
			temperror[4]=0;

			HeaterControlMode=1;
			TestProcess=2;
			EndTimeCounter=100;
		}
		else if(TestProcess==2){
			HeatingTest();
			//결과 디스플레이
			if(temperror[0]==1){
				DisplayPage4Char(0x26,0x10,"FAIL");
			}
			else{
				DisplayPage4Char(0x26,0x10,"PASS");
			}
			if(temperror[1]==1){
				DisplayPage4Char(0x26,0x14,"FAIL");
			}
			else{
				DisplayPage4Char(0x26,0x14,"PASS");
			}
			if(temperror[2]==1){
				DisplayPage4Char(0x26,0x18,"FAIL");
			}
			else{
				DisplayPage4Char(0x26,0x18,"PASS");
			}
			if(temperror[3]==1){
				DisplayPage4Char(0x26,0x1C,"FAIL");
			}
			else{
				DisplayPage4Char(0x26,0x1C,"PASS");
			}
			if(temperror[4]==1){
				DisplayPage4Char(0x26,0x20,"FAIL");
			}
			else{
				DisplayPage4Char(0x26,0x20,"PASS");
			}

			//추가 수정
			EndTimeCounter=100;
			TestMode=0;
			TestCompleteFlag=1;
		}
	}

	//벨브 테스트
	else if(TestMode==3) {
			if(TestProcess==1){
				TestProcess=2;
				VacuumValve(0);
				VentValve(0);
				InjectionValve(1);
				Valveerror[0]=0;
				Valveerror[1]=0;
				Valveerror[2]=0;
				EndTimeCounter=1*100;
			}
			else if(TestProcess==2){
				TestProcess=3;
				if(ValveCheck()!=1){
					Valveerror[0]=1;
					DisplayPage4Char(0x27,0x20,"FAIL");
				}
				else{
					Valveerror[0]=0;
					DisplayPage4Char(0x27,0x20,"PASS");
				}
				EndTimeCounter=1*100;
			}
			else if(TestProcess==3){
				TestProcess=4;
				VacuumValve(0);
				VentValve(1);
				InjectionValve(0);
				EndTimeCounter=1*100;
			}
			else if(TestProcess==4){
				TestProcess=5;
				if(ValveCheck()!=1){
					Valveerror[1]=1;
					DisplayPage4Char(0x27,0x24,"FAIL");
				}
				else{
					Valveerror[1]=0;
					DisplayPage4Char(0x27,0x24,"PASS");
				}
				EndTimeCounter=1*100;
			}
			else if(TestProcess==5){
				TestProcess=6;
				VacuumValve(1);
				VentValve(0);
				InjectionValve(0);
				EndTimeCounter=1*100;
			}
			else if(TestProcess==6){
				TestProcess=7;
				if(ValveCheck()!=1){
					Valveerror[2]=1;
					DisplayPage4Char(0x27,0x28,"FAIL");
				}
				else{
					Valveerror[2]=0;
					DisplayPage4Char(0x27,0x28,"PASS");
				}
				EndTimeCounter=1*100;
			}
			else if(TestProcess==7){
				TestProcess=8;
				VacuumValve(0);
				VentValve(0);
				InjectionValve(0);
				if(Valveerror[0]+Valveerror[1]+Valveerror[2]==0){
					DisplayPage4Char(0x27,0x2C,"PASS");
				}
				else{
					DisplayPage4Char(0x27,0x2C,"FAIL");
				}
				EndTimeCounter=1*100;
			}

			else if(TestProcess==8){
				EndTimeCounter=1*100;
				TestMode=0;
				TestCompleteFlag=1;
			}
		}

	if(TestMode==9) {
		if(TestProcess==1){
			VacuumPump(0);
			VacuumValve(1);
			VentValve(1);
			TestProcess=2;
			EndTimeCounter=10*100;
		}
		else if(TestProcess==2){
			VacuumPump(0);
			VacuumValve(0);
			VentValve(0);

			EndTimeCounter=1*100;
			TestMode=0;
			TestCompleteFlag=0;
			Display25page();
	       	if(CurrentUser==10){
				DisplayPage(LCD_USER_LEAKTEST_PAGE);
			}
			else if(CurrentUser==9){
				DisplayPage(LCD_USER_LEAKTEST_PAGE+50);
			}
			else{
				DisplayPage(LCD_USER_LEAKTEST_PAGE+60);
			}
		}
	}
	EndTimer_Flag = 0;
}



void GetStartTime(){
	unsigned char week;
	ReadRTC( &p_data.year, &p_data.month, &p_data.day, &week,
			&p_data.start_hour, &p_data.start_minute, &p_data.start_second);

}
void GetEndTime(){
	unsigned char year,month,day,week;
	ReadRTC( &year, &month, &day, &week,
			&p_data.end_hour, &p_data.end_minute, &p_data.end_second);
}

void GetData(){
	if(Temperature[1]>p_data.tempmax[CurrentProcess]){
		p_data.tempmax[CurrentProcess]=Temperature[1];	//Max Temperature
	}
	if(Pressure>p_data.pressuremax[CurrentProcess]){
		p_data.pressuremax[CurrentProcess]=Pressure;	//Max Pressure
	}
	if(Pressure<p_data.pressuremin[CurrentProcess]){
		p_data.pressuremin[CurrentProcess]=Pressure;	//Max Pressure
	}
}
void InitData(){
	for(int i=1;i<7;i++){
		p_data.tempmax[i]=0;
		p_data.pressuremax[i]=0;
		p_data.pressuremin[i]=760;
		fProcessTime[i]=0;
	}
	errorcode=0;
}

int calculateElapsedDays(int current_year, int current_month, int current_day, int open_year, int open_month, int open_day) {
    int elapsed_days = 0;
    int days_in_month[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // Calculate the number of elapsed days since the open date
    while (open_year < current_year ||
           (open_year == current_year && open_month < current_month) ||
           (open_year == current_year && open_month == current_month && open_day < current_day)) {
        elapsed_days++;
        open_day++;

        if (open_day > days_in_month[open_month]) {
            open_day = 1;
            open_month++;
        }

        if (open_month > 12) {
            open_month = 1;
            open_year++;
        }
    }

    return elapsed_days;
}

int check_expiry() {
    // BCD를 바이너리 값으로 변환
    unsigned int year = bcd2bin(today_date.year);
    unsigned int month = bcd2bin(today_date.month);
    unsigned int day = bcd2bin(today_date.day);

    // 연도 비교
    if (RFIDData.expiry_year < year) {
        return 0; // 만료됨
    } else if (RFIDData.expiry_year > year) {
        return 1; // 만료 안됨
    } else { // 연도가 같을 경우
        // 월 비교
        if (RFIDData.expiry_month < month) {
            return 0; // 만료됨
        } else if (RFIDData.expiry_month > month) {
            return 1; // 만료 안됨
        } else { // 월도 같을 경우
            // 일 비교
            if (RFIDData.expiry_day < day) {
                return 0; // 만료됨
            } else {
                return 1; // 만료 안됨 (같거나 이후)
            }
        }
    }
}
