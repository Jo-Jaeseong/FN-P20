/*
 * usb.h
 *
 *  Created on: Mar 25, 2024
 *      Author: CBT_DEV
 */

#ifndef SRC_PERIPHERAL_USBMEMORY_H_
#define SRC_PERIPHERAL_USBMEMORY_H_


typedef struct {
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	//모델 정보
    char model_name[10];
    char model_number[10];
    char facility_name[10];
    char department_name[10];
	//----------------------------------------------------------------
    // 용액 정보
    int serialNumber[4]; //시리얼넘버
	int loadingDate[3]; // 오픈 일자
	int expiryDate[3]; // 만료 일자
	int volume; // 남은 양
	//----------------------------------------------------------------
	int cycleName; // 공정 이름
	//----------------------------------------------------------------
    int totalCount;
    int dailyCount; // 카운트
    int date[3]; // 공정 일자
    int startTime[3]; // 시작 시간
	int endTime[3]; // 종료 시간
    //----------------------------------------------------------------
	int processTime[7][2]; // 각 프로세스 별 동작 시간
    int maxTemperature[7]; //최대 온도
    int maxPressure[7]; // 최대 압력
    int minPressure[7]; // 최소 압력
	//----------------------------------------------------------------
    int carbonFilter;
    int hepaFilter;
    int plasmaAssy;
	//----------------------------------------------------------------
    int totalTime[2]; // 총 동작 시간
	int status; // 상태
	int ID;
	//----------------------------------------------------------------

    int temperature[270]; // 온도
    int pressure[270]; // 압력

} CycleUSBData;

extern CycleUSBData saveCycleData;
extern int temptotalcycle[11];
extern int fileCount;


void SaveCycleData();
void LoadCycleData(int readyear, int readmonth, int readday, int readcount);

void SaveSettingData(CycleUSBData* data);
void LoadSettingData(CycleUSBData* data);
void ReadListData(int year, int month, int day);
void USB_Error_Handler(void);

#endif /* SRC_PERIPHERAL_USBMEMORY_H_ */
