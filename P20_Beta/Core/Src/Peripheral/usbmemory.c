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

ProcessData myProcessData = {24, 4, 8, 1, "09:00:00", "11:00:00", 10, 300, 20,
                             {24, 4, 1, 2024, 4, 7, 50, 30},
                             {"120:00", {"10:00", "20:00", "30:00", "40:00", "10:00", "10:00"}},
                             {1000, 2000, 3000},
                             {{100, 200, 150}, {100, 200, 150}, {100, 200, 150},
                              {100, 200, 150}, {100, 200, 150}, {100, 200, 150}},
                             {{100, 200}, {100, 200}}}; // 예시 데이터, 실제 사용 시 적절히 수정 필요


ProcessData SavemyCycleData;

void SaveCycleData(){
    char filename[20]; // 파일 이름을 저장할 배열

    GetStartTime();
    GetEndTime();

    sprintf(filename, "%02d.%02d.%02d_", bcd2bin(p_data.year), bcd2bin(p_data.month), bcd2bin(p_data.day));
    sprintf(filename + strlen(filename), "%03d.txt", totalCount);
	// USB 메모리가 준비된 상태인지 확인
	if(Appli_state == APPLICATION_READY){
		// USB 메모리를 마운트
		USBret = f_mount(&USBHFatFS, USBHPath, 1);
		if(USBret == FR_OK) {
			// 파일 생성 및 열기
			USBret = f_open(&USBHFile, filename, FA_CREATE_ALWAYS | FA_WRITE);
			if(USBret == FR_OK) {
				fprintCycelData();
				// 파일 닫기
				f_close(&USBHFile);
			}
			// USB 메모리 마운트 해제
			f_mount(NULL, USBHPath, 0);
		}
	}

	/*
	//아래는 Raw 데이터 가공 입력해야함.
	for(int i=0;i<270;i++){
		write_data[115+4*i]=(int)((TemperatureData[i+1]*10)/100);
		write_data[116+4*i]=((int)(TemperatureData[i+1]*10)%100);

		write_data[117+4*i]=(int)((PressureData[i+1]*10)/100);
		write_data[118+4*i]=((int)(PressureData[i+1]*10)%100);
	}
	*/




	//CycleData.year


	//저장 예시
	//f_printf(&USBHFile,"Date: %d-%d-%d\nProcess Name: %d\n", data->year, data->month, data->day, data->processName);
    //char buffer[100]; // 파일로부터 읽은 데이터를 저장하기 위한 버퍼
    //UINT bytesRead; // 실제로 읽은 바이트 수를 저장
    //f_read(&USBHFile, buffer, sizeof(buffer)-1, &bytesRead);
    //buffer[bytesRead] = '\0'; // 문자열 종료를 위해 NULL 문자 추가
	// 읽은 데이터를 ProcessData 구조체에 파싱
	//예: sscanf(buffer, "Date: %d-%d-%d\nProcess Name: %d\n", &data->year, &data->month, &data->day, &data->processName);

	//불러오기 예시
}

