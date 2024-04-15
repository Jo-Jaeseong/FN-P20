/*
 * usb.c
 *
 *  Created on: Mar 25, 2024
 *      Author: CBT_DEV
 */


/*
 *Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Src/usbh_msc.c
 * if (status == USBH_NOT_SUPPORTED)를
 * if (status == USBH_NOT_SUPPORTED|| (status == USBH_BUSY))로 수정 필요
 */

/*
 * usb.c
 *
 *  Created on: Mar 25, 2024
 *      Author: CBT_DEV
 */
#include "stdio.h"
#include "main.h"
#include "fatfs.h"
#include "ff.h"
#include "usb_host.h"
#include "peripheral.h"
#include "sensor.h"
#include "hardware.h"


extern char USBHPath[4];   /* USBH logical drive path */
extern FATFS USBHFatFS;    /* File system object for USBH logical drive */
extern FIL USBHFile;       /* File object for USBH */
FRESULT USBret;
int firstcall=1;
char buffer[1024];

DIR dir; // 디렉토리 객체
FILINFO fno; // 파일 정보 객체

extern USBH_HandleTypeDef hUsbHostFS;
extern ApplicationTypeDef Appli_state;

CycleUSBData saveCycleData;

int temptotalcycle[11];

void SaveCycle(){
    char filename[25]; // 파일 이름을 저장할 배열
	//GetStartTime();
	//GetEndTime();
    sprintf(filename, "%02d-%02d-%02d", bcd2bin(p_data.year), bcd2bin(p_data.month), bcd2bin(p_data.day));
    //sprintf(filename + strlen(filename), "%02d%02d%02d.txt",bcd2bin(p_data.start_hour),bcd2bin(p_data.start_minute),bcd2bin(p_data.start_second));
    sprintf(filename + strlen(filename), "_%03d.txt",totalCount);
    //sprintf(filename, "0.txt");
	// USB 메모리가 준비된 상태인지 확인
	if(Appli_state == APPLICATION_READY){
		// USB 메모리를 마운트
		USBret = f_mount(&USBHFatFS, USBHPath, 1);
		if(USBret == FR_OK) {
			// 파일 생성 및 열기
			USBret = f_open(&USBHFile, filename, FA_CREATE_ALWAYS | FA_WRITE);
			if(USBret == FR_OK) {
				f_printf(&USBHFile,"SERIAL NO     :  %02d-%02d-%02d-%02d\n",(int)RFIDData.production_year,(int)RFIDData.production_month,(int)RFIDData.production_day, (int)RFIDData.production_number);
				f_printf(&USBHFile,"Loading Date  :  %2d-%02d-%02d\n",(int)RFIDData.open_year,(int)RFIDData.open_month,(int)RFIDData.open_day);
				f_printf(&USBHFile,"Expiry Date   :  %2d-%02d-%02d\n",(int)RFIDData.expiry_year,(int)RFIDData.expiry_month,(int)RFIDData.expiry_day);
				f_printf(&USBHFile,"Remain H2O2   :  %02d\n",RFIDData.volume);

				f_printf(&USBHFile,"Selected Cycle:  %d    \n",CycleName);

				f_printf(&USBHFile,"Total Cycle   :  %3d     \n",totalCount);
				f_printf(&USBHFile,"Daily Cycle   :  %3d     \n",dailyCount);
				f_printf(&USBHFile,"Cycle Date    :  %02d-%02d-%02d\n", bcd2bin(p_data.year),bcd2bin(p_data.month),bcd2bin(p_data.day));

				f_printf(&USBHFile,"Start Time    :  %02d:%02d:%02d\n",
						bcd2bin(p_data.start_hour),bcd2bin(p_data.start_minute),bcd2bin(p_data.start_second));
				f_printf(&USBHFile,"End Time      :  %02d:%02d:%02d\n",
							bcd2bin(p_data.end_hour),bcd2bin(p_data.end_minute),bcd2bin(p_data.end_second));

				int tempminute,tempsecond;
				tempminute=(fProcessTime[1]/100)/60;
				tempsecond=(fProcessTime[1]/100)%60;
				f_printf(&USBHFile,"1_Time  :  %02d:%02d      \n",tempminute,tempsecond);
				f_printf(&USBHFile,"1.Temp(Max)     :  %3d   \n",(int)p_data.tempmax[1]);
				f_printf(&USBHFile,"1.Pressure(Max) :  %4d   \n",(int)p_data.pressuremax[1]);
				f_printf(&USBHFile,"1.Pressure(Min) :  %4d   \n",(int)p_data.pressuremin[1]);


				tempminute=(fProcessTime[2]/100)/60;
				tempsecond=(fProcessTime[2]/100)%60;
				f_printf(&USBHFile,"2_Time   :  %02d:%02d      \n",tempminute,tempsecond);
				f_printf(&USBHFile,"2.Temp(Max)     :  %3d   \n",(int)p_data.tempmax[2]);
				f_printf(&USBHFile,"2.Pressure(Max) :  %4d   \n",(int)p_data.pressuremax[2]);
				f_printf(&USBHFile,"2.Pressure(Min) :  %4d   \n",(int)p_data.pressuremin[2]);

				tempminute=(fProcessTime[3]/100)/60;
				tempsecond=(fProcessTime[3]/100)%60;
				f_printf(&USBHFile,"3_Time    :  %02d:%02d      \n",tempminute,tempsecond);
				f_printf(&USBHFile,"3.Temp(Max)     :  %3d   \n",(int)p_data.tempmax[3]);
				f_printf(&USBHFile,"3.Pressure(Max) :  %4d   \n",(int)p_data.pressuremax[3]);
				f_printf(&USBHFile,"3.Pressure(Min) :  %4d   \n",(int)p_data.pressuremin[3]);

				tempminute=(fProcessTime[4]/100)/60;
				tempsecond=(fProcessTime[4]/100)%60;
				f_printf(&USBHFile,"4_Time    :  %02d:%02d      \n",tempminute,tempsecond);
				f_printf(&USBHFile,"4.Temp(Max)     :  %3d   \n",(int)p_data.tempmax[4]);
				f_printf(&USBHFile,"4.Pressure(Max) :  %4d   \n",(int)p_data.pressuremax[4]);
				f_printf(&USBHFile,"4.Pressure(Min) :  %4d   \n",(int)p_data.pressuremin[4]);

				tempminute=(fProcessTime[5]/100)/60;
				tempsecond=(fProcessTime[5]/100)%60;
				f_printf(&USBHFile,"5_Time    :  %02d:%02d      \n",tempminute,tempsecond);
				f_printf(&USBHFile,"5.Temp(Max)     :  %3d   \n",(int)p_data.tempmax[5]);
				f_printf(&USBHFile,"5.Pressure(Max) :  %4d   \n",(int)p_data.pressuremax[5]);
				f_printf(&USBHFile,"5.Pressure(Min) :  %4d   \n",(int)p_data.pressuremin[5]);

				tempminute=(fProcessTime[6]/100)/60;
				tempsecond=(fProcessTime[6]/100)%60;
				f_printf(&USBHFile,"6_Time    :  %02d:%02d      \n",tempminute,tempsecond);
				f_printf(&USBHFile,"6.Temp(Max)     :  %3d   \n",(int)p_data.tempmax[6]);
				f_printf(&USBHFile,"6.Pressure(Max) :  %4d   \n",(int)p_data.pressuremax[6]);
				f_printf(&USBHFile,"6.Pressure(Min) :  %4d   \n",(int)p_data.pressuremin[6]);

				f_printf(&USBHFile,"CARBON FILTER :  %04d     \n",CarbonFilter);
				f_printf(&USBHFile,"HEPA FILTER   :  %04d     \n",HEPAFilter);
				f_printf(&USBHFile,"PLASMA ASSY   :  %04d     \n",PlasmaAssy);

				tempminute=(TotalTime/100)/60;
				tempsecond=(TotalTime/100)%60;
				f_printf(&USBHFile,"Cycle Time    :  %02d:%02d \n",tempminute,tempsecond);
				f_printf(&USBHFile,"Cycle Status  :  %02d      \n",p_data.status);
				f_printf(&USBHFile,"Operator ID   :  %02d      \n",CurrentUser);				// 파일 닫기
				f_close(&USBHFile);
			}
			// USB 메모리 마운트 해제
			f_mount(NULL, USBHPath, 0);
		}
	}
}

