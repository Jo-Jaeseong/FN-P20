/*
 * pm.c
 *
 *  Created on: 2023. 3. 17.
 *      Author: CBT
 */

#include "pm.h"
#include "main.h"

unsigned int CarbonFilterMax, HEPAFilterMax, PlasmaAssyMax;
unsigned int CarbonFilter, HEPAFilter, PlasmaAssy;

unsigned int totalCount, dailyCount;


void CarbonFilter_Count(){
	//sprintf(CarbonFilterStr, "%u", CarbonFilter);
	CarbonFilter--;
}
void CarbonFilter_Reset(){
	CarbonFilter=0;
}
void HEPAFilter_Count(){
	HEPAFilter--;
}
void HEPAFilter_Reset(){
	HEPAFilter=0;
}
void PlasmaAssy_Count(){
	PlasmaAssy--;
}
void PlasmaAssy_Reset(){
	PlasmaAssy=0;
}

void TotalCyle_Count(){
	totalCount++;
}
void DailyCyle_Count(){
		dailyCount++;
}
void DailyCyle_Reset(){
	dailyCount=0;
}
