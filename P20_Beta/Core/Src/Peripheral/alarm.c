/*
 * alarm.c
 *
 *  Created on: 2023. 3. 29.
 *      Author: CBT
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"

#include "hardware.h"
#include "sensor.h"
#include "peripheral.h"

/*
알람1	장비 전원 정전	장비 전원 차단 됨
알람2	챔버 도어 열림	도어가 열림, 스위치 동작 안됨
알람3	멸균제 병 감지 안됨	멸균제 병 없거나, 스위치 동작 안됨
알람4	멸균제 도어 열림	도어 열림, 스위치 동작 안됨
알람5	멸균제 S/N 오류	멸균제 S/N가 맞지 안음
알람6	멸균제 제조 만기일 지남	멸균제 제조 후 1년 지남
알람7	멸균제 사용 기간 지남	장비내 멸균제 삽입 후 60일 지남
알람8	도어 히터 온도 확인	도어 히터 및 히터 전선, TC 및 TC선 확인
알람9	챔버 히터 온도 확인	챔버 히터 및 히터 전선, TC 및 TC선 확인
알람10	챔버 후면 히터 온도 확인	챔버 후면 히터 및 히터 전선, TC 및 TC선 확인
알람11	기화기 히터 온도 확인	기화기 히터 및 히터 전선, TC 및 TC선 확인
알람12	PM 촉매제 필터 교체	촉매제 설정값 0임, 교체 후 재설정
알람13	PM 헤파필터 교체	헤파필터 설정값 0임, 교체 후 재설정
알람14	PM 플라즈마 전극 교체	플라즈마 전극 설정값 0임, 교체 후 재설정

*/

extern int checkret;
int Alarmcode=0;
unsigned char devicealarm[15]={};


int ErrorCheckFlag;
int AlarmCheckFlag;

int Before_CycleAlarm_Check(){
	//장비 차단 전원 관련 확인

	if(DoorLatchCheck()){
		devicealarm[2]=0;
	}
	else{
		devicealarm[2]=1;
	}

	if(BottleCheck()){
		devicealarm[3]=0;
	}
	else{
		devicealarm[3]=1;
	}

	if(BottleDoorCheck()){
		devicealarm[4]=0;
	}
	else{
		devicealarm[4]=1;
	}

	//멸균제 시리얼 확인
	if(checkret==-2){
		devicealarm[5]=1;
	}
	else{
		devicealarm[5]=0;
	}
	//멸균제 제조 기간 확인
	GetTime();
	if((365-calculateElapsedDays(bcd2bin(today_date.year),bcd2bin(today_date.month),bcd2bin(today_date.day),
			(int)RFIDData.production_year,(int)RFIDData.production_month,(int)RFIDData.production_day))>0){
		devicealarm[6]=0;
	}
	else{
		devicealarm[6]=1;
	}
	//멸균제 장착 기간 확인
	if((60-calculateElapsedDays(bcd2bin(today_date.year),bcd2bin(today_date.month),bcd2bin(today_date.day),
			RFIDData.production_year,RFIDData.open_month,RFIDData.open_day))>0){
		devicealarm[7]=0;
	}
	else{
		devicealarm[7]=1;
	}

	if((DoorSettingTemp[0]-TestTempErrorValue)>Temperature[0]){
		devicealarm[8]=1;
	}
	else{
		devicealarm[8]=0;
	}

	if((ChamberSettingTemp[0]-TestTempErrorValue)>Temperature[1]){
		devicealarm[9]=1;
	}
	else{
		devicealarm[9]=0;
	}

	if((ChamberBackSettingTemp[0]-TestTempErrorValue)>Temperature[2]){
		devicealarm[10]=1;
	}
	else{
		devicealarm[10]=0;
	}

	if((VaporizerSettingTemp[0]-TestTempErrorValue)>Temperature[3]){
		devicealarm[11]=1;
	}
	else{
		devicealarm[11]=0;
	}

	if(CarbonFilter<=0){
		devicealarm[12]=1;
	}
	else{
		devicealarm[12]=0;
	}

	if(HEPAFilter<=0){
		devicealarm[13]=1;
	}
	else{
		devicealarm[13]=0;
	}

	if(PlasmaAssy<=0){
		devicealarm[14]=1;
	}
	else{
		devicealarm[14]=0;
	}
	devicealarm[0]=0;
	for(int i=1;i<15;i++){
		devicealarm[0]+=devicealarm[i];
	}
	return devicealarm[0];

}

