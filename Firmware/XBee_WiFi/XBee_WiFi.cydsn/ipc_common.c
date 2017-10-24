/*****************************************************************************
* File Name: ipc_common.c
*
* Version: 1.00
*
* Description: Contains common IPC utility functions for use by both CPUs.
*
* Note: These functions may be executed simultaneously by both CPUs.
*
* Related Document: Code example CE216795
*
* Hardware Dependency: See code example CE216795
*
******************************************************************************
* Copyright (2017), Cypress Semiconductor Corporation.
******************************************************************************
* This software is owned by Cypress Semiconductor Corporation (Cypress) and is
* protected by and subject to worldwide patent protection (United States and
* foreign), United States copyright laws and international treaty provisions.
* Cypress hereby grants to licensee a personal, non-exclusive, non-transferable
* license to copy, use, modify, create derivative works of, and compile the
* Cypress Source Code and derivative works for the sole purpose of creating
* custom software in support of licensee product to be used only in conjunction
* with a Cypress integrated circuit as specified in the applicable agreement.
* Any reproduction, modification, translation, compilation, or representation of
* this software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH
* REGARD TO THIS MATERIAL, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* Cypress reserves the right to make changes without further notice to the
* materials described herein. Cypress does not assume any liability arising out
* of the application or use of any product or circuit described herein. Cypress
* does not authorize its products for use as critical components in life-support
* systems where a malfunction or failure may reasonably be expected to result in
* significant injury to the user. The inclusion of Cypress' product in a life-
* support systems application implies that the manufacturer assumes all risk of
* such use and in doing so indemnifies Cypress against all charges. Use may be
* limited by and subject to the applicable Cypress software license agreement.
*****************************************************************************/
#include "project.h"
#include "ipc_common.h"

/* timeout period, in microseconds */
#define MY_TIMEOUT 1000ul

/* debug pin usage */
#if (CY_CPU_CORTEX_M0P)  /* core is Cortex-M0+ */
  #define PIN_READING Pin_CM0p_Reading_0
  #define PIN_WRITING Pin_CM0p_Writing_0
#else /* core is Cortex-M4 */
  #define PIN_READING Pin_CM4_Reading_0
  #define PIN_WRITING Pin_CM4_Writing_0
#endif


/*******************************************************************************
* Function Name: ReadSharedVar
****************************************************************************//**
*
* This function returns a copy of the global shared variable, with
* IPC lock/release and error checking.
*
* \param *sharedVar The address of the shared variable.
* \param *copy The storage address of the copy of the shared variable.
*
* \return status / error code.
*
* \note This function should be used to read the shared variable; the variable
* should not be read directly.
*******************************************************************************/
int32_t ReadSharedVar(const uint8_t *sharedVar, uint8_t *copy)
{
    uint32_t timeout;
    int32_t rtnVal;
    Cy_GPIO_Set(PIN_READING);

    /* timeout wait to acquire lock */
    for (timeout = 0ul; timeout < MY_TIMEOUT; timeout++)
    {
        rtnVal = Cy_IPC_LOCK_Acquire(MY_LOCKNUM);
        /* exit the timeout wait if successfully acquired or error */
        if ((rtnVal == CY_IPC_LOCK_SUCCESS) || (rtnVal == CY_IPC_LOCK_OUT_OF_RANGE)) break;
        CyDelayUs(1);
    }
    if (timeout >= MY_TIMEOUT) rtnVal = (int32_t)CY_RET_TIMEOUT;
    
    if (rtnVal == CY_IPC_LOCK_SUCCESS)
    {
        *copy = *sharedVar;

        /* timeout wait to release lock */
        for (timeout = 0ul; timeout < MY_TIMEOUT; timeout++)
        {
            rtnVal = Cy_IPC_LOCK_Release(MY_LOCKNUM);
            /* exit the timeout wait if successfully released or error */
            if ((rtnVal == CY_IPC_LOCK_SUCCESS) || (rtnVal == CY_IPC_LOCK_OUT_OF_RANGE)) break;
            CyDelayUs(1);
        }
        if (timeout >= MY_TIMEOUT) rtnVal = (int32_t)CY_RET_TIMEOUT;
    }
    Cy_GPIO_Clr(PIN_READING);
    return (rtnVal);
}

/*******************************************************************************
* Function Name: WriteSharedVar
****************************************************************************//**
*
* This function writes a value to the global shared variable, with
* IPC lock/release and error checking.
*
* \param *sharedVar The address of the shared variable.
* \param value The value to be written.
*
* \return status / error code.
*
* \note This function should be used to write to the shared variable; the
* variable should not be written directly.
*******************************************************************************/
int32_t WriteSharedVar(uint8_t *sharedVar, uint8_t value)
{
    uint32_t timeout;
    int32_t rtnVal;
    Cy_GPIO_Set(PIN_WRITING);

    /* timeout wait to acquire lock */
    for (timeout = 0ul; timeout < MY_TIMEOUT; timeout++)
    {
        rtnVal = Cy_IPC_LOCK_Acquire(MY_LOCKNUM);
        /* exit the timeout wait if successfully acquired or error */
        if ((rtnVal == CY_IPC_LOCK_SUCCESS) || (rtnVal == CY_IPC_LOCK_OUT_OF_RANGE)) break;
        CyDelayUs(1);
    }
    if (timeout >= MY_TIMEOUT) rtnVal = (int32_t)CY_RET_TIMEOUT;

    if (rtnVal == CY_IPC_LOCK_SUCCESS)
    {
        *sharedVar = value;

        /* timeout wait to release lock */
        for (timeout = 0ul; timeout < MY_TIMEOUT; timeout++)
        {
            rtnVal = Cy_IPC_LOCK_Release(MY_LOCKNUM);
            /* exit the timeout wait if successfully released or error */
            if ((rtnVal == CY_IPC_LOCK_SUCCESS) || (rtnVal == CY_IPC_LOCK_OUT_OF_RANGE)) break;
            CyDelayUs(1);
        }
        if (timeout >= MY_TIMEOUT) rtnVal = (int32_t)CY_RET_TIMEOUT;
    }
    Cy_GPIO_Clr(PIN_WRITING);
    return (rtnVal);
}

/* [] END OF FILE */
