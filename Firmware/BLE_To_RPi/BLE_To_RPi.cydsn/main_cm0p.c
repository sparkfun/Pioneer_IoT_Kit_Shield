/******************************************************************************
* File Name: main_cm4.c
*
* Version: 1.00
*
* Description: This project demonstrates connectivity between the PSoC 6 BLE and 
*              CySmart BLE host Emulation tool or mobile device running the CySmart
*              mobile application, to transfer CapSense touch sensing and RGB LED
*              control information.
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
/******************************************************************************
* This project demonstrates the capability of PSoC 6 BLE to communicate 
* bi-directionally with a BLE Central device over custom services. The CapSense 
* custom service allows notifications to be sent to central device when 
* notifications are enabled. On the other hand, the RGB LED custom service allows 
* read and write of attributes under the RGB characteristics.
* This project utilizes CapSense component to check slider and button touch 
* information and report this to central device over BLE. On the other hand, the 
* control values sent to PSoC 6 BLE is converted to respective color and intensity 
* of the on-board  RGB LED. The BLE central device can be any BLE central device, 
* including CySmart mobile App or CySmart PC tool. 
*******************************************************************************/

/* Header file includes */
#include <project.h>
#include "BLE_applications.h"
#include "handle_RGB.h"
#include "handle_CapSense.h"

/* This flag is used by application to know whether a Central device has been 
   connected. This is updated in BLE event callback function */ 
extern bool deviceConnected;

/*These flags are set when the Central device writes to CCCD of the 
  CapSense slider/button Characteristic to enable notifications */
extern bool sendCapSenseSliderNotifications;
extern bool sendCapSenseButtonNotifications;

/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
*  System entrance point. This function enables the Cortex-M4 and continuously 
*  processes BLE controller events
*
* Parameters:
*  void
*
* Return:
*  int
*
*******************************************************************************/
int main(void)
{
    /* Enable global interrupts */
    __enable_irq();

    /* Disable the Watchdog Timer to avoid CPU resets */
    Cy_WDT_Unlock();
    Cy_WDT_Disable();
    
    /* Initialize the TCPWMs that drive the RGB LED */
	initRGB();
    
	/* Initialize CapSense touch sensing */
    initCapSense();
    
    /* Start BLE component and register the customEventHandler function. This 
	   function exposes the events from BLE component for application use */
    Cy_BLE_Start(customEventHandler);
    	
    
    for(;;)
    {
        /* Process event callback to handle BLE events. The events generated 
		   and used for this application are inside the 'customEventHandler' 
           routine */
        Cy_BLE_ProcessEvents();
		
        /* If a connection is detected, handle CapSense data transmission */
		if(deviceConnected == true)
		{
			/* Send CapSense Slider data when respective notification is 
               enabled */
			if(sendCapSenseSliderNotifications == CCCD_NOTIFY_BIT_MASK)
			{
				/* Check for CapSense slider swipe and send the data 
                   accordingly */
				handleCapSenseSlider();
			}
            /* Send CapSense Button data when respective notification is 
               enabled */
			if(sendCapSenseButtonNotifications == CCCD_NOTIFY_BIT_MASK)
			{
				/* Check for CapSense button press and send data
                   accordingly */
				handleCapSenseButton();
			}
		}
        /* Start the BLE advertisement if required */
        startAdvertisement();
    }	
}

/* [] END OF FILE */
