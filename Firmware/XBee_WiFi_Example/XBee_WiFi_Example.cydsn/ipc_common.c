#include "project.h"
#include "ipc_common.h"

/* timeout period, in microseconds */
#define MY_TIMEOUT 1000ul

/*******************************************************************************
* Function Name: IsSemaError
****************************************************************************//**
*
* This function checks a returned status value for IPC semaphore errors.
*
* \param status The status given from any semaphores function.
*
* \return error detected or not.
*
*******************************************************************************/
static bool IsSemaError(uint32_t status)
{
    return ((status & (uint32_t)CY_IPC_SEMA_ID_ERROR) == (uint32_t)CY_IPC_SEMA_ID_ERROR);
}

/*******************************************************************************
* Function Name: ReadSharedVar
****************************************************************************//**
*
* This function returns a copy of the global shared variable, with
* IPC semaphore set/clear and error checking.
*
* \param *sharedVar The address of the shared variable.
* \param *copy The storage address of the copy of the shared variable.
*
* \return status / error code.
*
* \note This function should be used to read the shared variable; the variable
* should not be read directly.
*******************************************************************************/
uint32_t ReadSharedVar(const uint8_t *sharedVar, uint8_t *copy, uint8_t semaID)
{
    uint32_t timeout;
    uint32_t rtnVal;

    /* timeout wait to set the semaphore */
    for (timeout = 0ul; timeout < MY_TIMEOUT; timeout++)
    {
        rtnVal = (uint32_t)Cy_IPC_Sema_Set(semaID, false);
        /* exit the timeout wait if semaphore successfully set or error */
        if ((rtnVal == (uint32_t)CY_IPC_SEMA_SUCCESS) || IsSemaError(rtnVal))
        {
            break;
        }
        CyDelayUs(1);
    }
    if (timeout >= MY_TIMEOUT) rtnVal = CY_RET_TIMEOUT;
    
    if (rtnVal == CY_IPC_SEMA_SUCCESS)
    {
        *copy = *sharedVar;

        /* timeout wait to clear semaphore */
        for (timeout = 0ul; timeout < MY_TIMEOUT; timeout++)
        {
            rtnVal = Cy_IPC_Sema_Clear(semaID, false);
            /* exit the timeout wait if semaphore successfully cleared or error */
            if ((rtnVal == (uint32_t)CY_IPC_SEMA_SUCCESS) || IsSemaError(rtnVal))
            {
                break;
            }
            CyDelayUs(1);
        }
        if (timeout >= MY_TIMEOUT) rtnVal = CY_RET_TIMEOUT;
    }
    return (rtnVal);
}

/*******************************************************************************
* Function Name: WriteSharedVar
****************************************************************************//**
*
* This function writes a value to the global shared variable, with
* IPC semaphore set/clear and error checking.
*
* \param *sharedVar The address of the shared variable.
* \param value The value to be written.
*
* \return status / error code.
*
* \note This function should be used to write to the shared variable; the
* variable should not be written directly.
*******************************************************************************/
uint32_t WriteSharedVar(uint8_t *sharedVar, uint8_t value, uint8_t semaID)
{
    uint32_t timeout;
    uint32_t rtnVal;

    /* timeout wait to set semaphore */
    for (timeout = 0ul; timeout < MY_TIMEOUT; timeout++)
    {
        rtnVal = (uint32_t)Cy_IPC_Sema_Set(semaID, false);
        /* exit the timeout wait if semaphore successfully set or error */
        if ((rtnVal == (uint32_t)CY_IPC_SEMA_SUCCESS) || IsSemaError(rtnVal))
        {
            break;
        }
        CyDelayUs(1);
    }
    if (timeout >= MY_TIMEOUT) rtnVal = CY_RET_TIMEOUT;

    if (rtnVal == CY_IPC_SEMA_SUCCESS)
    {
        *sharedVar = value;

        /* timeout wait to clear semaphore */
        for (timeout = 0ul; timeout < MY_TIMEOUT; timeout++)
        {
            rtnVal = (uint32_t)Cy_IPC_Sema_Clear(semaID, false);
            /* exit the timeout wait if semaphore successfully cleared or error */
            if ((rtnVal == (uint32_t)CY_IPC_SEMA_SUCCESS) || IsSemaError(rtnVal))
            {
                break;
            }
            CyDelayUs(1);
        }
        if (timeout >= MY_TIMEOUT) rtnVal = CY_RET_TIMEOUT;
    }
    return (rtnVal);
}

/* [] END OF FILE */
