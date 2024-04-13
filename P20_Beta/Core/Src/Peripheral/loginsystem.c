/*
 * login_system.c
 *
 *  Created on: 2023. 3. 21.
 *      Author: CBT
 */


#include <loginsystem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "lcd.h"
#include "process.h"
#include "flash.h"

extern UART_HandleTypeDef huart1;
#define LCD_USART	&huart1

#define MAX_USERS 5
#define MAX_ID_PW_LENGTH 10


char inputCreateID[10];
char inputCreatePW[10];
char inputCreatePW2[10];


unsigned char master_ID[10]="CBT";
unsigned char master_PW[10]="1234";

unsigned char admin_ID[10]="ADMIN";
unsigned char admin_PW[10]="1234";

char inputloginID[10];
char inputloginPW[10];

char inputManagementPW[10];
char inputManagementPW2[10];


unsigned int CurrentUser;
int Select_ID;

//아이디 생성
int createUser() {
	memset(inputCreateID, 0, 10);
	memset(inputCreatePW, 0, 10);
	memset(inputCreatePW2, 0, 10);
	ReadInputCreateIDPWFromLCD();
    if (flashuserCount >= MAX_USERS) {
        printf("최대 사용자 수를 초과했습니다.\n");
        return 2;
    }

    for (int i = 0; i < flashuserCount; i++) {
		if (strcmp(flash_ID[i], inputCreateID) == 0 ) {
			return 3;
		}
	}

    if (strcmp(inputCreatePW2, inputCreatePW) != 0) {
        printf("패스워드가 일치하지 않습니다.\n");
        return 0;
    }

    if((inputCreateID[0]==0)||(inputCreateID[0]==0xff)||(inputCreatePW[0]==0)||(inputCreatePW[0]==0xff)){
        printf("아이디와 패스워드를 입력하시오.\n");
        return 0;
    }
    for(int i=0;i<10;i++){
		flash_ID[flashuserCount][i]=inputCreateID[i];
		flash_PW[flashuserCount][i]=inputCreatePW[i];
    }
    flashuserCount++;

    return 1;
}

//로그인
int loginProcess() {
	ReadInputLoginIDPWFromLCD();
    if((inputloginID[0]==0)||(inputloginPW[0]==0)){
        printf("아이디와 패스워드를 입력하시오.\n");
        return 0;
    }
	if (strcmp(master_ID, inputloginID) == 0 && strcmp(master_PW, inputloginPW) == 0){
		CurrentUser=10;
        return 1;
	}
	else 	if (strcmp(admin_ID, inputloginID) == 0 && strcmp(admin_PW, inputloginPW) == 0){
		CurrentUser=9;
        return 1;
	}

    for (int i = 0; i < flashuserCount; i++) {
        if (strcmp(flash_ID[i], inputloginID) == 0 && strcmp(flash_PW[i], inputloginPW) == 0) {
        	CurrentUser=i;
            return 1;
        }
    }

    return 0;
}

int changePWloginUser(){
	memset(inputManagementPW, 0, 10);
	memset(inputManagementPW2, 0, 10);

	ReadInputManagmentIDPWFromLCD();
    if(strcmp(flash_PW[Select_ID], inputManagementPW) == 0) {
       	return 1;
    }
    else{
    	return 0;
    }
}

int DeletConfirmloginUser(){
	memset(inputManagementPW, 0, 10);
	memset(inputManagementPW2, 0, 10);

	ReadInputManagmentIDPWFromLCD();
    if(strcmp(flash_PW[Select_ID], inputManagementPW) == 0) {
		for (int i = Select_ID; i < 4; i++) {
			strcpy(flash_ID[i], flash_ID[i + 1]);
			strcpy(flash_PW[i], flash_PW[i + 1]);
		}
		strcpy(flash_ID[4], "\0");
		strcpy(flash_PW[4], "\0");
		flashuserCount--;
       	return 1;
    }
    else{
    	return 0;
    }
}

int changePWUser(){
	memset(inputManagementPW, 0, 10);
	memset(inputManagementPW2, 0, 10);

	ReadInputManagmentIDPWFromLCD();
    if (strcmp(inputManagementPW, inputManagementPW2) == 0) {
        for(int i=0;i<10;i++){
        	flash_PW[Select_ID][i]=inputManagementPW[i];
        }
        return 1;
    }
    else{
    	return 0;
    }
}


