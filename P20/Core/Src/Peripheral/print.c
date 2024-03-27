/*
 * print.c
 *
 *  Created on: Nov 1, 2022
 *      Author: CBT
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "lcd.h"
#include "process.h"
#include "hardware.h"

extern UART_HandleTypeDef huart2;
#define PRINT_USART	&huart2


unsigned char PRINT_rx_data[200]={};


#define	LF	0x0A;
#define ESC	0x1B;
#define J	0x4A;
#define d	0x64;
#define FF	0x0C;

//init Printer
//ESC	@

char pinrtdata[40];


void initprint(){
	PRINT_rx_data[0]=ESC;
	PRINT_rx_data[1]=0x40;
    HAL_UART_Transmit(PRINT_USART, PRINT_rx_data, 2, 10);
}

void print_pagemode(){
	PRINT_rx_data[0]=ESC;
	PRINT_rx_data[1]=FF;
    HAL_UART_Transmit(PRINT_USART, PRINT_rx_data, 2, 10);
}

void print_standardmode(){
	PRINT_rx_data[0]=FF;
    HAL_UART_Transmit(PRINT_USART, PRINT_rx_data, 1, 10);
}

void print_printnfeed(){
	PRINT_rx_data[0]=LF;
    HAL_UART_Transmit(PRINT_USART, PRINT_rx_data, 1, 10);
}

void printline(){
	//initprint();
	for(int i=0;i<32;i++){
		PRINT_rx_data[i]='-';
	}
	//PRINT_rx_data[32]=FF;
	HAL_UART_Transmit(PRINT_USART, PRINT_rx_data, 32, 10);
}

void printmsg(char *msg){
	//initprint();
	memset(PRINT_rx_data, 0, 50);
	for(int i = 0; i < 33; i++) {
		PRINT_rx_data[i] = msg[i];
	}
	HAL_UART_Transmit(PRINT_USART, PRINT_rx_data, sizeof(PRINT_rx_data), 10);
	memset(PRINT_rx_data, 0, 50);
}

void printmsg100(char *msg){
	//initprint();
	memset(PRINT_rx_data, 0, 200);
	for(int i = 0; i <strlen(msg) ; i++) {
		PRINT_rx_data[i] = msg[i];
	}
	HAL_UART_Transmit(PRINT_USART, PRINT_rx_data, strlen(msg), 10);
	memset(PRINT_rx_data, 0, 200);
}

void doprint(){
	PRINT_rx_data[0]=FF;
	HAL_UART_Transmit(PRINT_USART, PRINT_rx_data, 1, 10);
}

void printmain(){
	initprint();
	print_printnfeed();
	printmsg("--------------------------------\n");
	printmsg("Clean Bio Tech Corp             \n");
	printmsg("CYCLE Information               \n");
	printmsg("--------------------------------\n");
	printmsg("MODEL NO      : P1              \n");
	printmsg("STERILIZER NO : FN-P100230101   \n");
	printmsg("FACILITY NAME : SAMSUNG         \n");
	printmsg("DEPART NAME   : DENTAL          \n");
	printmsg("--------------------------------\n");
	printmsg("Sterilant                       \n");
	printmsg("SERIAL NO     : P1-S-230101     \n");
	printmsg("Loading Date  : 2022-11-02      \n");
	printmsg("Expiry Date   : 2022-12-02      \n");
	printmsg("Remain H2O2   : 49              \n");
	printmsg("--------------------------------\n");
    print_printnfeed();
    doprint();
}
