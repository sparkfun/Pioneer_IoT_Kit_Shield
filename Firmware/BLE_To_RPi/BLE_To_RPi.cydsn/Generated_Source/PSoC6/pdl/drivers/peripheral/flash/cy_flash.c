/***************************************************************************//**
* \file cy_flash.c
* \version 2.0
*
* \brief
* Provides the public functions for the API for the PSoC 6 Flash Driver.
*
********************************************************************************
* \copyright
* Copyright 2016-2017, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/
#include "flash/cy_flash.h"
#include "sysclk/cy_sysclk.h"
#include "sysint/cy_sysint.h"
#include "ipc/cy_ipc_drv.h"
#include "ipc/cy_ipc_sema.h"
#include "ipc/cy_ipc_pipe.h"

/***************************************
* Data Structure definitions
***************************************/

/* Flash driver context */
typedef struct
{
    uint32_t opcode;      /**< Specifies the code of flash operation */
    uint32_t arg1;        /**< Specifies the configuration of flash operation */
    uint32_t arg2;        /**< Specifies the configuration of flash operation */
    uint32_t arg3;        /**< Specifies the configuration of flash operation */
} cy_stc_flash_context_t;


/***************************************
* Macro definitions
***************************************/

/**
* \addtogroup group_flash_config_macros
* \{
*/

/** Set SROM API in blocking mode */
#define CY_FLASH_BLOCKING_MODE             ((0x01UL) << 8UL)
/** Set SROM API in non blocking mode */
#define CY_FLASH_NON_BLOCKING_MODE         (0UL)
/** SROM API opcode mask */
#define CY_FLASH_OPCODE_Msk                ((0xFFUL) << 24UL)
/** SROM API opcode for flash write operation */
#define CY_FLASH_OPCODE_WRITE_ROW          ((0x05UL) << 24UL)
/** SROM API opcode for flash program operation */
#define CY_FLASH_OPCODE_PROGRAM_ROW        ((0x06UL) << 24UL)
/** SROM API opcode for row erase operation */
#define CY_FLASH_OPCODE_ERASE_ROW          ((0x1CUL) << 24UL)
/** SROM API opcode for flash checksum operation */
#define CY_FLASH_OPCODE_CHECKSUM           ((0x0BUL) << 24UL)
/** SROM API opcode for flash hash operation */
#define CY_FLASH_OPCODE_HASH               ((0x0DUL) << 24UL)
/** SROM API flash row shift for flash checksum operation */
#define CY_FLASH_OPCODE_CHECKSUM_ROW_SHIFT (8UL)
/** SROM API flash data size parameter for flash write operation */
#define CY_FLASH_CONFIG_DATASIZE           (CPUSS_FLASHC_PA_SIZE_LOG2 - 1UL)
/** Data to be programmed to flash is located in SRAM memory region */
#define CY_FLASH_DATA_LOC_SRAM             (0x100UL)
/** SROM API flash verification option for flash write operation */
#define CY_FLASH_CONFIG_VERIFICATION_EN    ((0x01UL) << 16u)

/** \} group_flash_config_macros */

/** \cond INTERNAL */
#if defined(FLASH_WA_DISABLE)
#define FLASH_START_WA_DISABLE                     1
#define FLASH_RESUME_WA_DISABLE                    1
#endif

/** Command completed with no errors */
#define CY_FLASH_ROMCODE_SUCCESS                   (0xA0000000UL)
/** Invalid device protection state */
#define CY_FLASH_ROMCODE_INVALID_PROTECTION        (0xF0000001UL)
/** Invalid flash page latch address */
#define CY_FLASH_ROMCODE_INVALID_FM_PL             (0xF0000003UL)
/** Invalid flash address */
#define CY_FLASH_ROMCODE_INVALID_FLASH_ADDR        (0xF0000004UL)
/** Row is write protected */
#define CY_FLASH_ROMCODE_ROW_PROTECTED             (0xF0000005UL)
/** Comparison between Page Latches and FM row failed */
#define CY_FLASH_ROMCODE_PL_ROW_COMP_FA            (0xF0000022UL)
/** Command in progress; no error */
#define CY_FLASH_ROMCODE_IN_PROGRESS_NO_ERROR      (0xA0000009UL)
/** Flash operation is successfully initiated */
#define CY_FLASH_IS_OPERATION_STARTED              (0x00000010UL)
/** Flash is under operation */
#define CY_FLASH_IS_BUSY                           (0x00000040UL)
/** IPC structure is already locked by another process */
#define CY_FLASH_IS_IPC_BUSY                       (0x00000080UL)
/** Input parameters passed to Flash API are not valid */
#define CY_FLASH_IS_INVALID_INPUT_PARAMETERS       (0x00000100UL)

/** Result mask */
#define CY_FLASH_RESULT_MASK                       (0x0FFFFFFFUL)
/** Error shift */
#define CY_FLASH_ERROR_SHIFT                       (28UL)
/** No error */
#define CY_FLASH_ERROR_NO_ERROR                    (0xAUL)

/** CM4 Flash Proxy address */
#define CY_FLASH_CM4_FLASH_PROXY_ADDR              (*(Cy_Flash_Proxy *)(0x00000D1CUL))
typedef cy_en_flashdrv_status_t (*Cy_Flash_Proxy)(cy_stc_flash_context_t *context);

/** IPC notify bit for IPC_STRUCT0 (dedicated to flash operation) */
#define CY_FLASH_IPC_NOTIFY_STRUCT0                (0x1UL << CY_IPC_INTR_SYSCALL1)

/** Number of ticks to wait 1 uS */
#define CY_FLASH_TICKS_FOR_1US                     (8U)
/** Slow control register */
#define CY_FLASH_TST_DDFT_SLOW_CTL_REG             (*(reg32 *) 0x40260108U)
/** Slow control register */
#define CY_FLASH_TST_DDFT_FAST_CTL_REG             (*(reg32 *) 0x40260104U)
/** Define to set the IMO to perform a delay after the flash operation started */
#define CY_FLASH_TST_DDFT_SLOW_CTL_MASK            (0x40001F1EUL)
/** Fast control register */
#define CY_FLASH_TST_DDFT_FAST_CTL_MASK            (62U)
/** Slow output register - output disabled */
#define CY_FLASH_CLK_OUTPUT_DISABLED               (0U)

/** Disable delay */
#define CY_FLASH_NO_DELAY                          (0U)

