/******************************************************************************
* File Name: BLE_applications.h
*
* Version: 1.00
*
* Description: This file contains macros and the declaration of functions provided 
*              by the BLE_applications.c file 
*
* Related Document: CE218134_BLE_CapSense.pdf
*
* Hardware Dependency: CY8CKIT-062-BLE PSoC 6 BLE Pioneer Kit
*
******************************************************************************
* Copyright (2017), Cypress Semiconductor Corporation.
******************************************************************************
* This software, including source code, documentation and related materials
* ("Software") is owned by Cypress Semiconductor Corporation (Cypress) and is
* protected by and subject to worldwide patent protection (United States and 
* foreign), United States copyright laws and international treaty provisions. 
* Cypress hereby grants to licensee a personal, non-exclusive, non-transferable
* license to copy, use, modify, create derivative works of, and compile the 
* Cypress source code and derivative works for the sole purpose of creating 
* custom software in support of licensee product, such licensee product to be
* used only in conjunction with Cypress's integrated circuit as specified in the
* applicable agreement. Any reproduction, modification, translation, compilation,
* or representation of this Software except as specified above is prohibited 
* without the express written permission of Cypress.
* 
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, 
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* Cypress reserves the right to make changes to the Software without notice. 
* Cypress does not assume any liability arising out of the application or use
* of Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use as critical components in any products 
* where a malfunction or failure may reasonably be expected to result in 
* significant injury or death ("ACTIVE Risk Product"). By including Cypress's 
* product in a ACTIVE Risk Product, the manufacturer of such system or application
* assumes all risk of such use and in doing so indemnifies Cypress against all
* liability. Use of this Software may be limited by and subject to the applicable
* Cypress software license agreement.
*****************************************************************************/
/********************************************************************************
* This file contains macros and the declaration of functions provided by the
* BLE_applications.c file 
********************************************************************************/

/* Include guard */
#ifndef BLE_APPLICATIONS_H
#define BLE_APPLICATIONS_H

/* Header file includes */
#include <project.h>

/* Bit mask for the notification bit in CCCD (Client Characteristic Configuration 
   Descriptor), which is written by the client device */
#define CCCD_NOTIFY_BIT_MASK    (uint8_t) (0x01u)

/* Event handler function for the three custom services in this project */
void    customEventHandler(uint32_t event, void *eventParameter);

/* Functions that send CapSense data as notifications */
void    sendCapSenseButtonNotification(uint8_t *CapSenseButtonData);
void    sendCapSenseSliderNotification(uint8_t CapSenseSliderData);

/* Function that checks for events from the handler and starts the BLE advertisement
  if required */
void    startAdvertisement(void);

#endif
/* [] END OF FILE */
