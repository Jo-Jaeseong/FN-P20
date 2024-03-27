/*
 * sd.c
 *
 *  Created on: Mar 25, 2024
 *      Author: CBT_DEV
 */

#include "main.h"
#include "fatfs.h"
#include "sd.h"
#include "stdio.h"

extern uint8_t retSD;    /* Return value for SD */
extern char SDPath[4];   /* SD logical drive path */
extern FATFS SDFatFS;    /* File system object for SD logical drive */
extern FIL SDFile;       /* File object for SD */
FIL MyFile;

FRESULT SDres;

void SaveToSDCard(void) {
	/*
    if(f_mount(&SDFatFS, SDPath, 1) == FR_OK) {
        if(f_open(&MyFile, "SD_FILE.TXT", FA_CREATE_ALWAYS | FA_WRITE) == FR_OK) {
            f_printf(&MyFile, "Hello, SD Card!\n");
            f_close(&MyFile);
        }
        f_mount(NULL, SDPath, 1);
    }
    */

	SDres=f_mount(&SDFatFS, SDPath, 0);
    if(SDres == FR_OK) {
    	SDres=f_open(&SDFile, "SD_FILE.TXT", FA_CREATE_ALWAYS | FA_WRITE);
        if(SDres == FR_OK) {
            f_printf(&SDFile, "Hello, SD Card!\n");
            f_close(&SDFile);
        }
        f_mount(NULL, SDPath, 1);
    }

}