/* The default delay time value */
#define CY_FLASH_DEFAULT_DELAY                     (150UL)
/* Calculates the time in microseconds to wait for the number of the CM0P ticks */
#define CY_FLASH_DELAY_CORRECTIVE(ticks)           ((((uint32)Cy_SysClk_ClkPeriGetDivider() + 1UL) * \
                                                   (Cy_SysClk_ClkSlowGetDivider() + 1UL) * (ticks) * 1000UL)\
                                                   / ((uint32_t)cy_Hfclk0FreqHz / 1000UL))
/* Number of the CM0P ticks for StartWrite function delay corrective time */
#define CY_FLASH_START_WRITE_DELAY_TICKS             (19000UL)
/* Number of the CM0P ticks for StartProgram function delay corrective time */
#define CY_FLASH_START_PROGRAM_DELAY_TICKS           (5250UL)
/* Number of the CM0P ticks for StartErase function delay corrective time */
#define CY_FLASH_START_ERASE_DELAY_TICKS             (8500UL)
/* Delay time for StartWrite function in us */
#define CY_FLASH_START_WRITE_DELAY_TIME              (9800UL + CY_FLASH_DELAY_CORRECTIVE(CY_FLASH_START_WRITE_DELAY_TICKS))
/* Delay time for StartProgram function in us */
#define CY_FLASH_START_PROGRAM_DELAY_TIME            (120UL + CY_FLASH_DELAY_CORRECTIVE(CY_FLASH_START_PROGRAM_DELAY_TICKS))
/* Delay time for StartErase function in us */
#define CY_FLASH_START_ERASE_DELAY_TIME              (1000UL + CY_FLASH_DELAY_CORRECTIVE(CY_FLASH_START_ERASE_DELAY_TICKS))
/** Delay time for Start Write function in us with corrective time */
#define CY_FLASH_START_WRITE_DELAY                 ((CY_SYSLIB_DEVICE_REV_0A == Cy_SysLib_GetDeviceRevision()) ? \
                                                   (CY_FLASH_START_WRITE_DELAY_TIME) : (CY_FLASH_DEFAULT_DELAY))
/** Delay time for Start Program function in us with corrective time */
#define CY_FLASH_START_PROGRAM_DELAY               ((CY_SYSLIB_DEVICE_REV_0A == Cy_SysLib_GetDeviceRevision()) ? \
                                                   (CY_FLASH_START_PROGRAM_DELAY_TIME) : (CY_FLASH_DEFAULT_DELAY))
/** Delay time fot Start Erase function in uS with corrective time */
#define CY_FLASH_START_ERASE_DELAY                 ((CY_SYSLIB_DEVICE_REV_0A == Cy_SysLib_GetDeviceRevision()) ? \
                                                   (CY_FLASH_START_ERASE_DELAY_TIME) : (CY_FLASH_DEFAULT_DELAY))
#define CY_FLASH_RESUME_OPER                       (0xFEU)
#define CY_FLASH_ENTER_WAIT_LOOP                   (0xFFU)
#define CY_FLASH_IPC_CLIENT_ID                     (2U)

/** Semaphore number reserved for flash driver */
#define CY_FLASH_WAIT_SEMA                         (0UL)
/* Semaphore check timeout (in tries) */
#define CY_FLASH_SEMA_WAIT_MAX_TRIES               (150000UL)


typedef struct
{
    uint8_t  clientID;
    uint8_t  pktType;
    uint16_t intrRelMask;
} cy_flash_notify_t;

#if !defined(FLASH_START_WA_DISABLE)
static void Cy_Flash_NotifyHandler(uint32_t * msgPtr);
#endif
#if !(defined(FLASH_START_WA_DISABLE) && defined(FLASH_RESUME_WA_DISABLE))
static void Cy_Flash_RAMDelay(uint32_t microseconds);
#endif

#if (CY_CPU_CORTEX_M0P)
    #define IS_CY_PIPE_FREE(...)       (!Cy_IPC_Drv_IsLockAcquired(Cy_IPC_Drv_GetIpcBaseAddress(CY_IPC_INTR_CYPIPE_EP1)))
    #define NOTIFY_PEER_CORE(a)         Cy_IPC_Pipe_SendMessage(CY_IPC_EP_CYPIPE_CM4_ADDR, CY_IPC_EP_CYPIPE_CM0_ADDR, (a), NULL)
#else
    #define IS_CY_PIPE_FREE(...)       (!Cy_IPC_Drv_IsLockAcquired(Cy_IPC_Drv_GetIpcBaseAddress(CY_IPC_INTR_CYPIPE_EP0)))
    #define NOTIFY_PEER_CORE(a)         Cy_IPC_Pipe_SendMessage(CY_IPC_EP_CYPIPE_CM0_ADDR, CY_IPC_EP_CYPIPE_CM4_ADDR, (a), NULL)
#endif

#if (CY_CPU_CORTEX_M4) && !defined(FLASH_RESUME_WA_DISABLE)
static void Cy_Flash_ResumeIrqHandler(void);
#endif
/** \endcond */


/* Static functions */
static bool Cy_Flash_BoundsCheck(uint32_t flashAddr);
static uint32_t Cy_Flash_PrepareContext(uint32_t rowAddr, const uint32_t *data, uint32_t blocking);
static cy_en_flashdrv_status_t Cy_Flash_ProcessOpcode(uint32_t opcode);
static cy_en_flashdrv_status_t Cy_Flash_OperationStatus(void);
static cy_en_flashdrv_status_t Cy_Flash_SendCmd(uint32_t mode, uint32_t microseconds);

static volatile cy_stc_flash_context_t flashContext;

