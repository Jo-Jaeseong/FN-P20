/*
 * Process.c
 *
 *  Created on: Aug 10, 2022
 *      Author: CBT
 */
#include "main.h"

#include "hardware.h"
#include "sensor.h"

#include "process.h"
#include "lcd.h"
#include "pm.h"

extern unsigned char Timer_DeliSecond_Flag, Timer_Half_1s_Flag, Timer_1s_Flag;
extern unsigned char UART_Receive_Flag;

int TenSecondCount=0;
int testcount=0;
int tempcnt=0;
extern int Pressure, Pressure2;

extern uint32_t uiDensity, adcData[10];

void loop(){
	if(UART_Receive_Flag) {
		UART_Receive_Flag = 0;
		LCD_Process();
	}
	if(Timer_DeliSecond_Flag) {
	  Timer_DeliSecond_Flag=0;
	  DeliSecondProcess();
	}
	if(Timer_Half_1s_Flag){
	  Timer_Half_1s_Flag=0;
	  HalfSecondProcess();
	}
	if(Timer_1s_Flag) {
	  Timer_1s_Flag=0;

	  OneSecondProcess();
	}
}

int data1 = 0;

void DeliSecondProcess(void){

	GetValue();
	DisplayVacuumSensor();
	data1 *= .9;
	data1 += Pressure2;
	printf("max:4095, min:3500, data1:%d, data2:%d, data3:%d  \n",data1/10, Pressure, Pressure2);

}

void HalfSecondProcess(void){
	DisplayIcon();
	Check_Temp(tempcnt);
	if(tempcnt>=3){
		tempcnt=0;
	}
	else{
		tempcnt++;
	}
	DisplayTemprature();
}

void OneSecondProcess(void){
	TenSecondCount++;
	if(TenSecondCount==10){
		TenSecondProcess();
		TenSecondCount=0;
	}


}

void TenSecondProcess(void){

}

void Init_Device(){

}

void Inithardware(){

}