void Sterilant_Check(){
	if(BottleCheck()){
		devicealarm[3]=0;
	}
	else{
		devicealarm[3]=1;
	}

	if(BottleDoorCheck()){
		devicealarm[4]=0;
	}
	else{
		devicealarm[4]=1;
	}

	//멸균제 시리얼 확인
	if(checkret==-2){
		devicealarm[5]=1;
	}
	else{
		devicealarm[5]=0;
	}
	//멸균제 제조 기간 확인
	GetTime();
	if((365-calculateElapsedDays(bcd2bin(today_date.year),bcd2bin(today_date.month),bcd2bin(today_date.day),
			(int)RFIDData.production_year,(int)RFIDData.production_month,(int)RFIDData.production_day))>0){
		devicealarm[6]=0;
	}
	else{
		devicealarm[6]=1;
	}
	//멸균제 장착 기간 확인
	if((60-calculateElapsedDays(bcd2bin(today_date.year),bcd2bin(today_date.month),bcd2bin(today_date.day),
			flash_sterilant_open_year,RFIDData.open_month,RFIDData.open_day))>0){
		devicealarm[7]=0;
	}
	else{
		devicealarm[7]=1;
	}
}

/*
알람1	장비 전원 정전	장비 전원 차단 됨
알람2	챔버 도어 열림	도어가 열림, 스위치 동작 안됨
알람3	멸균제 병 감지 안됨	멸균제 병 없거나, 스위치 동작 안됨
알람4	멸균제 도어 열림	도어 열림, 스위치 동작 안됨
알람5	멸균제 S/N 오류	멸균제 S/N가 맞지 안음
알람6	멸균제 제조 만기일 지남	멸균제 제조 후 1년 지남
알람7	멸균제 사용 기간 지남	장비내 멸균제 삽입 후 60일 지남
알람8	도어 히터 온도 확인	도어 히터 및 히터 전선, TC 및 TC선 확인
알람9	챔버 히터 온도 확인	챔버 히터 및 히터 전선, TC 및 TC선 확인
알람10	챔버 후면 히터 온도 확인	챔버 후면 히터 및 히터 전선, TC 및 TC선 확인
알람11	기화기 히터 온도 확인	기화기 히터 및 히터 전선, TC 및 TC선 확인
알람12	PM 촉매제 필터 교체	촉매제 설정값 0임, 교체 후 재설정
알람13	PM 헤파필터 교체	헤파필터 설정값 0임, 교체 후 재설정
알람14	PM 플라즈마 전극 교체	플라즈마 전극 설정값 0임, 교체 후 재설정

*/

