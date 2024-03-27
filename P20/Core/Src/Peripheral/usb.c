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


#include "main.h"
#include "fatfs.h"
#include "ff.h"
#include "usb_host.h"
#include "flash.h"
#include "lcd.h"
#include "Process.h"


extern uint8_t retUSBH;    /* Return value for USBH */
extern char USBHPath[4];   /* USBH logical drive path */
extern FATFS USBHFatFS;    /* File system object for USBH logical drive */
extern FIL USBHFile;       /* File object for USBH */

extern USBH_HandleTypeDef hUsbHostFS;
extern ApplicationTypeDef Appli_state;

FRESULT USBres;

void USB_Error_Handler(void)
{
  /* USER CODE BEGIN USB_Error_Handler */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END USB_Error_Handler */
}

void SaveToUSBMemory(void) {
	USBres=f_mount(&USBHFatFS, USBHPath, 0);;
    if( USBres == FR_OK) {
    	USBres=f_open(&USBHFile, "USB_FILE.TXT", FA_CREATE_ALWAYS | FA_WRITE);
        if(USBres == FR_OK) {
            f_printf(&USBHFile, "Hello, USB Memory!\n");
            f_close(&USBHFile);
        }
        f_mount(NULL, USBHPath, 1);
    }
}


