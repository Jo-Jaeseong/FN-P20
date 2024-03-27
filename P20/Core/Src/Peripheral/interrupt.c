/*
 * interrupt.c
 *
 *  Created on: Aug 10, 2022
 *      Author: CBT
 */

#include "main.h"
#include "Process.h"

unsigned int	mscounter;

unsigned char Timer_DeliSecond_Flag, Timer_Half_1s_Flag, Timer_1s_Flag;

unsigned char UART_Receive_Flag;

extern unsigned char LCD_rx_data[30];
extern UART_HandleTypeDef huart1;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM7) {
		mscounter++;
		if((mscounter % 10) == 0) {

		}
		if((mscounter % 100) == 0) {
			Timer_DeliSecond_Flag = 1;
		}
		if((mscounter % 500) == 0){
			Timer_Half_1s_Flag = 1;
		}
		if(mscounter == 1000) {
			mscounter = 0;
			Timer_1s_Flag = 1;
		}
	}
}



void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)
    {
    	UART_Receive_Flag = 1;
    	// Restart Interrupt.
    	//HAL_UART_Receive_IT(&huart1, (uint8_t*)LCD_rx_data, 9);

    }
    else if(huart->Instance == USART6)
    {

    	//
    	//HAL_UART_Receive_IT(&huart1, (uint8_t*)LCD_rx_data, 9);

    }

}