#if !defined(FLASH_START_WA_DISABLE)
    /*******************************************************************************
    * Function Name: Cy_Flash_NotifyHandler
    ****************************************************************************//**
    *
    * This is the interrupt service routine for the pipe notifications.
    *
    *******************************************************************************/
    typedef struct
    {
        uint32_t maxSema;      /* Maximum semaphores in system */
        uint32_t *arrayPtr;    /* Pointer to semaphores array  */
    } cy_stc_ipc_sema_t;

    #if defined (__ICCARM__)
        #pragma diag_suppress=Ta023
        __ramfunc
    #else
        CY_SECTION(".cy_ramfunc")
    #endif
    static void Cy_Flash_NotifyHandler(uint32_t * msgPtr)
    {
        uint32_t intr;
        static uint32_t semaIndex;
        static uint32_t semaMask;
        static volatile uint32_t *semaPtr;
        static cy_stc_ipc_sema_t *semaStruct;

        cy_flash_notify_t *ipcMsgPtr = (cy_flash_notify_t *)msgPtr;

        if (CY_FLASH_ENTER_WAIT_LOOP == ipcMsgPtr->pktType)
        {
            intr = Cy_SysLib_EnterCriticalSection();

            /* Get pointer to structure */
            semaStruct = (cy_stc_ipc_sema_t *)Cy_IPC_Drv_ReadDataValue(Cy_IPC_Drv_GetIpcBaseAddress( CY_IPC_CHAN_SEMA));

            /* Get the index into the semaphore array and calculate the mask */
            semaIndex = CY_FLASH_WAIT_SEMA / CY_IPC_SEMA_PER_WORD;
            semaMask = (uint32_t)(1ul << (CY_FLASH_WAIT_SEMA - (semaIndex * CY_IPC_SEMA_PER_WORD) ));
            semaPtr = &semaStruct->arrayPtr[semaIndex];

            /* Notification to the Flash driver to start the current operation */
            *semaPtr |= semaMask;

            /* Check a notification from other core to end of waiting */
            while (((*semaPtr) & semaMask) != 0ul)
            {
            }

            Cy_SysLib_ExitCriticalSection(intr);
        }
    }
    #if defined (__ICCARM__)
        #pragma diag_default=Ta023
    #endif
#endif

/*******************************************************************************
* Function Name: Cy_Flash_Init
****************************************************************************//**
*
* Initiates all needed prerequisites to support flash erase/write.
* Should be called from each core.
*
* Requires a call to Cy_IPC_SystemSemaInit() and Cy_IPC_SystemPipeInit() functions
* before use.
*
* This function is called in the SystemInit() function, for proper flash write
* and erase operations. If the default startup file is not used, or the function
* SystemInit() is not called in your project, call the following three functions
* prior to executing any flash or EmEEPROM write or erase operations:
* -# Cy_IPC_SystemSemaInit()
* -# Cy_IPC_SystemPipeInit()
* -# Cy_Flash_Init()
*
*******************************************************************************/
void Cy_Flash_Init(void)
{
    #if (CY_CPU_CORTEX_M4) && !defined(FLASH_RESUME_WA_DISABLE)
        cy_stc_sysint_t flashIntConfig =
        {
            cpuss_interrupt_fm_IRQn,        /* .intrSrc */
            0                               /* .intrPriority */
        };

        (void)Cy_SysInt_Init(&flashIntConfig, &Cy_Flash_ResumeIrqHandler);
        NVIC_EnableIRQ(flashIntConfig.intrSrc);
    #endif

    #if !defined(FLASH_START_WA_DISABLE)
        (void)Cy_IPC_Pipe_RegisterCallback(CY_IPC_EP_CYPIPE_ADDR, &Cy_Flash_NotifyHandler,
                                          (uint32_t)CY_FLASH_IPC_CLIENT_ID);
    #endif
}

/*******************************************************************************
* Function Name: Cy_Flash_SendCmd
****************************************************************************//**
*
* Sends a command to the SROM via the IPC channel. The function is placed to the
* SRAM memory to guarantee successful operation. After an IPC message is sent,
* the function waits for a defined time before exiting the function.
*
* \param mode
* Sets the blocking or non-blocking Flash operation.
*
* \param microseconds
* The number of microseconds to wait before exiting the functions
* in range 0-65535 us.
*
* \return Returns the status of the Flash operation,
* see \ref cy_en_flashdrv_status_t.
*
*******************************************************************************/
#if defined(CY_DEVICE_PSOC6ABLE2) \
    && !defined(CY_PSOC6ABLE2_REV_0A_SUPPORT_DISABLE) \
    && !defined(FLASH_START_WA_DISABLE)
    #if defined (__ICCARM__)
        #pragma diag_suppress=Ta023
        __ramfunc
    #else
        CY_SECTION(".cy_ramfunc")
    #endif
#endif
static cy_en_flashdrv_status_t Cy_Flash_SendCmd(uint32_t mode, uint32_t microseconds)
{
    cy_en_flashdrv_status_t result = CY_FLASH_DRV_IPC_BUSY;

#if !defined(FLASH_START_WA_DISABLE)

    uint32_t semaTryCount = 0uL;
    uint32_t intr;

    CY_ALIGN(4) static cy_flash_notify_t ipcWaitMessage =
    {
        /* .clientID    */ CY_FLASH_IPC_CLIENT_ID,
        /* .pktType     */ CY_FLASH_ENTER_WAIT_LOOP,
        /* .intrRelMask */ 0
    };

    #if (CY_CPU_CORTEX_M0P)
        bool isCM4Powered = (CY_SYS_CM4_STATUS_ENABLED == Cy_SysGetCM4Status());

        if (!isCM4Powered)
        {
            result = CY_FLASH_DRV_SUCCESS;
        }
        else
        {
    #endif
            if (IS_CY_PIPE_FREE())
            {
                if (CY_IPC_SEMA_STATUS_LOCKED != Cy_IPC_Sema_Status(CY_FLASH_WAIT_SEMA))
                {
                    if (CY_IPC_PIPE_SUCCESS == NOTIFY_PEER_CORE(&ipcWaitMessage))
                    {
                        /* Wait for SEMA lock by peer core */
                        while ((CY_IPC_SEMA_STATUS_LOCKED != Cy_IPC_Sema_Status(CY_FLASH_WAIT_SEMA)) && ((semaTryCount < CY_FLASH_SEMA_WAIT_MAX_TRIES)))
                        {
                            /* check for timeout (as maximum tries count) */
                            ++semaTryCount;
                        }

                        if (semaTryCount < CY_FLASH_SEMA_WAIT_MAX_TRIES)
                        {
                            result = CY_FLASH_DRV_SUCCESS;
                        }
                    }
                }
            }
    #if (CY_CPU_CORTEX_M0P)
        }
    #endif

    if (CY_FLASH_DRV_SUCCESS == result)
    {
        /* Notifier is ready, start of the operation */
        intr = Cy_SysLib_EnterCriticalSection();

#endif /* !defined(FLASH_START_WA_DISABLE) */

       /* Tries to acquire the IPC structure and pass the arguments to SROM API */
        if (Cy_IPC_Drv_SendMsgPtr(Cy_IPC_Drv_GetIpcBaseAddress(CY_IPC_CHAN_SYSCALL), CY_FLASH_IPC_NOTIFY_STRUCT0,
                                  (void*)&flashContext) == CY_IPC_DRV_SUCCESS)
        {
            if (mode == CY_FLASH_NON_BLOCKING_MODE)
            {
                #if !defined(FLASH_START_WA_DISABLE)
                    Cy_Flash_RAMDelay(microseconds);
                #endif

                /* The Flash operation is successfully initiated */
                result = CY_FLASH_DRV_OPERATION_STARTED;
            }
            else
            {
                /* Polls whether the IPC is released and the Flash operation is performed */
                do
                {
                    result = Cy_Flash_OperationStatus();
                }
                while (result == CY_FLASH_DRV_OPCODE_BUSY);
            }
        }
        else
        {
            /* The IPC structure is already locked by another process */
            result = CY_FLASH_DRV_IPC_BUSY;
        }

#if !defined(FLASH_START_WA_DISABLE)
    #if (CY_CPU_CORTEX_M0P)
        if (isCM4Powered)
        {
    #endif
            while (CY_IPC_SEMA_SUCCESS != Cy_IPC_Sema_Clear(CY_FLASH_WAIT_SEMA, true))
            {
            }
    #if (CY_CPU_CORTEX_M0P)
        }
    #endif

        Cy_SysLib_ExitCriticalSection(intr);
        /* End of the flash operation */
    }
#endif /* !defined(FLASH_START_WA_DISABLE) */

    return (result);
}
#if defined (__ICCARM__)
    #pragma diag_default=Ta023
