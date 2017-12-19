/******************************************************************************
* File Name: handle_CapSense.c
*
* Version: 1.00
*
* Description: This file contains functions that handle the initialization and
*              the scanning of the CapSense Slider and the Buttons
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
/*******************************************************************************
* This file contains functions that handle the initialization and the scanning of 
* the CapSense Slider and the Buttons
*******************************************************************************/

/* Header file includes */
#include <project.h>
#include "handle_CapSense.h"
#include "BLE_applications.h"

/* CapSense Button macros */
#define BUTTON_DATA_HEADER  (uint8_t) (0x02u)
#define BUTTON0_MASK        (uint8_t) (0x01u)
#define BUTTON1_MASK        (uint8_t) (0x02u)
#define BUTTON_HEADER_POS   (uint8_t) (0x00u)
#define BUTTON_MASK_POS0    (uint8_t) (0x02u)
#define BUTTON_MASK_POS1    (uint8_t) (0x01u)
#define BUTTON_MASK_CLEAR   (uint8_t) (0x00u)

/* Slider position value received from the CapSense Component when the slider is 
   not touched */
#define NO_SLIDER_TOUCH   	(uint16_t) (0xFFFFu)

/* Range of CapSense slider centroid values during a touch is 0-100 */
#define SLIDER_MAX_VALUE    (uint16_t) (0x0064u)

/*******************************************************************************
* Function Name: handleCapSenseSlider
********************************************************************************
* Summary:
*  This function scans for touches on the CapSense slider, and if the centroid 
*  position is different from the previous scan, a separate routine for the BLE 
*  notification is called.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void handleCapSenseSlider(void)
{
    /* CapSense slider position value from the previous scan */
    static uint16_t   previousPosition;

    /*  CapSense slider position value from the current scan */
    uint16_t          currentPosition;

    /* Do this only when the CapSense isn't busy with a previous operation */
    if (CapSense_NOT_BUSY == CapSense_IsBusy())
    {

        /* Read the finger position on the slider */
		CapSense_ProcessAllWidgets();
        currentPosition = CapSense_GetCentroidPos
                          (CapSense_LINEARSLIDER0_WDGT_ID);

        /* If the finger position on the slider is different from the previous 
		   scan, then send the slider data as a BLE notification */
        if (currentPosition != previousPosition)
        {
            /*If touch is detected on the slider */
            if ((currentPosition == NO_SLIDER_TOUCH) ||
                (currentPosition <= SLIDER_MAX_VALUE))
            {
                /* Send data over slider notification */
                sendCapSenseSliderNotification((uint8_t) currentPosition);
            }   

            /* Update the local static variable with the present finger
			    position on the slider */
            previousPosition = currentPosition;
        }
        /* Start CapSense scan */
        CapSense_ScanAllWidgets();
    }
}

/*******************************************************************************
* Function Name: handleCapSenseButton
********************************************************************************
* Summary:
*  This function scans for button presses and  triggers a separate routine for 
*  BLE notification
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void handleCapSenseButton(void)
{
    /* Variables used to store the button information */
    uint8_t         buttonPosition[CAPSENSE_BUTTON_DATA_LEN] = 
                    {
                        /* Header that stores the number of buttons */
                        BUTTON_DATA_HEADER,
                        /* Unused button mask */
                        BUTTON_MASK_CLEAR,
                         /* Button mask for BUTTON0 and BUTTON1 */
                        BUTTON_MASK_CLEAR
                    };  
    uint8_t         cuurButtonMask = BUTTON_MASK_CLEAR;
    uint8_t static  prevButtonMask = BUTTON_MASK_CLEAR;

    /* Do this only when a scan isn't busy */
    if (CapSense_NOT_BUSY == CapSense_IsBusy())
    {
        /* Process touch information from the CapSense sensors */
        CapSense_ProcessAllWidgets();

        /* Get the touch status of button 0 and button 1 */
        if (CapSense_IsWidgetActive(CapSense_BUTTON0_WDGT_ID))
        {
            cuurButtonMask |= BUTTON0_MASK;
        }

        if (CapSense_IsWidgetActive(CapSense_BUTTON1_WDGT_ID))
        {
            cuurButtonMask |= BUTTON1_MASK;
        }

        /* Check for a change in the button mask from the previous scan */
        if (cuurButtonMask != prevButtonMask)
        {
            /* Load the button mask into the button information array */
            buttonPosition[BUTTON_MASK_POS0] = cuurButtonMask;

            /* Send data over button notification */
            sendCapSenseButtonNotification(buttonPosition);
			
			/* Store the current button mask */
            prevButtonMask = cuurButtonMask;
        }

        /* Start CapSense scan */
        CapSense_ScanAllWidgets();
    }
}

/*******************************************************************************
* Function Name: void initCapSense(void)
********************************************************************************
* Summary:
*  Initializes CapSense touch sensing
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void initCapSense(void)
{
    /* Start the CapSense component and initialize the baselines */
    CapSense_Start();
}

/* [] END OF FILE */