void fprintCycelData(){
	f_printf(&USBHFile,"--------------------------------\n");
	f_printf(&USBHFile,"Clean Bio Tech Corp             \n");
	f_printf(&USBHFile,"CYCLE Information               \n");
	f_printf(&USBHFile,"--------------------------------\n");
	f_printf(&USBHFile,"MODEL NAME    :  %s\n",flash_MODEL_NAME);
	f_printf(&USBHFile,"MODEL NUMBER  :  %s\n",flash_SERIAL_NUMBER);
	f_printf(&USBHFile,"FACILITY NAME :  %s\n",flash_FACILITY_NAME);
	f_printf(&USBHFile,"DEPART_NAME   :  %s\n",flash_DEPARTMENT_NAME);
	//여기까지

	//과수 정보(RFIDData) 저장하고 loadRFIDData로 불러와야함
	f_printf(&USBHFile,"--------------------------------\n");
	f_printf(&USBHFile,"Sterilant                       \n");
	f_printf(&USBHFile,"SERIAL NO     :  %02d%02d%02d%02d\n",(int)RFIDData.production_year,(int)RFIDData.production_month,(int)RFIDData.production_day, (int)RFIDData.production_number);
	f_printf(&USBHFile,"Loading Date  :  20%2d-%02d-%02d\n",(int)RFIDData.open_year,(int)RFIDData.open_month,(int)RFIDData.open_day);
	f_printf(&USBHFile,"Expiry Date   :  %02d\n",SterilantCheckDay-RFIDData.elapsed_days);
	f_printf(&USBHFile,"Remain H2O2   :  %02d\n",RFIDData.volume);

	//사이클 정보 저장하고 불러와야함
	f_printf(&USBHFile,"--------------------------------\n");
	if(CycleName==1){
		f_printf(&USBHFile,"Selected Cycle:  Short          \n");
	}
	else if(CycleName==2){
		f_printf(&USBHFile,"Selected Cycle:  Standard       \n");
	}
	else if(CycleName==3){
		f_printf(&USBHFile,"Selected Cycle:  Advanced       \n");
	}

	f_printf(&USBHFile,"--------------------------------\n");
	f_printf(&USBHFile,"Total Cycle   :  %3d     \n",totalCount);
	f_printf(&USBHFile,"Daily Cycle   :  %3d     \n",dailyCount);
	f_printf(&USBHFile,"DATE          :  20%2d-%02d-%02d\n",
			bcd2bin(p_data.year),bcd2bin(p_data.month),bcd2bin(p_data.day));

	f_printf(&USBHFile,"Start Time    :  %02d:%02d:%02d\n",
			bcd2bin(p_data.start_hour),bcd2bin(p_data.start_minute),bcd2bin(p_data.start_second));
	f_printf(&USBHFile,"End Time      :  %02d:%02d:%02d\n",
				bcd2bin(p_data.end_hour),bcd2bin(p_data.end_minute),bcd2bin(p_data.end_second));
	f_printf(&USBHFile,"--------------------------------\n");



	//여기부터는 데이터
	//공정 데이터 저장하고 불러와야함
	f_printf(&USBHFile,"[ VACUUM 1 ]                    \n");
	int tempminute,tempsecond;
	tempminute=(fProcessTime[1]/100)/60;
	tempsecond=(fProcessTime[1]/100)%60;
	f_printf(&USBHFile,"Total Time    :  %02d:%02d      \n",tempminute,tempsecond);
	if((int)p_data.tempmax[1]==0){
		f_printf(&USBHFile,"Temp(Max)     :                 \n");
	}
	else{
		//f_printf(&USBHFile,"Temp(Max)     :  %3d   \n",p_data.tempmax[1]);
		f_printf(&USBHFile,"Temp(Max)     :  %3d   \n",(int)p_data.tempmax[1]);
	}
	if((int)p_data.pressuremax[1]==0){
		f_printf(&USBHFile,"Pressure(Max) :                 \n");
	}
	else{
		//f_printf(&USBHFile,"Pressure(Max) :  %4.1f   \n",(int)(int)p_data.pressuremax[1]);
		f_printf(&USBHFile,"Pressure(Max) :  %4d   \n",(int)p_data.pressuremax[1]);
	}
	if((int)p_data.pressuremin[1]==760){
		f_printf(&USBHFile,"Pressure(Min) :                 \n");
	}
	else{
		f_printf(&USBHFile,"Pressure(Min) :  %4d   \n",(int)p_data.pressuremin[1]);
	}
	f_printf(&USBHFile,"[ INJECTION 1 & DIFFUSION 1 ]   \n");
	tempminute=(fProcessTime[2]/100)/60;
	tempsecond=(fProcessTime[2]/100)%60;
	f_printf(&USBHFile,"Total Time    :  %02d:%02d      \n",tempminute,tempsecond);
	if((int)p_data.tempmax[2]==0){
		f_printf(&USBHFile,"Temp(Max)     :                 \n");
	}
	else{

		f_printf(&USBHFile,"Temp(Max)     :  %3d   \n",(int)p_data.tempmax[2]);

	}
	if((int)p_data.pressuremax[2]==0){
		f_printf(&USBHFile,"Pressure(Max) :                 \n");
	}
	else{

		f_printf(&USBHFile,"Pressure(Max) :  %4d   \n",(int)p_data.pressuremax[2]);

	}
	if((int)p_data.pressuremin[2]==760){
		f_printf(&USBHFile,"Pressure(Min) :                 \n");
	}
	else{

		f_printf(&USBHFile,"Pressure(Min) :  %4d   \n",(int)p_data.pressuremin[2]);

	}
	f_printf(&USBHFile,"[ VACUUM 2 & PLASMA 1 ]         \n");
	tempminute=(fProcessTime[3]/100)/60;
	tempsecond=(fProcessTime[3]/100)%60;
	f_printf(&USBHFile,"Total Time    :  %02d:%02d      \n",tempminute,tempsecond);
	if((int)p_data.tempmax[3]==0){
		f_printf(&USBHFile,"Temp(Max)     :                 \n");
	}
	else{
		f_printf(&USBHFile,"Temp(Max)     :  %3d   \n",(int)p_data.tempmax[3]);
	}
	if((int)p_data.pressuremax[3]==0){
		f_printf(&USBHFile,"Pressure(Max) :                 \n");
	}
	else{
		f_printf(&USBHFile,"Pressure(Max) :  %4d   \n",(int)p_data.pressuremax[3]);
	}
	if((int)p_data.pressuremin[3]==760){
		f_printf(&USBHFile,"Pressure(Min) :                 \n");
	}
	else{
		f_printf(&USBHFile,"Pressure(Min) :  %4d   \n",(int)p_data.pressuremin[3]);
	}
	f_printf(&USBHFile,"[ INJECTION 2 & DIFFUSION 2 ]   \n");
	tempminute=(fProcessTime[4]/100)/60;
	tempsecond=(fProcessTime[4]/100)%60;
	f_printf(&USBHFile,"Total Time    :  %02d:%02d      \n",tempminute,tempsecond);
	if((int)p_data.tempmax[4]==0){
		f_printf(&USBHFile,"Temp(Max)     :                 \n");
	}
	else{
		f_printf(&USBHFile,"Temp(Max)     :  %3d   \n",(int)p_data.tempmax[4]);
	}
	if((int)p_data.pressuremax[4]==0){
		f_printf(&USBHFile,"Pressure(Max) :                 \n");
	}
	else{
		f_printf(&USBHFile,"Pressure(Max) :  %4d   \n",(int)p_data.pressuremax[4]);
	}
	if((int)p_data.pressuremin[4]==760){
		f_printf(&USBHFile,"Pressure(Min) :                 \n");
	}
	else{
		f_printf(&USBHFile,"Pressure(Min) :  %4d   \n",(int)p_data.pressuremin[4]);
	}

	f_printf(&USBHFile,"[ VACUUM 3 & PLASMA 2 ]         \n");
	tempminute=(fProcessTime[5]/100)/60;
	tempsecond=(fProcessTime[5]/100)%60;
	f_printf(&USBHFile,"Total Time    :  %02d:%02d      \n",tempminute,tempsecond);
	if((int)p_data.tempmax[5]==0){
		f_printf(&USBHFile,"Temp(Max)     :                 \n");
	}
	else{
		f_printf(&USBHFile,"Temp(Max)     :  %3d   \n",(int)p_data.tempmax[5]);
	}
	if((int)p_data.pressuremax[5]==0){
		f_printf(&USBHFile,"Pressure(Max) :                 \n");
	}
	else{
		f_printf(&USBHFile,"Pressure(Max) :  %4d   \n",(int)p_data.pressuremax[5]);
	}
	if((int)p_data.pressuremin[5]==760){
		f_printf(&USBHFile,"Pressure(Min) :                 \n");
	}
	else{
		f_printf(&USBHFile,"Pressure(Min) :  %4d   \n",(int)p_data.pressuremin[5]);
	}
	f_printf(&USBHFile,"[ VENT 1 ]                      \n");
	tempminute=(fProcessTime[6]/100)/60;
	tempsecond=(fProcessTime[6]/100)%60;

	f_printf(&USBHFile,"Total Time    :  %02d:%02d      \n",tempminute,tempsecond);
	if((int)p_data.tempmax[6]==0){
		f_printf(&USBHFile,"Temp(Max)     :                 \n");
	}
	else{
		f_printf(&USBHFile,"Temp(Max)     :  %3d   \n",(int)p_data.tempmax[6]);
	}
	if((int)p_data.pressuremax[6]==0){
		f_printf(&USBHFile,"Pressure(Max) :                 \n");
	}
	else{
		f_printf(&USBHFile,"Pressure(Max) :  %4d   \n",(int)p_data.pressuremax[6]);
	}
	if((int)p_data.pressuremin[6]==760){
		f_printf(&USBHFile,"Pressure(Min) :                 \n");
	}
	else{
		f_printf(&USBHFile,"Pressure(Min) :  %4d   \n",(int)p_data.pressuremin[6]);
	}
	//여기까지 데이터

	//PM 데이터 저장하고 불러와야함.
	f_printf(&USBHFile,"--------------------------------\n");
	f_printf(&USBHFile,"CARBON FILTER :  %04d     \n",CarbonFilter);
	f_printf(&USBHFile,"HEPA FILTER   :  %04d     \n",HEPAFilter);
	f_printf(&USBHFile,"PLASMA ASSY   :  %04d     \n",PlasmaAssy);
	f_printf(&USBHFile,"--------------------------------\n");

	tempminute=(TotalTime/100)/60;
	tempsecond=(TotalTime/100)%60;
	f_printf(&USBHFile,"Cycle Time    :  %02d:%02d      \n",tempminute,tempsecond);
	if(errorcode==0){
		f_printf(&USBHFile,"Cycle Status  :  Completed      \n");
	}
	else{
		f_printf(&USBHFile,"Cycle Status  :  ERROR%02d        \n",errorcode);
	}
	f_printf(&USBHFile,"Operator ID   :  %c%c%c%c%c%c%c%c%c%c\n",flash_ID[CurrentUser][0],flash_ID[CurrentUser][1],flash_ID[CurrentUser][2],
			flash_ID[CurrentUser][3],flash_ID[CurrentUser][4],flash_ID[CurrentUser][5],flash_ID[CurrentUser][6],flash_ID[CurrentUser][7],flash_ID[CurrentUser][8],flash_ID[CurrentUser][9]);
	//f_printf(&USBHFile,"Approved by   :                 \n");
}

