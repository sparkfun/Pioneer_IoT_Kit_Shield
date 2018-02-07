/*******************************************************************************
* File Name: CONNECT_LED.c  
* Version 2.20
*
* Description:
*  This file contains APIs to set up the Pins component for low power modes.
*
* Note:
*
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "cytypes.h"
#include "CONNECT_LED.h"

static CONNECT_LED_BACKUP_STRUCT  CONNECT_LED_backup = {0u, 0u, 0u};


/*******************************************************************************
* Function Name: CONNECT_LED_Sleep
****************************************************************************//**
*
* \brief Stores the pin configuration and prepares the pin for entering chip 
*  deep-sleep/hibernate modes. This function applies only to SIO and USBIO pins.
*  It should not be called for GPIO or GPIO_OVT pins.
*
* <b>Note</b> This function is available in PSoC 4 only.
*
* \return 
*  None 
*  
* \sideeffect
*  For SIO pins, this function configures the pin input threshold to CMOS and
*  drive level to Vddio. This is needed for SIO pins when in device 
*  deep-sleep/hibernate modes.
*
* \funcusage
*  \snippet CONNECT_LED_SUT.c usage_CONNECT_LED_Sleep_Wakeup
*******************************************************************************/
void CONNECT_LED_Sleep(void)
{
    #if defined(CONNECT_LED__PC)
        CONNECT_LED_backup.pcState = CONNECT_LED_PC;
    #else
        #if (CY_PSOC4_4200L)
            /* Save the regulator state and put the PHY into suspend mode */
            CONNECT_LED_backup.usbState = CONNECT_LED_CR1_REG;
            CONNECT_LED_USB_POWER_REG |= CONNECT_LED_USBIO_ENTER_SLEEP;
            CONNECT_LED_CR1_REG &= CONNECT_LED_USBIO_CR1_OFF;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(CONNECT_LED__SIO)
        CONNECT_LED_backup.sioState = CONNECT_LED_SIO_REG;
        /* SIO requires unregulated output buffer and single ended input buffer */
        CONNECT_LED_SIO_REG &= (uint32)(~CONNECT_LED_SIO_LPM_MASK);
    #endif  
}


/*******************************************************************************
* Function Name: CONNECT_LED_Wakeup
****************************************************************************//**
*
* \brief Restores the pin configuration that was saved during Pin_Sleep(). This 
* function applies only to SIO and USBIO pins. It should not be called for
* GPIO or GPIO_OVT pins.
*
* For USBIO pins, the wakeup is only triggered for falling edge interrupts.
*
* <b>Note</b> This function is available in PSoC 4 only.
*
* \return 
*  None
*  
* \funcusage
*  Refer to CONNECT_LED_Sleep() for an example usage.
*******************************************************************************/
void CONNECT_LED_Wakeup(void)
{
    #if defined(CONNECT_LED__PC)
        CONNECT_LED_PC = CONNECT_LED_backup.pcState;
    #else
        #if (CY_PSOC4_4200L)
            /* Restore the regulator state and come out of suspend mode */
            CONNECT_LED_USB_POWER_REG &= CONNECT_LED_USBIO_EXIT_SLEEP_PH1;
            CONNECT_LED_CR1_REG = CONNECT_LED_backup.usbState;
            CONNECT_LED_USB_POWER_REG &= CONNECT_LED_USBIO_EXIT_SLEEP_PH2;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(CONNECT_LED__SIO)
        CONNECT_LED_SIO_REG = CONNECT_LED_backup.sioState;
    #endif
}


/* [] END OF FILE */