void Alarm_Check(){
	if(devicealarm[1]==1){
		DisplayPage10Char(0x07,0x10," ALARM001 ");
		DisplayPage10Char(0x07,0x20,"          ");
		char msg[80] = "Power Problem";
		DisplayMsg(0x07,0x30,msg);
	}
	else if(devicealarm[2]==1){
		DisplayPage10Char(0x07,0x10," ALARM002 ");
		DisplayPage10Char(0x07,0x20,"DOOR OPEN ");
		char msg[80] = "The door is open. Please close the door and try again.";
		DisplayMsg(0x07,0x30,msg);
	}
	else if(devicealarm[3]==1){
		DisplayPage10Char(0x07,0x10," ALARM003 ");
		DisplayPage10Char(0x07,0x20,"NO BOTTLE ");
		char msg[80] = "No bottle detected. Please check and try again.";
		DisplayMsg(0x07,0x30,msg);
	}
	else if(devicealarm[4]==1){
		DisplayPage10Char(0x07,0x10," ALARM004 ");
		DisplayPage10Char(0x07,0x20,"BDOOR OPEN");
		char msg[80] = "The bottle door is open. Please close it and try again.";
		DisplayMsg(0x07,0x30,msg);
	}
	else if(devicealarm[5]==1){
		DisplayPage10Char(0x07,0x10," ALARM005 ");
		DisplayPage10Char(0x07,0x20,"WRONG S/N ");
		char msg[80] = "The serial number is not valid. Must Use CBT Sterile H2O2.";
		DisplayMsg(0x07,0x30,msg);
	}
	else if(devicealarm[6]==1){
		DisplayPage10Char(0x07,0x10," ALARM006 ");
		DisplayPage10Char(0x07,0x20,"DATEOVER  ");
		char msg[80] = "Sterilizer expired.";
		DisplayMsg(0x07,0x30,msg);
	}
	else if(devicealarm[7]==1){
		DisplayPage10Char(0x07,0x10," ALARM007 ");
		DisplayPage10Char(0x07,0x20,"DATEOVER2 ");
		char msg[80] = "Sterilizer expired.";
		DisplayMsg(0x07,0x30,msg);
	}
	else if(devicealarm[8]==1){
		DisplayPage10Char(0x07,0x10," ALARM008 ");
		DisplayPage10Char(0x07,0x20,"D.H ALARM ");
		char msg[80] = "The door heater is not heated enough.";
		DisplayMsg(0x07,0x30,msg);
	}
	else if(devicealarm[9]==1){
		DisplayPage10Char(0x07,0x10," ALARM009 ");
		DisplayPage10Char(0x07,0x20,"C.H ALARM ");
		char msg[80] = "The chamber heater is not heated enough.";
		DisplayMsg(0x07,0x30,msg);
	}
	else if(devicealarm[10]==1){
		DisplayPage10Char(0x07,0x10," ALARM010 ");
		DisplayPage10Char(0x07,0x20,"B.H ALARM ");
		char msg[80] = "The chamber back heater is not heated enough.";
		DisplayMsg(0x07,0x30,msg);
	}
	else if(devicealarm[11]==1){
		DisplayPage10Char(0x07,0x10," ALARM011 ");
		DisplayPage10Char(0x07,0x20,"V.H ALARM ");
		char msg[80] = "The vaporizer heater is not heated enough.";
		DisplayMsg(0x07,0x30,msg);
	}
	else if(devicealarm[12]==1){
		DisplayPage10Char(0x07,0x10," ALARM012 ");
		DisplayPage10Char(0x07,0x20,"C.Filter  ");
		char msg[80] = "The carbon filter has expired.";
		DisplayMsg(0x07,0x30,msg);
	}
	else if(devicealarm[13]==1){
		DisplayPage10Char(0x07,0x10," ALARM013 ");
		DisplayPage10Char(0x07,0x20,"HEPAFilter");
		char msg[80] = "The HEPA filter has expired.";
		DisplayMsg(0x07,0x30,msg);
	}
	else if(devicealarm[14]==1){
		DisplayPage10Char(0x07,0x10," ALARM014 ");
		DisplayPage10Char(0x07,0x20,"PLASMAASSY");
		char msg[80] = "The plasma ASSY has expired.";
		DisplayMsg(0x07,0x30,msg);
	}

}