void SaveSettingData(ProcessData* data) {
    char filename[20]; // 파일 이름을 저장할 배열
    sprintf(filename, "Setting.txt");
    // USB 메모리가 준비된 상태인지 확인
    if(Appli_state == APPLICATION_READY){
        // USB 메모리를 마운트
        USBret = f_mount(&USBHFatFS, USBHPath, 1);
        if(USBret == FR_OK) {
            // 파일 생성 및 열기
            USBret = f_open(&USBHFile, filename, FA_CREATE_ALWAYS | FA_WRITE);
            if(USBret == FR_OK) {
                f_printf(&USBHFile,"Date: %d-%d-%d\nProcess Name: %d\n", data->year, data->month, data->day, data->processName);
                // 파일 닫기
                f_close(&USBHFile);
            }
            // USB 메모리 마운트 해제
            f_mount(NULL, USBHPath, 0);
        }
    }
}

void LoadCycleData(ProcessData* data) {
    char filename[20]; // 파일 이름을 저장할 배열
    int file_number = 1; // 파일 번호 (예시로 1을 사용)

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
                예: sscanf(buffer, "Date: %d-%d-%d\nProcess Name: %d\n", &data->year, &data->month, &data->day, &data->processName);

                // 파일 닫기
                f_close(&USBHFile);
            }
            // USB 메모리 마운트 해제
            f_mount(NULL, USBHPath, 0);
        }
    }
}

