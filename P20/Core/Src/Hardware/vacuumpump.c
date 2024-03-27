/*
 * vacuumpump.c
 *
 *  Created on: Nov 10, 2022
 *      Author: CBT
 */

#include "main.h"
#include "hardware.h"

// ------------------- Variables -------------------

// ------------------- Functions -------------------
void VacuumPump(int onoff){
	AC6(onoff);
}