#endif


#if !(defined(FLASH_START_WA_DISABLE) && defined(FLASH_RESUME_WA_DISABLE))
    /*******************************************************************************
    * Function Name: Cy_Flash_RAMDelay
    ****************************************************************************//**
    *
    * Wait for a defined time in the SRAM memory region.
    *
    * \param microseconds
    * Delay time in microseconds in range 0-65535 us.
    *
    *******************************************************************************/
    #if defined (__ICCARM__)
        #pragma diag_suppress=Ta023
        __ramfunc
    #else
        CY_SECTION(".cy_ramfunc")
    #endif
    static void Cy_Flash_RAMDelay(uint32_t microseconds)
    {
        uint32_t ticks = (microseconds & 0xFFFFUL) * CY_FLASH_TICKS_FOR_1US;
        if (ticks != CY_FLASH_NO_DELAY)
        {
            CY_FLASH_TST_DDFT_FAST_CTL_REG  = CY_FLASH_TST_DDFT_FAST_CTL_MASK;
            CY_FLASH_TST_DDFT_SLOW_CTL_REG  = CY_FLASH_TST_DDFT_SLOW_CTL_MASK;

            SRSS->CLK_OUTPUT_SLOW = _VAL2FLD(SRSS_CLK_OUTPUT_SLOW_SLOW_SEL0, CY_SYSCLK_MEAS_CLK_IMO) |
                                    _VAL2FLD(SRSS_CLK_OUTPUT_SLOW_SLOW_SEL1, CY_FLASH_CLK_OUTPUT_DISABLED);

            /* Load the down-counter without status bit value */
            SRSS->CLK_CAL_CNT1 = _VAL2FLD(SRSS_CLK_CAL_CNT1_CAL_COUNTER1, ticks);

            /* Make sure that the counter is started */
            (void)Cy_SysClk_ClkMeasurementCountersDone();

            while (!Cy_SysClk_ClkMeasurementCountersDone())
            {
                /* Wait until the counter stops counting */
            }
        }
    }
    #if defined (__ICCARM__)
        #pragma diag_default=Ta023
    #endif
#endif

#if (CY_CPU_CORTEX_M4) && !defined(FLASH_RESUME_WA_DISABLE)

    /* Based on bookmark codes of mxs40srompsoc BROS,002-03298 */
    #define CY_FLASH_PROGRAM_ROW_BOOKMARK        (0x00000001UL)
    #define CY_FLASH_ERASE_ROW_BOOKMARK          (0x00000002UL)
    #define CY_FLASH_WRITE_ROW_ERASE_BOOKMARK    (0x00000003UL)
    #define CY_FLASH_WRITE_ROW_PROGRAM_BOOKMARK  (0x00000004UL)
    #define CY_FLASH_PROGRAM_ROW_DELAY           (150U)
    #define CY_FLASH_ERASE_ROW_DELAY             (150U)
    #define CY_FLASH_WRITE_ROW_ERASE_DELAY       (150U)
    #define CY_FLASH_WRITE_ROW_PROGRAM_DELAY     (150U)

    /*******************************************************************************
    * Function Name: Cy_Flash_ResumeIrqHandler
    ****************************************************************************//**
    *
    * This is the interrupt service routine to make additional processing of the
    * flash operations resume phase.
    *
    *******************************************************************************/
    #if defined (__ICCARM__)
        #pragma diag_suppress=Ta023
        __ramfunc
    #else
        CY_SECTION(".cy_ramfunc")
    #endif
    static void Cy_Flash_ResumeIrqHandler(void)
    {
        CY_ALIGN(4) static cy_flash_notify_t ipcResumeMessage =
        {
            /* .clientID    */ CY_FLASH_IPC_CLIENT_ID,
            /* .pktType     */ CY_FLASH_RESUME_OPER,
            /* .intrRelMask */ 0
        };

        uint32_t bookmark;
        bookmark = FLASHC->FM_CTL.BOOKMARK & 0xffffUL;

        uint32_t cm0s = CPUSS->CM0_STATUS;

        switch (bookmark)
        {
        case CY_FLASH_PROGRAM_ROW_BOOKMARK:
            if (cm0s == (CPUSS_CM0_STATUS_SLEEPING_Msk | CPUSS_CM0_STATUS_SLEEPDEEP_Msk))
            {
                (void)NOTIFY_PEER_CORE(&ipcResumeMessage);
            }
            Cy_Flash_RAMDelay(CY_FLASH_PROGRAM_ROW_DELAY);
            break;
        case CY_FLASH_ERASE_ROW_BOOKMARK:
            if (cm0s == (CPUSS_CM0_STATUS_SLEEPING_Msk | CPUSS_CM0_STATUS_SLEEPDEEP_Msk))
            {
                (void)NOTIFY_PEER_CORE(&ipcResumeMessage);
            }
            Cy_Flash_RAMDelay(CY_FLASH_ERASE_ROW_DELAY);               /* Delay when erase row is finished */
            break;
        case CY_FLASH_WRITE_ROW_ERASE_BOOKMARK:
            if (cm0s == (CPUSS_CM0_STATUS_SLEEPING_Msk | CPUSS_CM0_STATUS_SLEEPDEEP_Msk))
            {
                (void)NOTIFY_PEER_CORE(&ipcResumeMessage);
            }
            Cy_Flash_RAMDelay(CY_FLASH_WRITE_ROW_ERASE_DELAY);         /* Delay when erase phase for row is finished */
            break;
        case CY_FLASH_WRITE_ROW_PROGRAM_BOOKMARK:
            if (cm0s == (CPUSS_CM0_STATUS_SLEEPING_Msk | CPUSS_CM0_STATUS_SLEEPDEEP_Msk))
            {
                (void)NOTIFY_PEER_CORE(&ipcResumeMessage);
            }
            Cy_Flash_RAMDelay(CY_FLASH_WRITE_ROW_PROGRAM_DELAY);
            break;
        default:
            break;
        }
    }
    #if defined (__ICCARM__)
        #pragma diag_default=Ta023
    #endif
