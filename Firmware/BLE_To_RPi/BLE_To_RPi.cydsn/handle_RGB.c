/******************************************************************************
* File Name: handle_RGB.c
*
* Version: 1.00
*
* Description: This file contains functions that handle the initialization and
*              the color /  intensity control of the RGB LED
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
* This file contains functions that handle the initialization and the color / 
* intensity control of the RGB LED
*******************************************************************************/

/* Header file includes */
#include <project.h>
#include "handle_RGB.h"

/* Macros used to turn off the individual LEDs */
#define TurnOffRed      Cy_TCPWM_Counter_SetCompare0(PWM_Red_TCPWM__HW, \
                        PWM_Red_TCPWM__CNT_IDX, RGB_LED_OFF)
#define TurnOffGreen    Cy_TCPWM_Counter_SetCompare0(PWM_Green_TCPWM__HW, \
                        PWM_Green_TCPWM__CNT_IDX, RGB_LED_OFF)
#define TurnOffBlue     Cy_TCPWM_Counter_SetCompare0(PWM_Blue_TCPWM__HW, \
                        PWM_Blue_TCPWM__CNT_IDX, RGB_LED_OFF)

/* Threshold below which color mixing is skipped */
#define LED_NO_COLOR_THRESHOLD      (uint8_t) (0x04u)

/* Macro used to divide the color component to reduce the values to 8 bits */
#define COLOR_COMPONENT_SCALER       (uint8_t) (0xFFu)

/*******************************************************************************
* Function Name: void setColorRGB(uint8_t *colorInfo)
********************************************************************************
* Summary:
*  Changes the RGB LED color to the specified value
*
* Parameters:
*  uint8_t* colorInfo : pointer to a 4-byte array holding the color information
*
* Return:
*  void
*
*******************************************************************************/
void setColorRGB(uint8_t* colorInfo)
{
    /* Local variables to calculate the color components from the received RGB
       data*/
    uint16_t    redComponent;
    uint16_t    greenComponent;
    uint16_t    blueComponent;

    /* If the Intensity value sent by the client is below a set threshold, then 
	   turn off the LEDs */
    if (colorInfo[INTENSITY_INDEX] < LED_NO_COLOR_THRESHOLD)
    {
        TurnOffRed;
        TurnOffGreen;
        TurnOffBlue;
    }
    else
    {
        /* If the individual color values of Red, Green and Blue components are 
		   less than the predefined threshold, then turn off the LEDs */
        if ((colorInfo[RED_INDEX] < LED_NO_COLOR_THRESHOLD) &&
            (colorInfo[GREEN_INDEX] < LED_NO_COLOR_THRESHOLD) &&
            (colorInfo[BLUE_INDEX] < LED_NO_COLOR_THRESHOLD))
        {
            TurnOffRed;
            TurnOffGreen;
            TurnOffBlue;
        }
        else
        {
            /* Calculate the intensity of the Red, Green and Blue components 
        	   from the received 4-byte data */
            redComponent   = (colorInfo[RED_INDEX] * colorInfo
                              [INTENSITY_INDEX]) /COLOR_COMPONENT_SCALER;
            greenComponent = (colorInfo[GREEN_INDEX] * colorInfo
                              [INTENSITY_INDEX])  /COLOR_COMPONENT_SCALER;
            blueComponent  = (colorInfo[BLUE_INDEX] * colorInfo
                              [INTENSITY_INDEX]) /COLOR_COMPONENT_SCALER;

            /* Update the compare value of the TCPWMs for color control */
            Cy_TCPWM_Counter_SetCompare0(PWM_Red_TCPWM__HW,
                                   PWM_Red_TCPWM__CNT_IDX, redComponent);
            Cy_TCPWM_Counter_SetCompare0(PWM_Green_TCPWM__HW,
                                   PWM_Green_TCPWM__CNT_IDX, greenComponent);
            Cy_TCPWM_Counter_SetCompare0(PWM_Blue_TCPWM__HW,
                                   PWM_Blue_TCPWM__CNT_IDX, blueComponent);
        }
    }
}

/*******************************************************************************
* Function Name: void initRGB(void)
********************************************************************************
* Summary:
*  Initializes the RGB LED by starting the TCPWM components
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void initRGB(void)
{
    /* Start the PWM components for LED control*/
    PWM_Red_Start();
    PWM_Green_Start();
    PWM_Blue_Start();
}

/* [] END OF FILE */
