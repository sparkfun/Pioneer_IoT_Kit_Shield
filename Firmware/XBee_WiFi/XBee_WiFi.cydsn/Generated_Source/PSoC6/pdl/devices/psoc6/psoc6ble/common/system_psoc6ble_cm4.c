/***************************************************************************//**
* \file system_psoc6ble_cm4.c
* \version 1.0
*
* The device system-source file.
*
********************************************************************************
* \copyright
* Copyright 2016-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "system_psoc6ble_cm4.h"
#include "cy_device_headers.h"

#include "ipc/cy_ipc_drv.h"
#include "ipc/cy_ipc_pipe.h"
#include "ipc/cy_ipc_lock.h"
#include "sysint/cy_sysint.h"

/*******************************************************************************
* Inter Process Communication (IPC)
*******************************************************************************/
cy_stc_ipc_pipe_ep_t cy_ipc_pipe_sysEpArray[CPUSS_IPC_IPC_NR];
uint32_t ipcLockArray[CY_IPC_LOCK_COUNT/CY_IPC_LOCKS_PER_WORD];
cy_ipc_pipe_callback_ptr_t  ipc_pipe_cyPipeCbArray[CY_IPC_CYPIPE_CLIENT_CNT];


/*******************************************************************************
* SystemCoreClockUpdate()
*******************************************************************************/

/** Default HFClk frequency in Hz */
#define CY_HFCLK0_FREQ_HZ_DEFAULT       ( 8000000UL)

/** Default PeriClk frequency in Hz */
#define CY_PERICLK_FREQ_HZ_DEFAULT      (4000000UL)

/** Default SlowClk system core frequency in Hz */
#define CY_SYSTEM_CLOCK_FREQ_HZ_DEFAULT (8000000UL)


#define CY_IMO_FREQ_HZ                  ( 8000000UL)
#ifndef CY_EXT_FREQ_HZ
    #define CY_EXT_FREQ_HZ              (24000000UL)
#endif
#ifndef CY_ECO_FREQ_HZ
    #define CY_ECO_FREQ_HZ              (24000000UL)
#endif

#if defined (CY_IP_MXBLESS) && (CY_IP_MXBLESS == 1UL)
    #ifndef CY_ALTHF_FREQ_HZ
        #define CY_ALTHF_FREQ_HZ        (32000000UL)
    #endif
    uint32_t cy_BleEcoClockFreqHz = CY_ALTHF_FREQ_HZ;
#endif /* defined (CY_IP_MXBLESS) && (CY_IP_MXBLESS == 1UL) */

#define CY_ROOT_PATH_SRC_IMO            (0UL)
#define CY_ROOT_PATH_SRC_EXT            (1UL)
#define CY_ROOT_PATH_SRC_ECO            (2UL)
#define CY_ROOT_PATH_SRC_ALTHF          (3UL)

/** Holds the SlowClk system core clock, which is the system clock frequency supplied to the SysTick timer and the
* processor core clock. This variable can be used by debuggers to query the frequency of the debug timer or to configure
* the trace clock speed.
*
* \attention Compilers must be configured to avoid removing this variable in case the application program is not using
* it. Debugging systems require the variable to be physically present in memory so that it can be examined to configure
* the debugger. */
uint32_t SystemCoreClock = CY_SYSTEM_CLOCK_FREQ_HZ_DEFAULT;

/** Holds the HFClk0 clock frequency. Updated by \ref SystemCoreClockUpdate(). */
uint32_t cy_Hfclk0FreqHz  = CY_HFCLK0_FREQ_HZ_DEFAULT;

/** Holds the PeriClk clock frequency. Updated by \ref SystemCoreClockUpdate(). */
uint32_t cy_PeriClkFreqHz = CY_PERICLK_FREQ_HZ_DEFAULT;