#endif /* (CY_CPU_CORTEX_M4) && !defined(FLASH_RESUME_WA_DISABLE) */


/*******************************************************************************
* Function Name: Cy_Flash_EraseRow
****************************************************************************//**
*
* This function erases a single row of flash. Reports success or
* a reason for failure. Does not return until the Write operation is
* complete. Returns immediately and reports a \ref CY_FLASH_DRV_IPC_BUSY error in
* the case when another process is writing to flash or erasing the row.
* User firmware should not enter the Hibernate or Deep-Sleep mode until flash Erase
* is complete. The Flash operation is allowed in Sleep mode.
* During the Flash operation, the device should not be reset, including the
* XRES pin, a software reset, and watchdog reset sources. Also, low-voltage
* detect circuits should be configured to generate an interrupt instead of a
* reset. Otherwise, portions of flash may undergo unexpected changes.
*
* \param rowAddr Address of the flash row number. The number of the flash rows
* is defined by the \ref CY_FLASH_NUMBER_ROWS macro for the selected device.
* The Read-while-Write violation occurs when the flash read operation is
* initiated in the same flash sector where the flash write operation is
* performing. Refer to the device datasheet for the details.
* Address must match row start address.
*
* \return Returns the status of the Flash operation,
* see \ref cy_en_flashdrv_status_t.
*
*******************************************************************************/
cy_en_flashdrv_status_t Cy_Flash_EraseRow(uint32_t rowAddr)
{
    cy_en_flashdrv_status_t result = CY_FLASH_DRV_INVALID_INPUT_PARAMETERS;

    /* Prepares arguments to be passed to SROM API */
    if (Cy_Flash_BoundsCheck(rowAddr) != false)
    {
        flashContext.opcode = CY_FLASH_OPCODE_ERASE_ROW | CY_FLASH_BLOCKING_MODE;
        flashContext.arg1 = rowAddr;
        flashContext.arg2 = 0UL;
        flashContext.arg3 = 0UL;

#if (CY_CPU_CORTEX_M0P)
        result = Cy_Flash_SendCmd(CY_FLASH_BLOCKING_MODE, CY_FLASH_NO_DELAY);
#else
        Cy_Flash_Proxy Cy_Flash_Cm4Proxy;
        Cy_Flash_Cm4Proxy = CY_FLASH_CM4_FLASH_PROXY_ADDR;
        result = Cy_Flash_Cm4Proxy((cy_stc_flash_context_t*)&flashContext);
        result = Cy_Flash_ProcessOpcode((uint32_t)result);

#endif /* (CY_CPU_CORTEX_M0P) */
    }

    return (result);
}


/*******************************************************************************
* Function Name: Cy_Flash_WriteRow
****************************************************************************//**
*
* This function writes an array of data to a single row of flash. This is done
* in three steps - pre-program, erase and then program flash row with the input
* data. Reports success or a reason for failure. Does not return until the Write
* operation is complete.
* Returns immediately and reports a \ref CY_FLASH_DRV_IPC_BUSY error in the case
* when another process is writing to flash. User firmware should not enter the
* Hibernate or Deep-sleep mode until flash Write is complete. The Flash operation
* is allowed in Sleep mode. During the Flash operation, the
* device should not be reset, including the XRES pin, a software
* reset, and watchdog reset sources. Also, low-voltage detect
* circuits should be configured to generate an interrupt
* instead of a reset. Otherwise, portions of flash may undergo
* unexpected changes.
*
* \param rowAddr Address of the flash row number. The number of the flash rows
* is defined by the \ref CY_FLASH_NUMBER_ROWS macro for the selected device.
* The Read-while-Write violation occurs when the flash read operation is
* initiated in the same flash sector where the flash write operation is
* performing. Refer to the device datasheet for the details.
* Address must match row start address.
*
* \param data The pointer to the data which has to be written to flash. The size
* of the data array must be equal to the flash row size. The flash row size for
* the selected device is defined by the \ref CY_FLASH_SIZEOF_ROW macro. Refer to
* the device datasheet for the details.
*
* \return Returns the status of the Flash operation,
* see \ref cy_en_flashdrv_status_t.
*
*******************************************************************************/
cy_en_flashdrv_status_t Cy_Flash_WriteRow(uint32_t rowAddr, const uint32_t* data)
{
    cy_en_flashdrv_status_t result = CY_FLASH_DRV_INVALID_INPUT_PARAMETERS;

    /* Checks whether the input parameters are valid */
    if (Cy_Flash_PrepareContext(rowAddr, data, CY_FLASH_BLOCKING_MODE) != (uint32_t)CY_FLASH_DRV_INVALID_INPUT_PARAMETERS)
    {
#if (CY_CPU_CORTEX_M0P)
        result = Cy_Flash_SendCmd(CY_FLASH_BLOCKING_MODE, CY_FLASH_NO_DELAY);
#else
        Cy_Flash_Proxy Cy_Flash_Cm4Proxy;
        Cy_Flash_Cm4Proxy = CY_FLASH_CM4_FLASH_PROXY_ADDR;
        result = Cy_Flash_Cm4Proxy((cy_stc_flash_context_t*)&flashContext);
        result = Cy_Flash_ProcessOpcode((uint32_t)result);
#endif /* (CY_CPU_CORTEX_M0P) */
    }

    return (result);
}


