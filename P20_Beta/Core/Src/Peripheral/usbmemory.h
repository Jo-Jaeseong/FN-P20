/*
 * usb.h
 *
 *  Created on: Mar 25, 2024
 *      Author: CBT_DEV
 */

#ifndef SRC_PERIPHERAL_USBMEMORY_H_
#define SRC_PERIPHERAL_USBMEMORY_H_

typedef struct {
    int year, month, day; // 공정 일자
    int processName; // 공정 이름
    char startTime[9]; // 시작 시간
    char endTime[9]; // 종료 시간
    int status; // 상태
    int totalCount, dailyCount; // 카운트

    // 용액 정보
    struct {
        int prodYear, prodMonth, prodDay; // 생산 일자
        int openYear, openMonth, openDay; // 오픈 일자
        int remainingAmount; // 남은 양
        int remainingDays; // 남은 일수
    } solutionInfo;

    // 동작 시간
    struct {
        char total[6]; // 총 동작 시간
        char perProcess[6][6]; // 각 프로세스 별 동작 시간
    } operationTime;

    // 필터 사용 횟수
    struct {
        int carbonFilter;
        int hepaFilter;
        int plasmaAssy;
    } filterUsage;

    // 각 프로세스별 정보
    struct {
        int temperature; // 온도
        int maxPressure; // 최대 압력
        int minPressure; // 최소 압력
    } processInfo[6]; // 6개의 프로세스

    // 공정 데이터
    struct {
        int temperature; // 온도
        int pressure; // 압력
    } processData[480]; // 5초 단위 데이터, 최대 480번

} ProcessData;

extern ProcessData myProcessData;



void fprintCycelData();
void LoadCycleData(ProcessData* data);
void SaveCycleData();

void SaveSettingData(ProcessData* data);
void LoadSettingData(ProcessData* data);
void ReadListData(void);

void USB_Error_Handler(void);

#endif /* SRC_PERIPHERAL_USBMEMORY_H_ */
