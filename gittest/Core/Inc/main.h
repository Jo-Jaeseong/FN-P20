/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define GPIO_OUT3_Pin GPIO_PIN_2
#define GPIO_OUT3_GPIO_Port GPIOE
#define GPIO_OUT4_Pin GPIO_PIN_3
#define GPIO_OUT4_GPIO_Port GPIOE
#define GPIO_OUT5_Pin GPIO_PIN_4
#define GPIO_OUT5_GPIO_Port GPIOE
#define GPIO_OUT6_Pin GPIO_PIN_5
#define GPIO_OUT6_GPIO_Port GPIOE
#define GPIO_OUT7_Pin GPIO_PIN_6
#define GPIO_OUT7_GPIO_Port GPIOE
#define GPIO_OUT8_Pin GPIO_PIN_13
#define GPIO_OUT8_GPIO_Port GPIOC
#define GPIO_IN1_Pin GPIO_PIN_0
#define GPIO_IN1_GPIO_Port GPIOC
#define GPIO_IN2_Pin GPIO_PIN_1
#define GPIO_IN2_GPIO_Port GPIOC
#define GPIO_IN3_Pin GPIO_PIN_2
#define GPIO_IN3_GPIO_Port GPIOC
#define GPIO_IN4_Pin GPIO_PIN_3
#define GPIO_IN4_GPIO_Port GPIOC
#define ADC_IN0_Pin GPIO_PIN_0
#define ADC_IN0_GPIO_Port GPIOA
#define ADC_IN1_Pin GPIO_PIN_1
#define ADC_IN1_GPIO_Port GPIOA
#define ADC_IN2_Pin GPIO_PIN_2
#define ADC_IN2_GPIO_Port GPIOA
#define LED_GR_Pin GPIO_PIN_3
#define LED_GR_GPIO_Port GPIOA
#define GPIO_OUT24_Pin GPIO_PIN_4
#define GPIO_OUT24_GPIO_Port GPIOA
#define SPI1_SCK_Pin GPIO_PIN_5
#define SPI1_SCK_GPIO_Port GPIOA
#define GPIO_OUT25_Pin GPIO_PIN_6
#define GPIO_OUT25_GPIO_Port GPIOA
#define GPIO_IN5_Pin GPIO_PIN_4
#define GPIO_IN5_GPIO_Port GPIOC
#define GPIO_IN6_Pin GPIO_PIN_5
#define GPIO_IN6_GPIO_Port GPIOC
#define GPIO_IN7_Pin GPIO_PIN_0
#define GPIO_IN7_GPIO_Port GPIOB
#define GPIO_OUT18_Pin GPIO_PIN_1
#define GPIO_OUT18_GPIO_Port GPIOB
#define GPIO_OUT17_Pin GPIO_PIN_7
#define GPIO_OUT17_GPIO_Port GPIOE
#define GPIO_OUT16_Pin GPIO_PIN_8
#define GPIO_OUT16_GPIO_Port GPIOE
#define GPIO_OUT15_Pin GPIO_PIN_9
#define GPIO_OUT15_GPIO_Port GPIOE
#define GPIO_OUT14_Pin GPIO_PIN_10
#define GPIO_OUT14_GPIO_Port GPIOE
#define GPIO_OUT13_Pin GPIO_PIN_11
#define GPIO_OUT13_GPIO_Port GPIOE
#define GPIO_OUT12_Pin GPIO_PIN_12
#define GPIO_OUT12_GPIO_Port GPIOE
#define GPIO_OUT11_Pin GPIO_PIN_13
#define GPIO_OUT11_GPIO_Port GPIOE
#define GPIO_OUT23_Pin GPIO_PIN_14
#define GPIO_OUT23_GPIO_Port GPIOE
#define GPIO_OUT22_Pin GPIO_PIN_15
#define GPIO_OUT22_GPIO_Port GPIOE
#define GPIO_OUT21_Pin GPIO_PIN_10
#define GPIO_OUT21_GPIO_Port GPIOB
#define GPIO_OUT20_Pin GPIO_PIN_11
#define GPIO_OUT20_GPIO_Port GPIOB
#define GPIO_OUT19_Pin GPIO_PIN_12
#define GPIO_OUT19_GPIO_Port GPIOB
#define SPI2_SCK_Pin GPIO_PIN_13
#define SPI2_SCK_GPIO_Port GPIOB
#define SPI2_MISO_Pin GPIO_PIN_14
#define SPI2_MISO_GPIO_Port GPIOB
#define SPI2_MOSI_Pin GPIO_PIN_15
#define SPI2_MOSI_GPIO_Port GPIOB
#define USART3_TX_Pin GPIO_PIN_8
#define USART3_TX_GPIO_Port GPIOD
#define USART3_RX_Pin GPIO_PIN_9
#define USART3_RX_GPIO_Port GPIOD
#define Detect_SDIO_Pin GPIO_PIN_10
#define Detect_SDIO_GPIO_Port GPIOD
#define USB_OUT_Pin GPIO_PIN_11
#define USB_OUT_GPIO_Port GPIOD
#define UART6_TX_Pin GPIO_PIN_6
#define UART6_TX_GPIO_Port GPIOC
#define UART6_RX_Pin GPIO_PIN_7
#define UART6_RX_GPIO_Port GPIOC
#define SDIO_D0_Pin GPIO_PIN_8
#define SDIO_D0_GPIO_Port GPIOC
#define SDIO_D1_Pin GPIO_PIN_9
#define SDIO_D1_GPIO_Port GPIOC
#define USART1_TX_Pin GPIO_PIN_9
#define USART1_TX_GPIO_Port GPIOA
#define USART1_RX_Pin GPIO_PIN_10
#define USART1_RX_GPIO_Port GPIOA
#define USB_DM_Pin GPIO_PIN_11
#define USB_DM_GPIO_Port GPIOA
#define USB_DP_Pin GPIO_PIN_12
#define USB_DP_GPIO_Port GPIOA
#define F_CS_Pin GPIO_PIN_15
#define F_CS_GPIO_Port GPIOA
#define SDIO_D2_Pin GPIO_PIN_10
#define SDIO_D2_GPIO_Port GPIOC
#define SDIO_D3_Pin GPIO_PIN_11
#define SDIO_D3_GPIO_Port GPIOC
#define SDIO_CK_Pin GPIO_PIN_12
#define SDIO_CK_GPIO_Port GPIOC
#define SDIO_CMD_Pin GPIO_PIN_2
#define SDIO_CMD_GPIO_Port GPIOD
#define USART2_TX_Pin GPIO_PIN_5
#define USART2_TX_GPIO_Port GPIOD
#define USART2_RX_Pin GPIO_PIN_6
#define USART2_RX_GPIO_Port GPIOD
#define SPI1_MISO_Pin GPIO_PIN_4
#define SPI1_MISO_GPIO_Port GPIOB
#define SPI1_MOSI_Pin GPIO_PIN_5
#define SPI1_MOSI_GPIO_Port GPIOB
#define GPIO_OUT26_Pin GPIO_PIN_7
#define GPIO_OUT26_GPIO_Port GPIOB
#define PWM1_Pin GPIO_PIN_8
#define PWM1_GPIO_Port GPIOB
#define PWM2_Pin GPIO_PIN_9
#define PWM2_GPIO_Port GPIOB
#define GPIO_OUT1_Pin GPIO_PIN_0
#define GPIO_OUT1_GPIO_Port GPIOE
#define GPIO_OUT2_Pin GPIO_PIN_1
#define GPIO_OUT2_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