void LoadCycleData(int readyear, int readmonth, int readday, int readcount) {
	char filename[20]; // 파일 이름을 저장할 배열
	char line[100]; // 파일로부터 읽은 한 줄을 저장할 배열
    sprintf(filename, "%02d-%02d-%02d", readyear, readmonth, readday);
    //sprintf(filename + strlen(filename), "%02d%02d%02d.txt",bcd2bin(p_data.start_hour),bcd2bin(p_data.start_minute),bcd2bin(p_data.start_second));
    sprintf(filename + strlen(filename), "_%03d.txt",readcount);
	if(Appli_state == APPLICATION_READY){
		// USB 메모리를 마운트
		USBret = f_mount(&USBHFatFS, USBHPath, 1);
		if(USBret == FR_OK) {
			// 파일 열기
			USBret = f_open(&USBHFile, filename, FA_READ);
			if(USBret == FR_OK) {
				// 데이터 불러오기를 위한 코드
				while(f_gets(line, sizeof(line), &USBHFile)) {
				        if(strncmp(line, "SERIAL NO", 9) == 0) {
				        	sscanf(line, "SERIAL NO : %02d-%02d-%02d-%02d",
				                   &saveCycleData.serialNumber[0],
				                   &saveCycleData.serialNumber[1],
				                   &saveCycleData.serialNumber[2],
				                   &saveCycleData.serialNumber[3]);
				        }
				        else if(strncmp(line, "Loading Date", 12) == 0) {
				            sscanf(line, "Loading Date : %2d-%02d-%02d",
				                   &saveCycleData.loadingDate[0],
				                   &saveCycleData.loadingDate[1],
				                   &saveCycleData.loadingDate[2]);
				        }
				        else if(strncmp(line, "Expiry Date", 11) == 0) {
				            sscanf(line, "Expiry Date : %2d-%02d-%02d",
				                   &saveCycleData.expiryDate[0],
				                   &saveCycleData.expiryDate[1],
				                   &saveCycleData.expiryDate[2]);
				        }
				        else if(strncmp(line, "Remain H2O2", 11) == 0) {
				            sscanf(line, "Remain H2O2 : %d", &saveCycleData.volume);
				        }
				        else if(strncmp(line, "Selected Cycle", 14) == 0) {
							sscanf(line, "Selected Cycle:  %d", &saveCycleData.cycleName);
						}
				        else if(strncmp(line, "Total Cycle", 11) == 0) {
							sscanf(line, "Total Cycle   :  %3d", &saveCycleData.totalCount);
						}
				        else if(strncmp(line, "Daily Cycle", 11) == 0) {
							sscanf(line, "Daily Cycle   :  %3d", &saveCycleData.dailyCount);
						}
				        else if(strncmp(line, "Cycle Date", 10) == 0) {
							sscanf(line, "Cycle Date    :  %02d-%02d-%02d",
									&saveCycleData.date[0],
									&saveCycleData.date[1],
									&saveCycleData.date[2]);
						}
				        else if(strncmp(line, "Start Time", 10) == 0) {
				            sscanf(line, "Start Time    :  %02d:%02d:%02d",
				                   &saveCycleData.startTime[0],
				                   &saveCycleData.startTime[1],
				                   &saveCycleData.startTime[2]);
				        }
				        else if(strncmp(line, "End Time", 8) == 0) {
				            sscanf(line, "End Time      :  %02d:%02d:%02d",
				                   &saveCycleData.endTime[0],
				                   &saveCycleData.endTime[1],
				                   &saveCycleData.endTime[2]);
				        }
				        else if(strncmp(line, "1_Time", 6) == 0) {
				            sscanf(line, "1_Time  :  %02d:%02d",
				                   &saveCycleData.processTime[1][0],
				                   &saveCycleData.processTime[1][1]);
				        }
				        else if(strncmp(line, "1.Temp(Max)", 11) == 0) {
				            sscanf(line, "1.Temp(Max)     :  %3d",&saveCycleData.maxTemperature[1]);

				        }
				        else if(strncmp(line, "1.Pressure(Max)", 15) == 0) {
							sscanf(line, "1.Pressure(Max) :  %4d",&saveCycleData.maxPressure[1]);

						}
				        else if(strncmp(line, "1.Pressure(Min)", 15) == 0) {
							sscanf(line, "1.Pressure(Min) :  %4d",&saveCycleData.minPressure[1]);

						}
				        else if(strncmp(line, "2_Time", 6) == 0) {
				            sscanf(line, "2_Time  :  %02d:%02d",
				                   &saveCycleData.processTime[2][0],
				                   &saveCycleData.processTime[2][1]);
				        }
				        else if(strncmp(line, "2.Temp(Max)", 11) == 0) {
				            sscanf(line, "2.Temp(Max)     :  %3d",&saveCycleData.maxTemperature[2]);

				        }
				        else if(strncmp(line, "2.Pressure(Max)", 15) == 0) {
							sscanf(line, "2.Pressure(Max) :  %4d",&saveCycleData.maxPressure[2]);

						}
				        else if(strncmp(line, "2.Pressure(Min)", 15) == 0) {
							sscanf(line, "2.Pressure(Min) :  %4d",&saveCycleData.minPressure[2]);

						}
				        else if(strncmp(line, "3_Time", 6) == 0) {
				            sscanf(line, "3_Time  :  %02d:%02d",
				                   &saveCycleData.processTime[3][0],
				                   &saveCycleData.processTime[3][1]);
				        }
				        else if(strncmp(line, "3.Temp(Max)", 11) == 0) {
				            sscanf(line, "3.Temp(Max)     :  %3d",&saveCycleData.maxTemperature[3]);

				        }
				        else if(strncmp(line, "3.Pressure(Max)", 15) == 0) {
							sscanf(line, "3.Pressure(Max) :  %4d",&saveCycleData.maxPressure[3]);

						}
				        else if(strncmp(line, "3.Pressure(Min)", 15) == 0) {
							sscanf(line, "3.Pressure(Min) :  %4d",&saveCycleData.minPressure[3]);

						}
				        else if(strncmp(line, "4_Time", 6) == 0) {
				            sscanf(line, "4_Time  :  %02d:%02d",
				                   &saveCycleData.processTime[4][0],
				                   &saveCycleData.processTime[4][1]);
				        }
				        else if(strncmp(line, "4.Temp(Max)", 11) == 0) {
				            sscanf(line, "4.Temp(Max)     :  %3d",&saveCycleData.maxTemperature[4]);

				        }
				        else if(strncmp(line, "4.Pressure(Max)", 15) == 0) {
							sscanf(line, "4.Pressure(Max) :  %4d",&saveCycleData.maxPressure[4]);

						}
				        else if(strncmp(line, "4.Pressure(Min)", 15) == 0) {
							sscanf(line, "4.Pressure(Min) :  %4d",&saveCycleData.minPressure[4]);

						}
				        else if(strncmp(line, "5_Time", 6) == 0) {
				            sscanf(line, "5_Time  :  %02d:%02d",
				                   &saveCycleData.processTime[5][0],
				                   &saveCycleData.processTime[5][1]);
				        }
				        else if(strncmp(line, "5.Temp(Max)", 11) == 0) {
				            sscanf(line, "5.Temp(Max)     :  %3d",&saveCycleData.maxTemperature[5]);

				        }
				        else if(strncmp(line, "5.Pressure(Max)", 15) == 0) {
							sscanf(line, "5.Pressure(Max) :  %4d",&saveCycleData.maxPressure[5]);

						}
				        else if(strncmp(line, "5.Pressure(Min)", 15) == 0) {
							sscanf(line, "5.Pressure(Min) :  %4d",&saveCycleData.minPressure[5]);
						}
				        else if(strncmp(line, "6_Time", 6) == 0) {
				            sscanf(line, "6_Time  :  %02d:%02d",
				                   &saveCycleData.processTime[6][0],
				                   &saveCycleData.processTime[6][1]);
				        }
				        else if(strncmp(line, "6.Temp(Max)", 11) == 0) {
				            sscanf(line, "6.Temp(Max)     :  %3d",&saveCycleData.maxTemperature[6]);
				        }
				        else if(strncmp(line, "6.Pressure(Max)", 15) == 0) {
							sscanf(line, "6.Pressure(Max) :  %4d",&saveCycleData.maxPressure[6]);
						}
				        else if(strncmp(line, "6.Pressure(Min)", 15) == 0) {
							sscanf(line, "6.Pressure(Min) :  %4d",&saveCycleData.minPressure[6]);
						}
				        else if(strncmp(line, "CARBON FILTER", 13) == 0) {
							sscanf(line, "CARBON FILTER :  %04d",&saveCycleData.carbonFilter);
						}
				        else if(strncmp(line, "HEPA FILTER", 11) == 0) {
							sscanf(line, "HEPA FILTER   :  %04d",&saveCycleData.hepaFilter);
						}
				        else if(strncmp(line, "PLASMA ASSY", 11) == 0) {
							sscanf(line, "PLASMA ASSY   :  %04d",&saveCycleData.plasmaAssy);
						}
				        else if(strncmp(line, "Cycle Time", 10) == 0) {
							sscanf(line, "Cycle Time    :  %02d:%02d",
					                   &saveCycleData.totalTime[0],
					                   &saveCycleData.totalTime[1]);
						}
				        else if(strncmp(line, "Cycle Status", 12) == 0) {
							sscanf(line, "Cycle Status  :  %02d",&saveCycleData.status);
						}
				        else if(strncmp(line, "Operator ID", 11) == 0) {
							sscanf(line, "Operator ID   :  %02d",&saveCycleData.ID);
						}
				    }

				// 파일 닫기
				f_close(&USBHFile);
			}
			// USB 메모리 마운트 해제
			f_mount(NULL, USBHPath, 0);
		}
	}
}