/*******************************************************************************
* Function Name: Cy_Flash_StartWrite
****************************************************************************//**
*
* Performs pre-program, erase and then starts programming the flash row with
* the input data. Returns immediately and reports a successful start
* or reason for failure. Reports a \ref CY_FLASH_DRV_IPC_BUSY error
* in the case when another process is writing to flash. User
* firmware should not enter the Hibernate or Deep-Sleep mode until
* flash Write is complete. The Flash operation is allowed in Sleep mode.
* During the flash operation, the device should not be reset, including the
* XRES pin, a software reset, and watchdog reset sources. Also, the low-voltage
* detect circuits should be configured to generate an interrupt instead of a reset.
* Otherwise, portions of flash may undergo unexpected changes.
* \note Before reading data from previously programmed/erased flash rows, the
* user must clear the flash cache with the Cy_SysLib_ClearFlashCacheAndBuffer()
* function.
*
* \param rowAddr Address of the flash row number. The number of the flash rows
* is defined by the \ref CY_FLASH_NUMBER_ROWS macro for the selected device.
* The Read-while-Write violation occurs when the flash read operation is
* initiated in the same flash sector where the flash write operation is
* performing. Refer to the device datasheet for the details.
* Address must match row start address.
*
* \param data The pointer to the data to be written to flash. The size
* of the data array must be equal to the flash row size. The flash row size for
* the selected device is defined by the \ref CY_FLASH_SIZEOF_ROW macro. Refer to
* the device datasheet for the details.
*
* \return Returns the status of the Flash operation,
* see \ref cy_en_flashdrv_status_t.
*
*******************************************************************************/
cy_en_flashdrv_status_t Cy_Flash_StartWrite(uint32_t rowAddr, const uint32_t* data)
{
    cy_en_flashdrv_status_t result = CY_FLASH_DRV_INVALID_INPUT_PARAMETERS;

    /* Checks whether the input parameters are valid */
    if (Cy_Flash_PrepareContext(rowAddr, data, CY_FLASH_NON_BLOCKING_MODE) != (uint32_t)CY_FLASH_DRV_INVALID_INPUT_PARAMETERS)
    {
        result = Cy_Flash_SendCmd(CY_FLASH_NON_BLOCKING_MODE, CY_FLASH_START_WRITE_DELAY);
    }

    return (result);
}


/*******************************************************************************
* Function Name: Cy_Flash_IsOperationComplete
****************************************************************************//**
*
* Reports a successful operation result, reason of failure or busy status
* ( \ref CY_FLASH_DRV_OPCODE_BUSY ).
*
* \return Returns the status of the Flash operation (see \ref cy_en_flashdrv_status_t).
*
*******************************************************************************/
cy_en_flashdrv_status_t Cy_Flash_IsOperationComplete(void)
{
    return (Cy_Flash_OperationStatus());
}


/*******************************************************************************
* Function Name: Cy_Flash_StartErase
****************************************************************************//**
*
* Starts erasing a single row of flash. Returns immediately
* and reports a successful start or reason for failure.
* Reports a \ref CY_FLASH_DRV_IPC_BUSY error in the case when IPC structure is locked
* by another process. User firmware should not enter the Hibernate or Deep-Sleep mode until
* flash Erase is complete. The Flash operation is allowed in Sleep mode.
* During the flash operation, the device should not be reset, including the
* XRES pin, a software reset, and watchdog reset sources. Also, the low-voltage
* detect circuits should be configured to generate an interrupt instead of a reset.
* Otherwise, portions of flash may undergo unexpected changes.
* \note Before reading data from previously programmed/erased flash rows, the
* user must clear the flash cache with the Cy_SysLib_ClearFlashCacheAndBuffer()
* function.
*
* \param rowAddr Address of the flash row number. The number of the flash rows
* is defined by the \ref CY_FLASH_NUMBER_ROWS macro for the selected device.
* The Read-while-Write violation occurs when the flash read operation is
* initiated in the same flash sector where the flash erase operation is
* performing. Refer to the device datasheet for the details.
* Address must match row start address.
*
* \return Returns the status of the Flash operation,
* see \ref cy_en_flashdrv_status_t.
*
*******************************************************************************/
cy_en_flashdrv_status_t Cy_Flash_StartErase(uint32_t rowAddr)
{
    cy_en_flashdrv_status_t result = CY_FLASH_DRV_INVALID_INPUT_PARAMETERS;

    /* Prepares arguments to be passed to SROM API */
    if (Cy_Flash_BoundsCheck(rowAddr) != false)
    {
        flashContext.opcode = CY_FLASH_OPCODE_ERASE_ROW;
        flashContext.arg1 = rowAddr;
        flashContext.arg2 = 0UL;
        flashContext.arg3 = 0UL;
        result = Cy_Flash_SendCmd(CY_FLASH_NON_BLOCKING_MODE, CY_FLASH_START_ERASE_DELAY);
    }

    return (result);
}


/*******************************************************************************
* Function Name: Cy_Flash_StartProgram
****************************************************************************//**
*
* Starts writing an array of data to a single row of flash. Returns immediately
* and reports a successful start or reason for failure.
* Reports a \ref CY_FLASH_DRV_IPC_BUSY error if another process is writing
* to flash. The user firmware should not enter Hibernate or Deep-Sleep mode until flash
* Program is complete. The Flash operation is allowed in Sleep mode.
* During the Flash operation, the device should not be reset, including the
* XRES pin, a software reset, and watchdog reset sources. Also, the low-voltage
* detect circuits should be configured to generate an interrupt instead of a reset.
* Otherwise, portions of flash may undergo unexpected changes.\n
* Before calling this function, the target flash region must be erased by
* the StartErase/EraseRow function.\n
* Data to be programmed must be located in the SRAM memory region.
* \note Before reading data from previously programmed/erased flash rows, the
* user must clear the flash cache with the Cy_SysLib_ClearFlashCacheAndBuffer()
* function.
*
* \param rowAddr The address of the flash row number. The number of the flash rows
* is defined by the \ref CY_FLASH_NUMBER_ROWS macro for the selected device.
* The Read-while-Write violation occurs when the Flash Write operation is
* performing. Refer to the device datasheet for the details.
* The address must match the row start address.
*
* \param data The pointer to the data to be written to flash. The size
* of the data array must be equal to the flash row size. The flash row size for
* the selected device is defined by the \ref CY_FLASH_SIZEOF_ROW macro. Refer to
* the device datasheet for the details.
*
* \return Returns the status of the Flash operation,
* see \ref cy_en_flashdrv_status_t.
*
*******************************************************************************/
cy_en_flashdrv_status_t Cy_Flash_StartProgram(uint32_t rowAddr, const uint32_t* data)
{
    cy_en_flashdrv_status_t result = CY_FLASH_DRV_INVALID_INPUT_PARAMETERS;

    /* Prepares arguments to be passed to SROM API */
    if (Cy_Flash_BoundsCheck(rowAddr) != false)
    {
        flashContext.opcode = CY_FLASH_OPCODE_PROGRAM_ROW | CY_FLASH_NON_BLOCKING_MODE;
        flashContext.arg1 = CY_FLASH_CONFIG_DATASIZE | CY_FLASH_DATA_LOC_SRAM;
        flashContext.arg2 = rowAddr;
        flashContext.arg3 = (uint32_t)data;

        result = Cy_Flash_SendCmd(CY_FLASH_NON_BLOCKING_MODE, CY_FLASH_START_PROGRAM_DELAY);
    }

    return (result);
}


