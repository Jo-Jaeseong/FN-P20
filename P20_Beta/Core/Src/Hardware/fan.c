/*
 * door_latch.c
 *
 *  Created on: Nov 10, 2022
 *      Author: CBT
 */

#include "main.h"
#include "hardware.h"

// ------------------- Variables -------------------

// ------------------- Functions -------------------
void Fan(int onoff){
	DC7(onoff);
}