void SaveCycleSettingData(ProcessData* data) {
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
void LoadSettingData(ProcessData* data) {
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
                예: sscanf(buffer, "Date: %d-%d-%d\nProcess Name: %d\n", &data->year, &data->month, &data->day, &data->processName);

                // 파일 닫기
                f_close(&USBHFile);
            }
            // USB 메모리 마운트 해제
            f_mount(NULL, USBHPath, 0);
        }
    }
}


void ReadListData(void){
	if(Appli_state == APPLICATION_READY){
		USBret=f_mount(&USBHFatFS, USBHPath, 1);
		if (f_opendir(&dir, USBHPath) == FR_OK) { // USB 메모리의 루트 디렉토리를 열기
			while (1) {
				// 디렉토리 읽기
				if (f_readdir(&dir, &fno) != FR_OK || fno.fname[0] == 0) break; // 더 이상 읽을 파일이 없으면 종료
				if (fno.fattrib & AM_DIR) { // 디렉토리인 경우
					printf("DIR : %s\n", fno.fname);
				} else { // 파일인 경우
					printf("FILE: %s\n", fno.fname);
				}
			}
			// 디렉토리 닫기
			f_closedir(&dir);
		}
		f_mount(NULL, USBHPath, 0);
	}
}






void USB_Error_Handler(void)
{
  /* USER CODE BEGIN USB_Error_Handler */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END USB_Error_Handler */
}
