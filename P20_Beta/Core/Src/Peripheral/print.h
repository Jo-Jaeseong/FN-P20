/*
 * print.h
 *
 *  Created on: Nov 1, 2022
 *      Author: CBT
 */

#ifndef SRC_PERIPHERAL_PRINT_H_
#define SRC_PERIPHERAL_PRINT_H_

extern int autoprintFlag;
extern int printcopy;
extern int printgraphFlag;

void printSterilant();
void printmain();
void PrintLeakTest();
void PrintHeaterTest();
void PrintPartsTest();
void CyclePrint();
void LoadCyclePrint();
void printInformation();
//int printmain(int argc, char *argv[]);
void UsageGuide(char* name);
void PrintText();
void PrintReceipt();
void PrintImage();
void GetPrinterInfo();
void ReadMsrInfo();
void ProcessMSR(int length, unsigned char* buffer, int bufferLen);
void printmsg(char *msg);
void printmsg100(char *msg);
void Drawline(int xL, int xH, int yL, int yH, int n);
void SelectPageMode();
void SetPageArea(int xL, int xH, int yL, int yH, int dxL, int dxH, int dyL, int dyH);
void AbsoluteVertical(int nL, int nH);
void RelativeVertical(int nL, int nH);
void Direction_Left2Right();
void Direction_Top2Bottom();
void Direction_Right2Left();
void printpage();
void DrawVacuum(int x, int y1,int y2, int n);
void DrawTemp(int x, int y1,int y2, int n);

void CycleGrahp(int cycle);
void printtestgraph(int cycle);


#endif /* SRC_PERIPHERAL_PRINT_H_ */