/*
//아래는 Raw 데이터 가공 입력해야함.
for(int i=0;i<270;i++){
	TemperatureData[i+1]*10)/100);
	PressureData[i+1]*10)/100);

}
*/


void SaveCycleSettingData(CycleUSBData* data) {
    char filename[20]; // 파일 이름을 저장할 배열
    sprintf(filename, "Cycle_Setting.txt");
    // USB 메모리가 준비된 상태인지 확인
    if(Appli_state == APPLICATION_READY){
        // USB 메모리를 마운트
        USBret = f_mount(&USBHFatFS, USBHPath, 1);
        if(USBret == FR_OK) {
            // 파일 생성 및 열기
            USBret = f_open(&USBHFile, filename, FA_CREATE_ALWAYS | FA_WRITE);
            if(USBret == FR_OK) {
               // f_printf(&USBHFile,"Date: %d-%d-%d\nProcess Name: %d\n", data->year, data->month, data->day, data->processName);
                // 파일 닫기
                f_close(&USBHFile);
            }
            // USB 메모리 마운트 해제
            f_mount(NULL, USBHPath, 0);
        }
    }
}
void LoadSettingData(CycleUSBData* data) {
    char filename[20]; // 파일 이름을 저장할 배열
    //int file_number = 1; // 파일 번호 (예시로 1을 사용)

	sprintf(filename, "Setting.txt");
    // USB 메모리가 준비된 상태인지 확인
    if(Appli_state == APPLICATION_READY){
        // USB 메모리를 마운트
        USBret = f_mount(&USBHFatFS, USBHPath, 1);
        if(USBret == FR_OK) {
            // 파일 열기
            USBret = f_open(&USBHFile, filename, FA_READ);
            if(USBret == FR_OK) {
                // 데이터 불러오기를 위한 코드
                char buffer[100]; // 파일로부터 읽은 데이터를 저장하기 위한 버퍼
                UINT bytesRead; // 실제로 읽은 바이트 수를 저장
                f_read(&USBHFile, buffer, sizeof(buffer)-1, &bytesRead);
                buffer[bytesRead] = '\0'; // 문자열 종료를 위해 NULL 문자 추가

                // 읽은 데이터를 ProcessData 구조체에 파싱
                sscanf(buffer, "Date: %d-%d-%d\n", &data->date[0], &data->date[1], &data->date[2]);

                // 파일 닫기
                f_close(&USBHFile);
            }
            // USB 메모리 마운트 해제
            f_mount(NULL, USBHPath, 0);
        }
    }
}