/* Do not use these definitions directly in your application */
#define CY_DELAY_MS_OVERFLOW_THRESHOLD  (0x8000u)
#define CY_DELAY_1K_THRESHOLD           (1000u)
#define CY_DELAY_1K_MINUS_1_THRESHOLD   (CY_DELAY_1K_THRESHOLD - 1u)
#define CY_DELAY_1M_THRESHOLD           (1000000u)
#define CY_DELAY_1M_MINUS_1_THRESHOLD   (CY_DELAY_1M_THRESHOLD - 1u)
uint32_t cy_delayFreqHz   = CY_SYSTEM_CLOCK_FREQ_HZ_DEFAULT;
uint32_t cy_delayFreqKhz  = (CY_SYSTEM_CLOCK_FREQ_HZ_DEFAULT + CY_DELAY_1K_MINUS_1_THRESHOLD) / CY_DELAY_1K_THRESHOLD;
uint8_t cy_delayFreqMhz  = (uint8_t)((CY_SYSTEM_CLOCK_FREQ_HZ_DEFAULT + CY_DELAY_1M_MINUS_1_THRESHOLD) / CY_DELAY_1M_THRESHOLD);
uint32_t cy_delay32kMs    = CY_DELAY_MS_OVERFLOW_THRESHOLD * ((CY_SYSTEM_CLOCK_FREQ_HZ_DEFAULT + CY_DELAY_1K_MINUS_1_THRESHOLD) /
                        CY_DELAY_1K_THRESHOLD);


/*******************************************************************************
* Table Of Content (TOC)
*******************************************************************************/
typedef struct {
    uint32_t    securityImageAddr;  /**  Address of security image               */
    uint32_t    flashBootAddr;      /**  Address of Flash Boot stored in SFLASH  */
    uint32_t    flashBootSize;      /**  Size of Flash Boot (in bytes)           */
    uint32_t    objectOneAddr;      /**  Address of next object (0 if none)      */
    uint32_t    objectOneSize;      /**  Size of next object (0 if none)         */
    uint32_t    objectTwoAddr;      /**  Address of next object (0 if none)      */
    uint32_t    objectTwoSize;      /**  Size of next object (0 if none)         */
    uint32_t    objectThreeAddr;    /**  Address of next object (0 if none)      */
    uint32_t    objectThreeSize;    /**  Size of next object (0 if none)         */
    uint32_t    userAppAddr;        /**  Address of start of User Application    */
    uint32_t    userAppSize;        /**  Size of the User Application            */
    uint32_t    keyStorageAddr;     /**  Address of Key Storage Flash Blocks.    */
    uint32_t    keyStorageSize;     /**  Size of key storage area in bytes.      */
    uint32_t    smifConfigAddr;     /**  SMIF Configuration Table                */
    uint32_t    reserved[113u];
    uint32_t    crc;                /** 0x1FC   CRC16-CCITT                      */
} Cy_TOC_Type;


#define CY_TOC_BASE                     (0x100FFE00UL)
#define CY_TOC                          ((Cy_TOC_Type*) CY_TOC_BASE)

#ifndef CY_TOC_FLASH_BOOT_ADDR
    #define CY_TOC_FLASH_BOOT_ADDR      (0x160020D8UL)
#endif

#ifndef CY_TOC_FLASH_BOOT_SIZE
    #define CY_TOC_FLASH_BOOT_SIZE      (0x2000UL)
#endif

#ifndef CY_TOC_SMIF_CONFIG_ADDR
    #define CY_TOC_SMIF_CONFIG_ADDR     (0xFFFFFFFFUL)
#endif

#ifndef CY_TOC_USER_APP_ADDR
    #define CY_TOC_USER_APP_ADDR        (0x10000000UL)
#endif

#ifndef CY_TOC_USER_APP_SIZE
    #define CY_TOC_USER_APP_SIZE        (0x80000UL)
#endif

#ifdef CY_TOC_PRESENT
    #if defined(__GNUC__) || defined(__ARMCC_VERSION)
    __attribute__ ((__section__(".cy_toc"), used))
    #elif defined(__ICCARM__)
    #pragma  location=".cy_toc"
    #endif
    Cy_TOC_Type cyToc = {
        .flashBootAddr = CY_TOC_FLASH_BOOT_ADDR,
        .flashBootSize = CY_TOC_FLASH_BOOT_SIZE,
        .smifConfigAddr = CY_TOC_SMIF_CONFIG_ADDR,
        .userAppAddr = CY_TOC_USER_APP_ADDR,
        .userAppSize = CY_TOC_USER_APP_SIZE
    };
