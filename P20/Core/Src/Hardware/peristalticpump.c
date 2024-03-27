/*
 * PeristalticPump.c
 *
 *  Created on: Oct 13, 2022
 *      Author: CBT
 */

#include "process.h"
#include "main.h"
#include "hardware.h"

extern TIM_HandleTypeDef htim4;
#define PERI_PWM	&htim4
#define PERI_PWM_CH	TIM_CHANNEL_3



void TurnOnPeristalticPump(void){
	// PeristalticPump On.
	/*
	if(LevelSensorCheck()){

	}
	else{
		HAL_TIM_PWM_Start(PERI_PWM, PERI_PWM_CH);  //startup PeristalticPumpPwm
	}
	*/
	//HAL_TIM_PWM_Start(PERI_PWM, PERI_PWM_CH);  //startup PeristalticPumpPwm
	HAL_GPIO_WritePin(GPIO_OUT26_GPIO_Port, GPIO_OUT26_Pin, GPIO_PIN_SET);

}

void TurnOffPeristalticPump(void){
	//HAL_TIM_PWM_Stop(PERI_PWM, PERI_PWM_CH);  //Stop PeristalticPumpPwm
	HAL_GPIO_WritePin(GPIO_OUT26_GPIO_Port, GPIO_OUT26_Pin, GPIO_PIN_RESET);

}


void PeristalticSpeed(){	// 분당 분사량 조절
	TIM5->CNT=0;
	TIM5->ARR = 20000;
/*
	//2Channel JIHPUMP
	if(fInjectionPerMinute==15){
		TIM5->ARR = DeviceInfo.Peri1_15_Value;
	}
		//20100
	else if(fInjectionPerMinute==12){
		TIM5->ARR = 20000;
	}
	else if(fInjectionPerMinute==9){
		TIM5->ARR = DeviceInfo.Peri1_9_Value;
	}
	else{
		TIM5->ARR = DeviceInfo.Peri1_12_Value;
	}
	*/
	HAL_TIM_PWM_Start(PERI_PWM, PERI_PWM_CH);  //startup PeristalticPumpPwm
}

void PeriPump(int onoff){
	if(onoff==1){
		TurnOnPeristalticPump();
	}
	else{
		TurnOffPeristalticPump();
	}
}
