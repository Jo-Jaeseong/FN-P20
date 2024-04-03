/*
 * usb.h
 *
 *  Created on: Mar 25, 2024
 *      Author: CBT_DEV
 */

#ifndef SRC_PERIPHERAL_USBMEMORY_H_
#define SRC_PERIPHERAL_USBMEMORY_H_

void SD_Error_Handler(void);
void USB_Error_Handler(void);
void SaveToUSBMemory();
void SaveToSDCard();

#endif /* SRC_PERIPHERAL_USBMEMORY_H_ */
