/*
 * Process.h
 *
 *  Created on: Aug 10, 2022
 *      Author: CBT
 */

#ifndef SRC_PERIPHERAL_PROCESS_H_
#define SRC_PERIPHERAL_PROCESS_H_

struct data_format {
	unsigned char cyclename;
	unsigned char year, month, day;
	unsigned char start_hour, start_minute, start_second;
	unsigned char end_hour, end_minute, end_second;

	float	tempmax[7],pressuremax[7],pressuremin[7];
	unsigned char status;

	unsigned char sterilantNum[10];
	unsigned char Num[10];

};

extern struct data_format p_data;	//프린트 데이터
extern struct data_format load_data;	//로드 데이터

extern int HeaterControlMode;

extern unsigned int TotalTime;
extern unsigned int fProcessTime[7];
extern unsigned int CycleTime;
extern unsigned int EndTime;
extern unsigned int ProcessTime[7];
extern unsigned int StepTime[21];


extern int CycleName;
extern int ProcessNum,StepNum;
extern int testbit;

extern unsigned char deviceerror[11];
extern int errorcode;


extern int TestMode;
extern int TestCompleteFlag;
extern int TestMode,TestProcess;

extern int Select_NORMAL_MODE;
extern int StopFlag;


extern unsigned char ProcessFlag, CurrentProcess, CurrentStep, ProcessStep;
extern unsigned int ProcessTime[7];
extern unsigned int StepTime[21];

extern int TestVacuumValue;
extern int TestLeakValue;

extern int HeaterControlMode;

struct Process_data_format {
	unsigned int PartsSetting;
	unsigned int	Time;
};
extern struct Process_data_format	CycleData[7][21];


extern float TestPressure[10];
extern int Pressuererror[2];
extern int Valveerror[3];


void loop();

void DeliSecondProcess(void);
void HalfSecondProcess(void);
void OneSecondProcess(void);
void Init_Device(void);
void Inithardware();



void Start(void);
void Stop(void);
void FactoryTestStop();
void FactoryTestStart();
void ProcessEndTimer(void);


void FactoryTestMode();
void NormalMode();
void Endtime_Check_Process();
void ErrorCheck();
void ErrorEndProcess();
void DeviceErrorProcess();

void TestModeStart(int mode);
void TestModeStop(int mode);

void GetStartTime();
void GetEndTime();
void GetData();
void InitData();

int calculateElapsedDays(int current_year, int current_month, int current_day, int open_year, int open_month, int open_day);

#endif /* SRC_PERIPHERAL_PROCESS_H_ */