/*******************************************************************************
* Function Name: Cy_Flash_RowChecksum
****************************************************************************//**
*
* Returns a checksum value of the specified flash row.
*
* \param rowNum The Checksum is calculated to the flash row.
*
* \param checksumPtr The pointer to the address where checksum is to be stored
*
* \return Returns the status of the Flash operation.
*
*******************************************************************************/
cy_en_flashdrv_status_t Cy_Flash_RowChecksum (uint32_t rowNum, uint32_t* checksumPtr)
{
    cy_en_flashdrv_status_t result = CY_FLASH_DRV_INVALID_INPUT_PARAMETERS;
    uint32_t resTmp;

    /* Checks whether the input parameters are valid */
    if (rowNum < CY_FLASH_NUMBER_ROWS)
    {
        /* Prepares arguments to be passed to SROM API */
        flashContext.opcode = CY_FLASH_OPCODE_CHECKSUM | (rowNum << CY_FLASH_OPCODE_CHECKSUM_ROW_SHIFT);

        /* Tries to acquire the IPC structure and pass the arguments to SROM API */
        if (Cy_IPC_Drv_SendMsgPtr(Cy_IPC_Drv_GetIpcBaseAddress(CY_IPC_CHAN_SYSCALL), CY_FLASH_IPC_NOTIFY_STRUCT0,
                                  (void*)&flashContext) == CY_IPC_DRV_SUCCESS)
        {
            /* Polls whether IPC is released and the Flash operation is performed */
            while (Cy_IPC_Drv_IsLockAcquired(Cy_IPC_Drv_GetIpcBaseAddress(CY_IPC_CHAN_SYSCALL)) != false)
            {
                /* Wait till IPC is released */
            }

            resTmp = flashContext.opcode;

            if((resTmp >> CY_FLASH_ERROR_SHIFT) == CY_FLASH_ERROR_NO_ERROR)
            {
                result = CY_FLASH_DRV_SUCCESS;
                *checksumPtr = flashContext.opcode & CY_FLASH_RESULT_MASK;
            }
            else
            {
                result = Cy_Flash_ProcessOpcode(flashContext.opcode);
            }
        }
        else
        {
            /* The IPC structure is already locked by another process */
            result = CY_FLASH_DRV_IPC_BUSY;
        }
    }

    return (result);
}


/*******************************************************************************
* Function Name: Cy_Flash_CalculateHash
****************************************************************************//**
*
* Returns a hash value of the specified region of flash.
*
* \param data Start the data address.
*
* \param numberOfBytes The hash value is calculated for the number of bytes after the
* start data address (0 - 1 byte, 1- 2 bytes etc).
*
* \param hashPtr The pointer to the address where hash is to be stored
*
* \return Returns the status of the Flash operation.
*
*******************************************************************************/
cy_en_flashdrv_status_t Cy_Flash_CalculateHash (const uint32_t* data, uint32_t numberOfBytes,  uint32_t* hashPtr)
{
    cy_en_flashdrv_status_t result = CY_FLASH_DRV_INVALID_INPUT_PARAMETERS;
    volatile uint32_t resTmp;

    /* Checks whether the input parameters are valid */
    if (data != NULL)
    {
        /* Prepares arguments to be passed to SROM API */
        flashContext.opcode = CY_FLASH_OPCODE_HASH;
        flashContext.arg1 = (uint32_t)data;
        flashContext.arg2 = numberOfBytes;

        /* Tries to acquire the IPC structure and pass the arguments to SROM API */
        if (Cy_IPC_Drv_SendMsgPtr(Cy_IPC_Drv_GetIpcBaseAddress(CY_IPC_CHAN_SYSCALL), CY_FLASH_IPC_NOTIFY_STRUCT0,
                                  (void*)&flashContext) == CY_IPC_DRV_SUCCESS)
        {
            /* Polls whether IPC is released and the Flash operation is performed */
            while (Cy_IPC_Drv_IsLockAcquired(Cy_IPC_Drv_GetIpcBaseAddress(CY_IPC_CHAN_SYSCALL)) != false)
            {
                /* Wait till IPC is released */
            }

            resTmp = flashContext.opcode;

            if((resTmp >> CY_FLASH_ERROR_SHIFT) == CY_FLASH_ERROR_NO_ERROR)
            {
                result = CY_FLASH_DRV_SUCCESS;
                *hashPtr = flashContext.opcode & CY_FLASH_RESULT_MASK;
            }
            else
            {
                result = Cy_Flash_ProcessOpcode(flashContext.opcode);
            }
        }
        else
        {
            /* The IPC structure is already locked by another process */
            result = CY_FLASH_DRV_IPC_BUSY;
        }
    }

    return (result);
}


/*******************************************************************************
* Function Name: Cy_Flash_BoundsCheck
****************************************************************************//**
*
* Returns false if Flash address is out of boundary, otherwise returns true.
*
* \param flashAddr Address to be checked
*
* \return false - out of bound, true - in flash bounds
*
*******************************************************************************/
static bool Cy_Flash_BoundsCheck(uint32_t flashAddr)
{
    bool result = true;

    if ((flashAddr < CY_FLASH_BASE) || (flashAddr >= (CY_FLASH_BASE + CY_FLASH_SIZE)))
    {
        if ((flashAddr < CY_EM_EEPROM_BASE) || (flashAddr >= (CY_EM_EEPROM_BASE + CY_EM_EEPROM_SIZE)))
        {
            if ((flashAddr < SFLASH_BASE) || (flashAddr >= (SFLASH_BASE + SFLASH_SECTION_SIZE)))
            {
                result = false;
            }
        }
    }

    return (result);
}