#endif


/* SCB->CPACR */
#define SCB_CPACR_CP10_CP11_ENABLE      (0xFUL << 20u)


/*******************************************************************************
* SystemInit()
*******************************************************************************/
#define CY_WDT_LOCK_BIT0                ((uint32_t)0x01u << 30u)
#define CY_WDT_LOCK_BIT1                ((uint32_t)0x01u << 31u)


/*******************************************************************************
* Function Name: SystemInit
****************************************************************************//**
*
* Initializes the system.
*
*******************************************************************************/
void SystemInit(void)
{
    /* Unlock and disable WDT */
    SRSS->WDT_CTL = ((SRSS->WDT_CTL & (uint32_t)(~SRSS_WDT_CTL_WDT_LOCK_Msk)) | CY_WDT_LOCK_BIT0);
    SRSS->WDT_CTL = (SRSS->WDT_CTL | CY_WDT_LOCK_BIT1);
    SRSS->WDT_CTL &= (~ (uint32_t) SRSS_WDT_CTL_WDT_EN_Msk);


    Cy_SystemInit();
    SystemCoreClockUpdate();
    Cy_SystemInitIpc();
    Cy_SMIF_StartupCallBack();
}


/*******************************************************************************
* Function Name: Cy_SystemInit
****************************************************************************//**
*
* The function is called during device startup. Once project compiled as part of
* the PSoC Creator project, the Cy_SystemInit() function is generated by the
* PSoC Creator.
*
* The function generated by PSoC Creator performs all of the necessary device
* configuration based on the design settings.  This includes settings from the
* Design Wide Resources (DWR) such as Clocks and Pins as well as any component
* configuration that is necessary.
*
*******************************************************************************/
__WEAK void Cy_SystemInit(void)
{
     /* Empty weak function. The actual implementation to be in the PSoC Creator
      * generated strong function.
     */
}


/*******************************************************************************
* Function Name: Cy_SMIF_StartupCallBack
****************************************************************************//**
*
* The function is called during device startup. Once SMIF functionality is
* required, user should implement the function as described by the SMIF
* documentation.
*
******************************************************************************/
__WEAK void Cy_SMIF_StartupCallBack(void)
{
     /* Empty weak function. The actual implementation to be in the user's
      * strong function.
     */
}