void DeviceErrorProcess(){
	//여기
	/*
	에러1	공정 취소	공정 중 취소 버튼 누름
	에러2	챔버 온도 도달 안됨	챔버 설정 온도 도달 안됨
	에러3	압력1 도달 안됨	압력1 설정 시간내 도달 안됨
	에러4	압력2 도달 안됨	압력2 설정 시간내 도달 안됨
	에러5	기화기 히터 온도 확인	기화기 온도 설정 시간내 도달 안됨 (V3P2까지 온도 확인)
	에러6	멸균제1 감지 안됨	멸균제1 부족 및 공급 불량
	에러7	멸균제 주입 후 압력 도달 안됨	멸균제 주입 불량 및 압력 센서 문제
	에러8	압력3 도달 안됨	압력3 설정 시간내 도달 안됨
	에러9	멸균제2 감지 안됨	멸균제2 부족 및 공급 불량
	에러10	멸균제 주입 후 압력 도달 안됨	멸균제 주입 불량 및 압력 센서 문제
	 */
	ErrorCheck();
	if(deviceerror[0]!=0){
		if(StopFlag==0){
			if(Select_NORMAL_MODE==1){
				if(deviceerror[1]==1){
					//취소
					DisplayPage8Char(0x13,0x01,"ERROR01");
					DisplayPage8Char(0x12,0x01,"ERROR01");
					DisplayPage4Char(0x14,0x10,"01  ");
					errorcode=1;
				}
				else if(deviceerror[2]==1){
					//전구간 체크 온도 도달 안됨
					DisplayPage8Char(0x13,0x01,"ERROR02");
					DisplayPage8Char(0x12,0x01,"ERROR02");
					DisplayPage4Char(0x14,0x10,"02  ");
					errorcode=2;
				}
				else if(deviceerror[3]==1){
					//에러3 압력1 도달 안됨
					DisplayPage8Char(0x13,0x01,"ERROR03");
					DisplayPage8Char(0x12,0x01,"ERROR03");
					DisplayPage4Char(0x14,0x10,"03  ");
					errorcode=3;
				}
				else if(deviceerror[4]==1){
					DisplayPage8Char(0x13,0x01,"ERROR04");
					DisplayPage8Char(0x12,0x01,"ERROR04");
					DisplayPage4Char(0x14,0x10,"04  ");
					errorcode=4;
				}
				else if(deviceerror[5]==1){
					DisplayPage8Char(0x13,0x01,"ERROR05");
					DisplayPage8Char(0x12,0x01,"ERROR05");
					DisplayPage4Char(0x14,0x10,"05  ");
					errorcode=5;

				}
				else if(deviceerror[6]==1){
					DisplayPage8Char(0x13,0x01,"ERROR06");
					DisplayPage8Char(0x12,0x01,"ERROR06");
					DisplayPage4Char(0x14,0x10,"06  ");
					errorcode=6;
				}
				else if(deviceerror[7]==1){
					DisplayPage8Char(0x13,0x01,"ERROR07");
					DisplayPage8Char(0x12,0x01,"ERROR07");
					DisplayPage4Char(0x14,0x10,"07  ");
					errorcode=7;
				}
				else if(deviceerror[8]==1){
					DisplayPage8Char(0x13,0x01,"ERROR08");
					DisplayPage8Char(0x12,0x01,"ERROR08");
					DisplayPage4Char(0x14,0x10,"08  ");
					errorcode=8;
				}
				else if(deviceerror[9]==1){
					DisplayPage8Char(0x13,0x01,"ERROR09");
					DisplayPage8Char(0x12,0x01,"ERROR09");
					DisplayPage4Char(0x14,0x10,"09  ");
					errorcode=9;
				}
				else if(deviceerror[10]==1){
					DisplayPage8Char(0x13,0x01,"ERROR10");
					DisplayPage8Char(0x12,0x01,"ERROR10");
					DisplayPage4Char(0x14,0x10,"10  ");
					errorcode=10;
				}
				else{

				}
				if(ErrorCheckFlag){
					DisplayPage(LCD_EORROR_POPUP_PAGE);
					ErrorEndProcess();
					deviceerror[0]=0;
					StopFlag=1;
				}
			}
			else{
				if(deviceerror[1]==1){
					//취소
					DisplayPage8Char(0x13,0x01,"ERROR01");
					DisplayPage8Char(0x12,0x01,"ERROR01");
					errorcode=1;
				}
				else if(deviceerror[2]==1){
					//전구간 체크 온도 도달 안됨
					DisplayPage8Char(0x13,0x01,"ERROR02");
					DisplayPage8Char(0x12,0x01,"ERROR02");
					errorcode=2;
				}
				else if(deviceerror[3]==1){
					//에러3 압력1 도달 안됨
					DisplayPage8Char(0x13,0x01,"ERROR03");
					DisplayPage8Char(0x12,0x01,"ERROR03");
					errorcode=3;
				}
				else if(deviceerror[4]==1){
					DisplayPage8Char(0x13,0x01,"ERROR04");
					DisplayPage8Char(0x12,0x01,"ERROR04");
					errorcode=4;
				}
				else if(deviceerror[5]==1){
					DisplayPage8Char(0x13,0x01,"ERROR05");
					DisplayPage8Char(0x12,0x01,"ERROR05");
					errorcode=5;

				}
				else if(deviceerror[6]==1){
					DisplayPage8Char(0x13,0x01,"ERROR06");
					DisplayPage8Char(0x12,0x01,"ERROR06");
					errorcode=6;
				}
				else if(deviceerror[7]==1){
					DisplayPage8Char(0x13,0x01,"ERROR07");
					DisplayPage8Char(0x12,0x01,"ERROR07");
					errorcode=7;
				}
				else if(deviceerror[8]==1){
					DisplayPage8Char(0x13,0x01,"ERROR08");
					DisplayPage8Char(0x12,0x01,"ERROR08");
					errorcode=8;
				}
				else if(deviceerror[9]==1){
					DisplayPage8Char(0x13,0x01,"ERROR09");
					DisplayPage8Char(0x12,0x01,"ERROR09");
					errorcode=9;
				}
				else if(deviceerror[10]==1){
					DisplayPage8Char(0x13,0x01,"ERROR10");
					DisplayPage8Char(0x12,0x01,"ERROR10");
					errorcode=10;
				}
				else{

				}
				//테스트
				if(ErrorCheckFlag){
					ErrorEndProcess();
					deviceerror[0]=0;
					StopFlag=1;
				}

			}
		}
	}

}

void ErrorCheck(){
	if(Running_Flag==1){
		if(StopFlag==0){
			for(int i=1;i<10;i++){
				deviceerror[0]+=deviceerror[i];
			}
		}
	}
}