/*******************************************************************************
* Function Name: Cy_Flash_PrepareContext
****************************************************************************//**
*
* Returns 1 if parameters are correct and initialises context, otherwise returns
* CY_FLASH_DRV_INVALID_INPUT_PARAMETERS.
*
* \param rowAddr Address of the flash row number. The number of the flash rows
* is defined by the \ref CY_FLASH_NUMBER_ROWS macro for the selected device.
* The Read-while-Write violation occurs when the flash read operation is
* initiated in the same flash sector where the flash write operation is
* performing. Refer to the device datasheet for the details.
* Address must match row start address.
*
* \param data The pointer to the data which has to be written to flash. The size
* of the data array must be equal to the flash row size. The flash row size for
* the selected device is defined by the \ref CY_FLASH_SIZEOF_ROW macro. Refer to
* the device datasheet for the details.
*
* \param blocking The parameter of how the context will be executed.
* * 1 - blocking, If this function is called by the CM4 the user code on CM0P
* and the user code on CM4 are blocked until flash write operation is finished.
* If this function is called by the CM0P the user code on CM4 is not blocked
* and the user code on CM0P is blocked until flash write operation is finished.
* * 0 - non-blocking, the user code is not blocked while flash write operation
* is executed.
*
* \return 1 - Context ready, CY_FLASH_DRV_INVALID_INPUT_PARAMETERS - Wrong
* arguments
*
*******************************************************************************/
static uint32_t Cy_Flash_PrepareContext(uint32_t rowAddr, const uint32_t* data, uint32_t blocking)
{
    uint32_t result;

    if ((Cy_Flash_BoundsCheck(rowAddr) == false) || (NULL == data))
    {
        result = (uint32_t)CY_FLASH_DRV_INVALID_INPUT_PARAMETERS;
    }
    else
    {
        if ((rowAddr % CY_FLASH_SIZEOF_ROW) != 0UL)
        {
            result = (uint32_t)CY_FLASH_DRV_INVALID_INPUT_PARAMETERS;
        }
        else
        {
            /* Prepares arguments to be passed to SROM API */
            flashContext.opcode = CY_FLASH_OPCODE_WRITE_ROW | blocking;
            flashContext.arg1 = CY_FLASH_CONFIG_DATASIZE;
            flashContext.arg2 = rowAddr;
            flashContext.arg3 = (uint32_t)data;

            result = 1UL;
        }
    }

    return (result);
}


/*******************************************************************************
* Function Name: Cy_Flash_ProcessOpcode
****************************************************************************//**
*
* Converts System Call returns to the Flash driver return defines.
*
* \param opcode The value returned by the System Call.
*
* \return Flash driver return.
*
*******************************************************************************/
static cy_en_flashdrv_status_t Cy_Flash_ProcessOpcode(uint32_t opcode)
{
    cy_en_flashdrv_status_t result;

    switch (opcode)
    {
        case 0UL:
        {
            result = CY_FLASH_DRV_SUCCESS;
            break;
        }
        case CY_FLASH_ROMCODE_SUCCESS:
        {
            result = CY_FLASH_DRV_SUCCESS;
            break;
        }
        case CY_FLASH_ROMCODE_INVALID_PROTECTION:
        {
            result = CY_FLASH_DRV_INV_PROT;
            break;
        }
        case CY_FLASH_ROMCODE_INVALID_FM_PL:
        {
            result = CY_FLASH_DRV_INVALID_FM_PL;
            break;
        }
        case CY_FLASH_ROMCODE_INVALID_FLASH_ADDR:
        {
            result = CY_FLASH_DRV_INVALID_FLASH_ADDR;
            break;
        }
        case CY_FLASH_ROMCODE_ROW_PROTECTED:
        {
            result = CY_FLASH_DRV_ROW_PROTECTED;
            break;
        }
        case CY_FLASH_ROMCODE_IN_PROGRESS_NO_ERROR:
        {
            result = CY_FLASH_DRV_PROGRESS_NO_ERROR;
            break;
        }
        case (uint32_t)CY_FLASH_DRV_INVALID_INPUT_PARAMETERS:
        {
            result = CY_FLASH_DRV_INVALID_INPUT_PARAMETERS;
            break;
        }
        case CY_FLASH_IS_OPERATION_STARTED :
        {
            result = CY_FLASH_DRV_OPERATION_STARTED;
            break;
        }
        case CY_FLASH_IS_BUSY :
        {
            result = CY_FLASH_DRV_OPCODE_BUSY;
            break;
        }
        case CY_FLASH_IS_IPC_BUSY :
        {
            result = CY_FLASH_DRV_IPC_BUSY;
            break;
        }
        case CY_FLASH_IS_INVALID_INPUT_PARAMETERS :
        {
            result = CY_FLASH_DRV_INVALID_INPUT_PARAMETERS;
            break;
        }
        default:
        {
            result = CY_FLASH_DRV_ERR_UNC;
            break;
        }
    }

    return (result);
}


/*******************************************************************************
* Function Name: Cy_Flash_OperationStatus
****************************************************************************//**
*
* Checks the status of the Flash Operation, and returns it.
*
* \return Returns the status of the Flash operation
* (see \ref cy_en_flashdrv_status_t).
*
*******************************************************************************/
static cy_en_flashdrv_status_t Cy_Flash_OperationStatus(void)
{
    cy_en_flashdrv_status_t result = CY_FLASH_DRV_OPCODE_BUSY;

    /* Checks if the IPC structure is not locked */
    if (Cy_IPC_Drv_IsLockAcquired(Cy_IPC_Drv_GetIpcBaseAddress(CY_IPC_CHAN_SYSCALL)) == false)
    {
        /* The result of SROM API calling is returned to the driver context */
        result = Cy_Flash_ProcessOpcode(flashContext.opcode);
    }

    return (result);
}


/*******************************************************************************
* Function Name: Cy_Flash_GetExternalStatus
****************************************************************************//**
*
* This function handles the case where a module such as security image captures
* a system call from this driver and reports its own status or error code,
* for example protection violation. In that case, a function from this
* driver returns an unknown error (see \ref cy_en_flashdrv_status_t). After receipt
* of an unknown error, the user may call this function to get the status
* of the capturing module.
*
* The user is responsible for parsing the content of the returned value
* and casting it to the appropriate enumeration.
*
* \return
* The error code that was stored in the opcode variable.
*
*******************************************************************************/
uint32_t Cy_Flash_GetExternalStatus(void)
{
    return (flashContext.opcode);
}


/* [] END OF FILE */