/*******************************************************************************
* Function Name: SystemCoreClockUpdate
****************************************************************************//**
*
* Get Core Clock Frequency.
*
* Update \ref SystemCoreClock, \ref cy_Hfclk0FreqHz, and \ref cy_PeriClkFreqHz.
*
*******************************************************************************/
void SystemCoreClockUpdate (void)
{
    uint32_t srcFreqHz;
    uint32_t pathFreqHz;
    uint32_t fastClkDiv;
    uint32_t periClkDiv;
    uint32_t rootPath;
    uint32_t srcClk;

    /* Get root path clock for the high-frequency clock # 0 */
    rootPath = _FLD2VAL(SRSS_CLK_ROOT_SELECT_ROOT_MUX, SRSS->CLK_ROOT_SELECT[0u]);

    /* Get source of the root path clock */
    srcClk = _FLD2VAL(SRSS_CLK_PATH_SELECT_PATH_MUX, SRSS->CLK_PATH_SELECT[rootPath]);

    /* Get frequency of the source */
    switch (srcClk)
    {
    case CY_ROOT_PATH_SRC_IMO:
        srcFreqHz = CY_IMO_FREQ_HZ;
    break;

    case CY_ROOT_PATH_SRC_EXT:
        srcFreqHz = CY_EXT_FREQ_HZ;
    break;

    case CY_ROOT_PATH_SRC_ECO:
        srcFreqHz = CY_ECO_FREQ_HZ;
    break;

#if defined (CY_IP_MXBLESS) && (CY_IP_MXBLESS == 1UL)
    case CY_ROOT_PATH_SRC_ALTHF:
        srcFreqHz = cy_BleEcoClockFreqHz;
    break;
#endif /* defined (CY_IP_MXBLESS) && (CY_IP_MXBLESS == 1UL) */

    default:
        srcFreqHz = CY_EXT_FREQ_HZ;
    break;
    }

    if (rootPath == 0UL)
    {
        /* FLL */
        bool fllLocked       = ( 0UL != _FLD2VAL(SRSS_CLK_FLL_STATUS_LOCKED, SRSS->CLK_FLL_STATUS));
        bool fllOutputOutput = ( 3UL == _FLD2VAL(SRSS_CLK_FLL_CONFIG3_BYPASS_SEL, SRSS->CLK_FLL_CONFIG3));
        bool fllOutputAuto   = ((0UL == _FLD2VAL(SRSS_CLK_FLL_CONFIG3_BYPASS_SEL, SRSS->CLK_FLL_CONFIG3)) ||
                                (1UL == _FLD2VAL(SRSS_CLK_FLL_CONFIG3_BYPASS_SEL, SRSS->CLK_FLL_CONFIG3)));
        if ((fllOutputAuto && fllLocked) || fllOutputOutput)
        {
            uint32_t fllMult;
            uint32_t refDiv;
            uint32_t outputDiv;

            fllMult = _FLD2VAL(SRSS_CLK_FLL_CONFIG_FLL_MULT, SRSS->CLK_FLL_CONFIG);
            refDiv  = _FLD2VAL(SRSS_CLK_FLL_CONFIG2_FLL_REF_DIV, SRSS->CLK_FLL_CONFIG2);
            outputDiv = _FLD2VAL(SRSS_CLK_FLL_CONFIG_FLL_OUTPUT_DIV, SRSS->CLK_FLL_CONFIG) + 1UL;

            pathFreqHz = ((srcFreqHz / refDiv) * fllMult) / outputDiv;
        }
        else
        {
            pathFreqHz = srcFreqHz;
        }
    }
    else if (rootPath == 1UL)
    {
        /* PLL */
        bool pllLocked       = ( 0UL != _FLD2VAL(SRSS_CLK_PLL_STATUS_LOCKED,     SRSS->CLK_PLL_STATUS[0UL]));
        bool pllOutputOutput = ( 3UL == _FLD2VAL(SRSS_CLK_PLL_CONFIG_BYPASS_SEL, SRSS->CLK_PLL_CONFIG[0UL]));
        bool pllOutputAuto   = ((0UL == _FLD2VAL(SRSS_CLK_PLL_CONFIG_BYPASS_SEL, SRSS->CLK_PLL_CONFIG[0UL])) ||
                                (1UL == _FLD2VAL(SRSS_CLK_PLL_CONFIG_BYPASS_SEL, SRSS->CLK_PLL_CONFIG[0UL])));
        if ((pllOutputAuto && pllLocked) || pllOutputOutput)
        {
            uint32_t feedbackDiv;
            uint32_t referenceDiv;
            uint32_t outputDiv;

            feedbackDiv  = _FLD2VAL(SRSS_CLK_PLL_CONFIG_FEEDBACK_DIV,  SRSS->CLK_PLL_CONFIG[0UL]);
            referenceDiv = _FLD2VAL(SRSS_CLK_PLL_CONFIG_REFERENCE_DIV, SRSS->CLK_PLL_CONFIG[0UL]);
            outputDiv    = _FLD2VAL(SRSS_CLK_PLL_CONFIG_OUTPUT_DIV,    SRSS->CLK_PLL_CONFIG[0UL]);

            pathFreqHz = ((srcFreqHz * feedbackDiv) / referenceDiv) / outputDiv;

        }
        else
        {
            pathFreqHz = srcFreqHz;
        }
    }
    else
    {
        /* Direct */
        pathFreqHz = srcFreqHz;
    }

    /* Get frequency after hf_clk pre-divider */
    pathFreqHz = pathFreqHz >> _FLD2VAL(SRSS_CLK_ROOT_SELECT_ROOT_DIV, SRSS->CLK_ROOT_SELECT[0u]);
    cy_Hfclk0FreqHz = pathFreqHz;

    /* Fast Clock Divider */
    fastClkDiv = 1u + _FLD2VAL(CPUSS_CM4_CLOCK_CTL_FAST_INT_DIV, CPUSS->CM4_CLOCK_CTL);

    /* Peripheral Clock Divider */
    periClkDiv = 1u + _FLD2VAL(CPUSS_CM0_CLOCK_CTL_PERI_INT_DIV, CPUSS->CM0_CLOCK_CTL);
    cy_PeriClkFreqHz = pathFreqHz / periClkDiv;

    pathFreqHz = pathFreqHz / fastClkDiv;
    SystemCoreClock = pathFreqHz;

    /* Sets clock frequency for Delay API */
    cy_delayFreqHz = SystemCoreClock;
    cy_delayFreqMhz = (uint8_t)((cy_delayFreqHz + CY_DELAY_1M_MINUS_1_THRESHOLD) / CY_DELAY_1M_THRESHOLD);
    cy_delayFreqKhz = (cy_delayFreqHz + CY_DELAY_1K_MINUS_1_THRESHOLD) / CY_DELAY_1K_THRESHOLD;
    cy_delay32kMs   = CY_DELAY_MS_OVERFLOW_THRESHOLD * cy_delayFreqKhz;
}


