/*
 * rfid.h
 *
 *  Created on: May 31, 2019
 *      Author: monster
 */

#ifndef PERIPHERAL_RFID_H_
#define PERIPHERAL_RFID_H_

#include "main.h"
#include "Process.h"
#include "rfid.h"

struct RFID_format {
	/*
	시리얼 넘버(생산일자+생산숫자)
	 - 생산 일자(production_date)
	 - 생산 숫자
	인식 날짜
	잔량
	*/
	unsigned int production_year, production_month, production_day;
	unsigned int production_number;
	unsigned int open_year, open_month, open_day;
	unsigned int volume;
	unsigned int elapsed_days;
	unsigned int currentID;
};

extern struct RFID_format RFIDData;

void InitRFID(void);
uint32_t ReadRFID(void);

#endif /* PERIPHERAL_RFID_H_ */
