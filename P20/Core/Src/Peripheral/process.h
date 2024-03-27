/*
 * Process.h
 *
 *  Created on: Aug 10, 2022
 *      Author: CBT
 */

#ifndef SRC_PERIPHERAL_PROCESS_H_
#define SRC_PERIPHERAL_PROCESS_H_

void loop();

void DeliSecondProcess(void);
void HalfSecondProcess(void);
void OneSecondProcess(void);
void TenSecondProcess(void);

void Init_Device(void);
void Inithardware();


#endif /* SRC_PERIPHERAL_PROCESS_H_ */
