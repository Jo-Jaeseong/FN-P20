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

void USB_Error_Handler(void)
{
  /* USER CODE BEGIN USB_Error_Handler */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END USB_Error_Handler */
}

void SD_Error_Handler(void)
{
  /* USER CODE BEGIN SD_Error_Handler */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END SD_Error_Handler */
}

void SaveToSDMemory(void) {

}


void SaveToUSBMemory(void) {
	if(Appli_state == APPLICATION_READY){
		if(firstcall){
			USBret=f_mount(&USBHFatFS, USBHPath, 1);
			if( USBret == FR_OK) {
				USBret=f_open(&USBHFile, "USBFILE.TXT", FA_CREATE_ALWAYS | FA_WRITE);
				if(USBret == FR_OK) {
					f_printf(&USBHFile, "Hello, USB Memory!\n");
					f_close(&USBHFile);
				}
			}
			firstcall=0;
			f_mount(NULL, USBHPath, 0);
		}
	}
}


void ReadFromUSBMemory(void) {
	if(Appli_state == APPLICATION_READY){
		USBret=f_mount(&USBHFatFS, USBHPath, 1);
		USBret=f_open(&USBHFile, "USBFILE.TXT", FA_READ);
		if (USBret == FR_OK) {
			// 파일 읽기
			UINT bytesRead;
			f_read(&USBHFile, buffer, sizeof(buffer)-1, &bytesRead);
			buffer[bytesRead] = '\0'; // 문자열 종료를 위해 NULL 추가

			// 읽어온 내용 처리 (예: 화면에 출력)
			//printf("%s", buffer);

			// 파일 닫기
			f_close(&USBHFile);
		} else {
			// 파일 열기 실패 처리
			//printf("Failed to open file\n");
		}
		f_mount(NULL, USBHPath, 0);
	}
}

void ReadListFromUSBMemory(void) {
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