void ReadInputCreateIDPWFromLCD(){
    __disable_irq();
    huart1.RxState= HAL_UART_STATE_READY;
    unsigned char get_lcd_data[7] = {0x5A, 0xA5, 0x04, 0x83, 0x03, 0x30, 0x05};
    memset(LCD_rx_data, 0, 30);
    get_lcd_data[5]=0x10;//ID
    get_lcd_data[6]=0x05;//Size
    HAL_UART_Transmit(LCD_USART, (uint8_t*)get_lcd_data, 7, 10);
    HAL_UART_Receive(LCD_USART, (uint8_t*)LCD_rx_data, 17, 10);

    for(int i=0;i<10;i++){
    	if(LCD_rx_data[i+7]==0xFF){
    		break;
    	}
    	else{
    		inputCreateID[i]=LCD_rx_data[i+7];
    	}
    }

	get_lcd_data[5]=0x20;//PW
    HAL_UART_Transmit(LCD_USART, (uint8_t*)get_lcd_data, 7, 10);
    HAL_UART_Receive(LCD_USART, (uint8_t*)LCD_rx_data, 17, 10);

    for(int i=0;i<10;i++){
    	if(LCD_rx_data[i+7]==0xFF){
    		break;
    	}
    	else{
    		inputCreatePW[i]=LCD_rx_data[i+7];
    	}
    }

    get_lcd_data[5]=0x30;//PW2
	HAL_UART_Transmit(LCD_USART, (uint8_t*)get_lcd_data, 7, 10);
	HAL_UART_Receive(LCD_USART, (uint8_t*)LCD_rx_data, 17, 10);

    for(int i=0;i<10;i++){
    	if(LCD_rx_data[i+7]==0xFF){
    		break;
    	}
    	else{
    		inputCreatePW2[i]=LCD_rx_data[i+7];
    	}
    }

    UART_Receive_Flag = 0;
    __enable_irq();
    HAL_UART_Receive_IT(LCD_USART, (uint8_t*)LCD_rx_data, 9);
}

void ReadInputLoginIDPWFromLCD(){
    __disable_irq();
    huart1.RxState= HAL_UART_STATE_READY;
    unsigned char get_lcd_data[7] = {0x5A, 0xA5, 0x04, 0x83, 0x02, 0x30, 0x05};
    memset(LCD_rx_data, 0, 30);
    get_lcd_data[5]=0x10;//ID
    get_lcd_data[6]=0x05;//Size
    HAL_UART_Transmit(LCD_USART, (uint8_t*)get_lcd_data, 7, 10);
    HAL_UART_Receive(LCD_USART, (uint8_t*)LCD_rx_data, 17, 10);
    memset(inputloginID,0,10);
    for(int i=0;i<10;i++){
    	if(LCD_rx_data[i+7]==0xFF){
    		break;
    		//inputloginID[i]=0;
    	}
    	else{
    		inputloginID[i]=LCD_rx_data[i+7];
    	}
    }

	get_lcd_data[5]=0x20;//PW
    HAL_UART_Transmit(LCD_USART, (uint8_t*)get_lcd_data, 7, 10);
    HAL_UART_Receive(LCD_USART, (uint8_t*)LCD_rx_data, 17, 10);
    memset(inputloginPW,0,10);
	for(int i=0;i<10;i++){
    	if(LCD_rx_data[i+7]==0xFF){
    		break;
    	}
    	else{
    		inputloginPW[i]=LCD_rx_data[i+7];
    	}
	}

    UART_Receive_Flag = 0;
    __enable_irq();
    HAL_UART_Receive_IT(LCD_USART, (uint8_t*)LCD_rx_data, 9);
}


void ReadInputManagmentIDPWFromLCD(){
    __disable_irq();
    huart1.RxState= HAL_UART_STATE_READY;
    unsigned char get_lcd_data[7] = {0x5A, 0xA5, 0x04, 0x83, 0x04, 0x30, 0x05};
    memset(LCD_rx_data, 0, 30);
    get_lcd_data[5]=0x70;//PW
    get_lcd_data[6]=0x05;//Size
    HAL_UART_Transmit(LCD_USART, (uint8_t*)get_lcd_data, 7, 10);
    HAL_UART_Receive(LCD_USART, (uint8_t*)LCD_rx_data, 17, 10);

	for(int i=0;i<10;i++){
    	if(LCD_rx_data[i+7]==0xFF){
    		break;
    	}
    	else{
    		inputManagementPW[i]=LCD_rx_data[i+7];
    	}
	}

	get_lcd_data[5]=0x80;//PW2
    HAL_UART_Transmit(LCD_USART, (uint8_t*)get_lcd_data, 7, 10);
    HAL_UART_Receive(LCD_USART, (uint8_t*)LCD_rx_data, 17, 10);

	for(int i=0;i<10;i++){
    	if(LCD_rx_data[i+7]==0xFF){
    		break;
    	}
    	else{
    		inputManagementPW2[i]=LCD_rx_data[i+7];
    	}
	}


    UART_Receive_Flag = 0;
    __enable_irq();
    HAL_UART_Receive_IT(LCD_USART, (uint8_t*)LCD_rx_data, 9);
}
