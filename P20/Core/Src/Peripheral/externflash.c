/*
 * extern_flash.c
 *
 *  Created on: Apr 12, 2023
 *      Author: CBT
 */
#include "main.h"

#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


#include "hardware.h"

#include "flash.h"
#include "lcd.h"
#include "rfid.h"
#include "extern_flash.h"

extern SPI_HandleTypeDef hspi1;

#define W25Q128_CS_GPIO_Port	F_CS_GPIO_Port
#define W25Q128_CS_Pin 			F_CS_Pin

// 기본 명령어 정의
#define CMD_WRITE_ENABLE        0x06
#define CMD_READ_STATUS_REG     0x05
#define CMD_WRITE_STATUS_REG    0x01
#define CMD_PAGE_PROGRAM        0x02
#define CMD_SECTOR_ERASE        0x20
#define CMD_BLOCK_ERASE         0xD8
#define CMD_CHIP_ERASE          0xC7
#define CMD_READ_DATA           0x03
#define CMD_FAST_READ           0x0B
#define CMD_RELEASE_POWER_DOWN  0xAB
#define CMD_MANUFACTURER_ID     0x90

void W25Q128_Init(void) {
    HAL_GPIO_WritePin(W25Q128_CS_GPIO_Port, W25Q128_CS_Pin, GPIO_PIN_SET);
}

void W25Q128_ChipSelect(uint8_t state) {
    if(state == 0) {
        HAL_GPIO_WritePin(W25Q128_CS_GPIO_Port, W25Q128_CS_Pin, GPIO_PIN_RESET);
    } else {
        HAL_GPIO_WritePin(W25Q128_CS_GPIO_Port, W25Q128_CS_Pin, GPIO_PIN_SET);
    }
}

void W25Q128_WriteEnable(void) {
    W25Q128_ChipSelect(0);
    uint8_t cmd = CMD_WRITE_ENABLE;
    HAL_SPI_Transmit(&hspi1, &cmd, 1, 100);
    W25Q128_ChipSelect(1);
}

void W25Q128_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite) {
    W25Q128_WriteEnable(); // 쓰기 활성화 명령을 보냄

    W25Q128_ChipSelect(0); // 칩 선택

    uint8_t cmd[4];
    cmd[0] = CMD_PAGE_PROGRAM;
    cmd[1] = (WriteAddr & 0xFF0000) >> 16; // 주소의 상위 바이트
    cmd[2] = (WriteAddr & 0xFF00) >> 8;    // 주소의 중간 바이트
    cmd[3] = WriteAddr & 0xFF;             // 주소의 하위 바이트

    HAL_SPI_Transmit(&hspi1, cmd, 4, 100); // 페이지 프로그램 명령과 주소 전송
    HAL_SPI_Transmit(&hspi1, pBuffer, NumByteToWrite, 100); // 실제 데이터 전송

    W25Q128_ChipSelect(1); // 칩 선택 해제

    // 쓰기 작업이 완료될 때까지 대기
    uint8_t status;
    do {
        W25Q128_ChipSelect(0);
        HAL_SPI_Transmit(&hspi1, (uint8_t[]){CMD_READ_STATUS_REG}, 1, 100);
        HAL_SPI_Receive(&hspi1, &status, 1, 100);
        W25Q128_ChipSelect(1);
    } while (status & 0x01); // 쓰기 작업 중 상태 레지스터의 LSB가 1인지 확인
}

void W25Q128_ReadData(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead) {
	W25Q128_ChipSelect(0); // 칩 선택
    uint8_t cmd[4];
    cmd[0] = CMD_READ_DATA;
    cmd[1] = (ReadAddr & 0xFF0000) >> 16; // 주소의 상위 바이트
    cmd[2] = (ReadAddr & 0xFF00) >> 8;    // 주소의 중간 바이트
    cmd[3] = ReadAddr & 0xFF;             // 주소의 하위 바이트
    HAL_SPI_Transmit(&hspi1, cmd, 4, HAL_MAX_DELAY); // 읽기 명령과 주소 전송
    HAL_SPI_Receive(&hspi1, pBuffer, NumByteToRead, HAL_MAX_DELAY); // 데이터 읽기
    W25Q128_ChipSelect(1); // 칩 선택 해제
}