/*******************************************************************************
* Function Name: Cy_SystemInitIpcCyPipeIsr
****************************************************************************//**
*
* This is the Interrupt Service Routine for the Cypress Pipe.
*
*******************************************************************************/
void Cy_SystemInitIpcCyPipeIsr(void)
{
    Cy_IPC_PIPE_ExecCallback(&cy_ipc_pipe_sysEpArray[CY_IPC_EP_CYPIPE_ADDR]);
}


/*******************************************************************************
* Function Name: Cy_SystemInitIpc
****************************************************************************//**
*
* Performs initial IPC configuration.
*
*******************************************************************************/
void Cy_SystemInitIpc(void)
{
    cy_stc_sysint_t ipc_intr_cypipeConfig;

    Cy_IPC_PIPE_Config(cy_ipc_pipe_sysEpArray);

    #if (CY_CPU_CORTEX_M0P)
        /* Initialize the lock subsystem. Should be called only on one CPU. */
        (void) Cy_IPC_LOCK_Init(CY_IPC_LOCK_COUNT, ipcLockArray);
    #endif  /* (CY_CPU_CORTEX_M0P) */

    /* Initialize the pipe endpoints */
    Cy_IPC_PIPE_Init(CY_IPC_EP_CYPIPE_ADDR, ipc_pipe_cyPipeCbArray, CY_IPC_CYPIPE_CLIENT_CNT, CY_IPC_CYPIPE_CONFIG);

    /* Configure interrupts */
    ipc_intr_cypipeConfig.intrSrc      = CY_IPC_INTR_CYPIPE_SRC;
    ipc_intr_cypipeConfig.intrCm0p     = (IRQn_CM0P_Type) CY_IPC_INTR_CYPIPE_CM0SRC;
    ipc_intr_cypipeConfig.intrPriority = CY_IPC_INTR_CYPIPE_PRIO;

    Cy_SysInt_Init(&ipc_intr_cypipeConfig, &Cy_SystemInitIpcCyPipeIsr);
    #if (CY_CPU_CORTEX_M0P)
        NVIC_EnableIRQ((IRQn_Type)ipc_intr_cypipeConfig.intrCm0p);
    #else
        NVIC_EnableIRQ((IRQn_Type)ipc_intr_cypipeConfig.intrSrc);
    #endif
}


/*******************************************************************************
* Function Name: Cy_SystemInitFpuEnable
****************************************************************************//**
*
* Enables the FPU if it is used.
*
*******************************************************************************/
void Cy_SystemInitFpuEnable(void)
{
    #if defined (__FPU_USED) && (__FPU_USED == 1U)
        uint32_t  interruptState;
        interruptState = Cy_SaveIRQ();
        SCB->CPACR |= SCB_CPACR_CP10_CP11_ENABLE;
        __DSB();
        __ISB();
        Cy_RestoreIRQ(interruptState);
    #endif /* (__FPU_USED) && (__FPU_USED == 1U) */
}

/* [] END OF FILE */