void sortNonZeroElementsKeepFirstZero(int arr[], int n) {
    int i, j, temp;

    // 배열의 첫 번째 요소를 제외하고 비-제로 요소를 찾아 정렬합니다.
    // 시작 인덱스를 1로 설정합니다.
    for (i = 1; i < n-1; i++) {
        for (j = i+1; j < n; j++) {
            // 두 요소가 비-제로이고 첫 번째 요소가 두 번째 요소보다 클 경우
            if ((arr[i] != 0 && arr[j] != 0) && (arr[i] > arr[j])) {
                // 두 요소의 위치를 바꿉니다.
                temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
    }

    // 제로 요소들을 배열의 끝으로 이동시킵니다, 첫 번째 요소는 제외합니다.
    int zeroCount = 0; // 제로 요소의 개수를 세기
    for (i = 1; i < n; i++) { // 첫 번째 요소를 제외하고 시작
        if (arr[i] == 0) {
            zeroCount++;
        } else if (zeroCount > 0) {
            arr[i - zeroCount] = arr[i];
            arr[i] = 0;
        }
    }
}

#define MAX_FILES   100 // 최대 파일 수
#define MAX_FILENAME 20 // 최대 파일 이름 길이
char fileList[MAX_FILES][MAX_FILENAME]; // 파일 이름을 저장할 배열
int fileCount; // 파일 개수

void ReadListData(int year, int month, int day){
    DIR dir; // 디렉토리 객체
    FILINFO fno; // 파일 정보 객체
    FRESULT res; // 결과 저장 변수
    fileCount = 0; // 파일 개수
    int fileYear, fileMonth, fileDay, fileNum; // 파일에서 추출한 날짜 및 숫자

	if(Appli_state == APPLICATION_READY){
		USBret=f_mount(&USBHFatFS, USBHPath, 1);
		if (f_opendir(&dir, USBHPath) == FR_OK) { // USB 메모리의 루트 디렉토리를 열기
            while (fileCount < MAX_FILES) {
                // 디렉토리 읽기
				res = f_readdir(&dir, &fno);
				if (res != FR_OK || fno.fname[0] == 0) break; // 더 이상 읽을 파일이 없으면 종료
					if (!(fno.fattrib & AM_DIR)) { // 파일인 경우
						// 파일 이름에서 날짜 정보 추출
						if (sscanf(fno.fname, "%02d-%02d-%02d_%03d.txt", &fileYear, &fileMonth, &fileDay, &fileNum) == 4) {
							// 주어진 날짜와 일치하는지 확인
							if (fileYear == year && fileMonth == month && fileDay == day) {
								printf("MATCHED FILE: %s\n", fno.fname);
								strncpy(fileList[fileCount], fno.fname, MAX_FILENAME-1);
								fileList[fileCount][MAX_FILENAME-1] = '\0';
								fileCount++;
								temptotalcycle[fileCount]=fileNum;
							}
						}
					}
            	}
			// 디렉토리 닫기
			f_closedir(&dir);
		}
		f_mount(NULL, USBHPath, 0);
	}
    int n = sizeof(temptotalcycle) / sizeof(temptotalcycle[0]);

    sortNonZeroElementsKeepFirstZero(temptotalcycle, n);
}



void USB_Error_Handler(void)
{
  /* USER CODE BEGIN USB_Error_Handler */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END USB_Error_Handler */
}
