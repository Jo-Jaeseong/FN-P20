/*
 * login_system.h
 *
 *  Created on: 2023. 3. 21.
 *      Author: CBT
 */

#ifndef SRC_PERIPHERAL_LOGINSYSTEM_H_
#define SRC_PERIPHERAL_LOGINSYSTEM_H_

extern unsigned int CurrentUser;
extern int Select_ID;

//아이디 생성
int createUser();
int loginProcess();

int changePWUser();
int changePWloginUser();
int DeletConfirmloginUser();

void ReadInputCreateIDPWFromLCD();
void ReadInputLoginIDPWFromLCD();
void ReadInputManagmentIDPWFromLCD();


#endif /* SRC_PERIPHERAL_LOGINSYSTEM_H_ */
