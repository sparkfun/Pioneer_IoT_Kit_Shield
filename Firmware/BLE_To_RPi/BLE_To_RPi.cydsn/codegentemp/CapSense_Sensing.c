/***************************************************************************//**
* \file CapSense_Sensing.c
* \version 1.0
*
* \brief
*   This file contains the source of functions common for
*   different sensing methods.
*
* \see CapSense v1.0 Datasheet
*
*//*****************************************************************************
* Copyright (2016-2017), Cypress Semiconductor Corporation.
********************************************************************************
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
*******************************************************************************/

#include <stdlib.h>
#include "cyfitter.h"
#include "gpio/cy_gpio.h"
#include "cyfitter_sysint_cfg.h"
#include "CapSense_ModClk.h"
#include "CapSense_Configuration.h"
#include "CapSense_Structure.h"
#include "CapSense_Sensing.h"
#if (CapSense_ENABLE == CapSense_CSX_EN)
    #include "CapSense_SensingCSX_LL.h"
#endif /* (CapSense_ENABLE == CapSense_CSX_EN) */
#if (CapSense_ENABLE == CapSense_CSD_EN)
    #include "CapSense_SensingCSD_LL.h"
#endif /* (CapSense_ENABLE == CapSense_CSD_EN) */
#if (CapSense_CSD_SS_DIS != CapSense_CSD_AUTOTUNE)
    #include "CapSense_SmartSense_LL.h"
#endif  /* (CapSense_CSD_SS_DIS != CapSense_CSD_AUTOTUNE) */
#if (CapSense_ENABLE == CapSense_ADC_EN)
    #include "CapSense_Adc.h"
#endif /* (CapSense_ENABLE == CapSense_ADC_EN) */
#if (CapSense_ENABLE == CapSense_SELF_TEST_EN)
    #include "CapSense_SelfTest.h"
#endif

/***************************************
* API Constants
***************************************/

#define CapSense_WIDGET_NUM_32                          (32u)
#define CapSense_WIDGET_NUM_32_DIV_SHIFT                (5u)
#define CapSense_WIDGET_NUM_32_MASK                     (0x0000001FLu)
#define CapSense_CALIBRATION_RESOLUTION                 (12u)
#define CapSense_COARSE_TRIM_THRESHOLD_1                (40u)
#define CapSense_COARSE_TRIM_THRESHOLD_2                (215u)
#define CapSense_FREQUENCY_OFFSET_5                     (20u)
#define CapSense_FREQUENCY_OFFSET_10                    (40u)
#define CapSense_CALIBRATION_FREQ_KHZ                   (1500u)
#define CapSense_CALIBRATION_MD                         (2u)
#define CapSense_MIN_IMO_FREQ_KHZ                       (6000u)
#define CapSense_CSD_AUTOTUNE_CAL_LEVEL                 (CapSense_CSD_RAWCOUNT_CAL_LEVEL)
#define CapSense_CP_MIN                                 (0u)
#define CapSense_CP_MAX                                 (65000Lu)
#define CapSense_CP_ERROR                               (4000Lu)
#define CapSense_CLK_SOURCE_LFSR_SCALE_OFFSET           (4u)

#if (CapSense_CLK_SOURCE_DIRECT != CapSense_CSD_SNS_CLK_SOURCE)
    #define CapSense_PRS_FACTOR_DIV                     (2u)
#else
    #define CapSense_PRS_FACTOR_DIV                     (0u)
#endif /* (CapSense_CLK_SOURCE_DIRECT != CapSense_CSD_SNS_CLK_SOURCE) */

#define CapSense_FLIP_FLOP_DIV                          (1u)

#define CapSense_MOD_CSD_CLK_12000KHZ                   (12000uL)
#define CapSense_MOD_CSD_CLK_24000KHZ                   (24000uL)
#define CapSense_MOD_CSD_CLK_48000KHZ                   (48000uL)

#define CapSense_P6_MAX_FLL_FREQ_HZ                     (150000000uL)
#define CapSense_P6_MAX_PLL_FREQ_HZ                     (100000000uL)

#define CapSense_P6_MIN_FLL_FREQ_HZ                     (1000000uL)
#define CapSense_P6_MIN_PLL_FREQ_HZ                     (1000000uL)

#define CapSense_P6_PLL_INDEX                           (0uL)
#define CapSense_P6_PERI_SRC_HFCLK                      (0uL)

#if ((CapSense_CLK_SOURCE_PRS8 == CapSense_CSD_SNS_CLK_SOURCE) || \
    (CapSense_CLK_SOURCE_PRS12 == CapSense_CSD_SNS_CLK_SOURCE) || \
    (CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSD_SNS_CLK_SOURCE))
    #define CapSense_FACTOR_FILTER_DELAY_12MHZ          (2u)
#else
    #define CapSense_FACTOR_FILTER_DELAY_12MHZ          (4u)
#endif /* (CapSense_CLK_SOURCE_DIRECT != CapSense_CSD_SNS_CLK_SOURCE) */

#define CapSense_FACTOR_MOD_SNS                         (8u)
#define CapSense_UINT8_MAX_VAL                          (0xFFu)
#define CapSense_MSB_OFFSET                             (8u)

/*****************************************************************************/
/* Enumeration types definition                                               */
/*****************************************************************************/

typedef enum
{
    CapSense_RES_PULLUP_E   = 0x02u,
    CapSense_RES_PULLDOWN_E = 0x03u
} CapSense_PORT_TEST_DM;

typedef enum
{
    CapSense_STS_RESET      = 0x01u,
    CapSense_STS_NO_RESET   = 0x02u
} CapSense_TEST_TYPE;


/*******************************************************************************
* Static Function Prototypes
*******************************************************************************/
/**
* \if SECTION_CAPSENSE_INTERNAL
* \addtogroup group_capsense_internal
* \{
*/

#if ((CapSense_ENABLE == CapSense_CSD_CSX_EN) || \
     (CapSense_ENABLE == CapSense_SELF_TEST_EN) || \
     (CapSense_ENABLE == CapSense_ADC_EN))
    #if (CapSense_ENABLE == CapSense_CSD_EN)
        static void CapSense_SsCSDDisableMode(void);
    #endif /* (CapSense_ENABLE == CapSense_CSD_EN) */
    #if (CapSense_ENABLE == CapSense_CSX_EN)
        static void CapSense_SsDisableCSXMode(void);
    #endif /* (CapSense_ENABLE == CapSense_CSX_EN) */
#endif /* ((CapSense_ENABLE == CapSense_CSD_CSX_EN) || \
           (CapSense_ENABLE == CapSense_SELF_TEST_EN) || \
           (CapSense_ENABLE == CapSense_ADC_EN)) */
#if (CapSense_CSD_SS_DIS != CapSense_CSD_AUTOTUNE)
    static void CapSense_SsSetDirectClockMode(void);
#endif /* (CapSense_CSD_SS_DIS != CapSense_CSD_AUTOTUNE) */

#if(((CapSense_ENABLE == CapSense_CSX_EN) && \
     (CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSX_TX_CLK_SOURCE)) ||\
    ((CapSense_ENABLE == CapSense_CSD_EN) && \
     (CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSD_SNS_CLK_SOURCE)))
    __STATIC_INLINE uint8 CapSense_SsCalcLfsrSize(uint32 snsClkDivider, uint32 conversionsNum);
    __STATIC_INLINE uint8 CapSense_SsCalcLfsrScale(uint32 snsClkDivider, uint8 lfsrSize);
#endif /* (((CapSense_ENABLE == CapSense_CSX_EN) && \
            (CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSX_TX_CLK_SOURCE)) ||\
           ((CapSense_ENABLE == CapSense_CSD_EN) && \
            (CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSD_SNS_CLK_SOURCE))) */
#if (CapSense_ENABLE == CapSense_CSD_EN)
    static void CapSense_SsSetWidgetSenseClkSrc(uint32 wdgtIndex, CapSense_RAM_WD_BASE_STRUCT * ptrWdgt);
#endif /* (CapSense_ENABLE == CapSense_CSD_EN) */

#if (CapSense_ENABLE == CapSense_CSX_EN)
    static void CapSense_SsSetWidgetTxClkSrc(uint32 wdgtIndex, CapSense_RAM_WD_BASE_STRUCT * ptrWdgt);
#endif /* (CapSense_ENABLE == CapSense_CSX_EN) */

/** \}
* \endif */

/*******************************************************************************
* Defines module variables
*******************************************************************************/

#if ((CapSense_ENABLE == CapSense_CSD_CSX_EN) || \
     (CapSense_ENABLE == CapSense_SELF_TEST_EN) || \
     (CapSense_ENABLE == CapSense_ADC_EN))
    CapSense_SENSE_METHOD_ENUM CapSense_currentSenseMethod = CapSense_UNDEFINED_E;
#endif /* ((CapSense_ENABLE == CapSense_CSD_CSX_EN) || \
           (CapSense_ENABLE == CapSense_SELF_TEST_EN) || \
           (CapSense_ENABLE == CapSense_ADC_EN))) */

#if(CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN)
    /*  Module variable keep track of frequency hopping channel index   */
    uint8 CapSense_scanFreqIndex = 0u;
    /*  Variable keep frequency offsets */
    CapSense_PLL_FLL_CFG_TYPE CapSense_immunity[CapSense_NUM_SCAN_FREQS];
#else
    /* const allows C-compiler to do optimization */
    const uint8 CapSense_scanFreqIndex = 0u;
#endif /* (CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN) */

/* Global software variables */
volatile uint8 CapSense_widgetIndex = 0u;    /* Index of the scanning widget */
volatile uint8 CapSense_sensorIndex = 0u;    /* Index of the scanning sensor */
uint8 CapSense_requestScanAllWidget = 0u;
#if (CapSense_CSD_SS_DIS != CapSense_CSD_AUTOTUNE)
    uint8 CapSense_prescalersTuningDone = 0u;
#endif /* (CapSense_CSD_SS_DIS != CapSense_CSD_AUTOTUNE) */

/* Pointer to RAM_SNS_STRUCT structure  */
CapSense_RAM_SNS_STRUCT *CapSense_curRamSnsPtr;

#if ((CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN) || \
     (CapSense_ENABLE == CapSense_CSX_EN))
    /*  Pointer to Flash structure holding configuration of widget to be scanned  */
    CapSense_FLASH_WD_STRUCT const *CapSense_curFlashWdgtPtr = 0u;
#endif /* ((CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN) || \
           (CapSense_ENABLE == CapSense_CSX_EN))  */
#if (CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN)
    /*  Pointer to Flash structure holding info of sensor to be scanned  */
    CapSense_FLASH_SNS_STRUCT const *CapSense_curFlashSnsPtr = 0u;
#endif /* (CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN) */
/*  Pointer to Flash structure to hold Sns electrode that was connected previously  */
CapSense_FLASH_IO_STRUCT const *CapSense_curSnsIOPtr;


/*******************************************************************************
* Function Name: CapSense_IsBusy
****************************************************************************//**
*
* \brief
*  Returns the current status of the component (Scan is completed or Scan is in
*  progress).
*
* \details
*  This function returns a status of the hardware block whether a scan is
*  currently in progress or not. If the component is busy, no new scan or setup
*  widgets is made. The critical section (i.e. disable global interrupt)
*  is recommended for the application when the device transitions from
*  the active mode to sleep or deep sleep modes.
*
* \return
*  Returns the current status of the component:
*    - CapSense_NOT_BUSY - No scan is in progress and a next scan
*      can be initiated.
*    - CapSense_SW_STS_BUSY - The previous scanning is not completed
*      and the hardware block is busy.
*
*******************************************************************************/
uint32 CapSense_IsBusy(void)
{
    return ((*(volatile uint32 *)&CapSense_dsRam.status) & CapSense_SW_STS_BUSY);
}

/*******************************************************************************
* Function Name: CapSense_SetupWidget
****************************************************************************//**
*
* \brief
*  Performs the initialization required to scan the specified widget.
*
* \details
*  This function prepares the component to scan all the sensors in the specified
*  widget by executing the following tasks:
*    1. Re-initialize the hardware if it is not configured to perform the
*       sensing method used by the specified widget, this happens only if the
*       CSD and CSX methods are used in the component.
*    2. Initialize the hardware with specific sensing configuration (e.g.
*       sensor clock, scan resolution) used by the widget.
*    3. Disconnect all previously connected electrodes, if the electrodes
*       connected by the CapSense_CSDSetupWidgetExt(),
*       CapSense_CSXSetupWidgetExt() or CapSense_CSDConnectSns()
*       functions and not disconnected.
*
*  This function does not start sensor scanning, the CapSense_Scan()
*  function must be called to start the scan sensors in the widget. If this
*  function is called more than once, it does not break the component operation,
*  but only the last initialized widget is in effect.

*
* \param widgetId
*  Specifies the ID number of the widget to be initialized for scanning.
*  A macro for the widget ID can be found in the
*  CapSense Configuration header file defined as
*  CapSense_<WidgetName>_WDGT_ID.
*
* \return
*  Returns the status of the widget setting up operation:
*    - CY_RET_SUCCESS - The operation is successfully completed.
*    - CY_RET_BAD_PARAM - The widget is invalid or if the specified widget is
*      disabled
*    - CY_RET_INVALID_STATE - The previous scanning is not completed and the
*      hardware block is busy.
*    - CY_RET_UNKNOWN - An unknown sensing method is used by the widget or any
*      other spurious error occurred.
*
**********************************************************************************/
cy_status CapSense_SetupWidget(uint32 widgetId)
{
    cy_status widgetStatus;

    if (CapSense_WDGT_SW_STS_BUSY == (CapSense_dsRam.status & CapSense_WDGT_SW_STS_BUSY))
    {
        /* Previous widget is being scanned. Return error. */
        widgetStatus = CY_RET_INVALID_STATE;
    }
    /*
     *  Check if widget id is valid, specified widget is enabled and widget did not
     *  detect any fault conditions if BIST is enabled. If all conditions are met,
     *  set widgetStatus as good, if not, set widgetStatus as bad.
     */
    else if ((CapSense_TOTAL_WIDGETS > widgetId) &&
        (0uL != CapSense_GET_WIDGET_EN_STATUS(widgetId)))

    {
        widgetStatus = CY_RET_SUCCESS;
    }
    else
    {
        widgetStatus = CY_RET_BAD_PARAM;
    }

    /*
     * Check widgetStatus flag that is set earlier, if flag is good, then set up only
     * widget
     */
    if (CY_RET_SUCCESS == widgetStatus)
    {
        #if (CapSense_ENABLE == CapSense_CSD_CSX_EN)
            /*  Check widget sensing method is CSX and call CSX APIs    */
            if (CapSense_SENSE_METHOD_CSX_E ==
                CapSense_GET_SENSE_METHOD(&CapSense_dsFlash.wdgtArray[widgetId]))
            {
                /*  Set up widget for CSX scan  */
                CapSense_CSXSetupWidget(widgetId);
            }
            /*  Check widget sensing method is CSD and call appropriate API */
            else if (CapSense_SENSE_METHOD_CSD_E ==
                     CapSense_GET_SENSE_METHOD(&CapSense_dsFlash.wdgtArray[widgetId]))
            {
                /*  Set up widget for CSD scan  */
                CapSense_CSDSetupWidget(widgetId);
            }
            else
            {
                /*  Sensing method is invalid, return error to caller  */
                widgetStatus = CY_RET_UNKNOWN;
            }
        #elif (CapSense_ENABLE == CapSense_CSD_EN)
            /*  Set up widget for scan */
            CapSense_CSDSetupWidget(widgetId);
        #elif (CapSense_ENABLE == CapSense_CSX_EN)
            /*  Set up widgets for scan     */
            CapSense_CSXSetupWidget(widgetId);
        #else
            widgetStatus = CY_RET_UNKNOWN;
            #error "No sensing method enabled, component cannot work in this mode"
        #endif
    }

    return (widgetStatus);
}


/*******************************************************************************
* Function Name: CapSense_Scan
****************************************************************************//**
*
* \brief
*  Initiates scanning of all the sensors in the widget initialized by
*  CapSense_SetupWidget(), if no scan is in progress.
*
* \details
*  This function is called only after the CapSense_SetupWidget()
*  function is called to start the scanning of the sensors in the widget. The
*  status of a sensor scan must be checked using the CapSense_IsBusy()
*  API prior to starting a next scan or setting up another widget.
*
* \return
*  Returns the status of the scan initiation operation:
*    - CY_RET_SUCCESS - Scanning is successfully started.
*    - CY_RET_INVALID_STATE - The previous scanning is not completed and the
*      hardware block is busy.
*    - CY_RET_UNKNOWN - An unknown sensing method is used by the widget.
*
********************************************************************************/
cy_status CapSense_Scan(void)
{
    cy_status scanStatus = CY_RET_SUCCESS;

    if (CapSense_WDGT_SW_STS_BUSY == (CapSense_dsRam.status & CapSense_WDGT_SW_STS_BUSY))
    {
        /* Previous widget is being scanned. Return error. */
        scanStatus = CY_RET_INVALID_STATE;
    }
    else
    {
        /*  If both CSD and CSX are enabled, call scan API based on widget sensing method    */
        #if (CapSense_ENABLE == CapSense_CSD_CSX_EN)
            /*  Check widget sensing method and call appropriate APIs   */
            switch (CapSense_currentSenseMethod)
            {
                case CapSense_SENSE_METHOD_CSX_E:
                    CapSense_CSXScan();
                    break;

                case CapSense_SENSE_METHOD_CSD_E:
                     CapSense_CSDScan();
                    break;

                default:
                    scanStatus = CY_RET_UNKNOWN;
                    break;
            }

        /*  If only CSD is enabled, call CSD scan   */
        #elif (CapSense_ENABLE == CapSense_CSD_EN)
            CapSense_CSDScan();

        /*  If only CSX is enabled, call CSX scan   */
        #elif (CapSense_ENABLE == CapSense_CSX_EN)
            CapSense_CSXScan();

        #else
            scanStatus = CY_RET_UNKNOWN;
            #error "No sensing method enabled, component cannot work in this mode"
        #endif /* (CapSense_ENABLE == CapSense_CSD_CSX_EN) */
    }

    return (scanStatus);
}


/*******************************************************************************
* Function Name: CapSense_ScanAllWidgets
****************************************************************************//**
*
* \brief
*  Initializes the first enabled widget and scanning of all the sensors in the
*  widget, then the same process is repeated for all the widgets in the component,
*  i.e. scanning of all the widgets in the component.
*
* \details
*  This function initializes a widget and scans all the sensors in the widget,
*  and then repeats the same for all the widgets in the component. The tasks of
*  the CapSense_SetupWidget() and CapSense_Scan() functions are
*  executed by these functions. The status of a sensor scan must be checked
*  using the CapSense_IsBusy() API prior to starting a next scan
*  or setting up another widget.
*
* \return
*  Returns the status of the operation:
*    - CY_RET_SUCCESS - Scanning is successfully started.
*    - CY_RET_BAD_PARAM - All the widgets are disabled.
*    - CY_RET_INVALID_STATE - The previous scanning is not completed and the HW block is busy.
*    - CY_RET_UNKNOWN - There are unknown errors.
*
*******************************************************************************/
cy_status CapSense_ScanAllWidgets(void)
{
    cy_status scanStatus = CY_RET_UNKNOWN;

    uint32 wdgtIndex;

    if (CapSense_SW_STS_BUSY == (CapSense_dsRam.status & CapSense_SW_STS_BUSY))
    {
        /* Previous widget is being scanned. Return error. */
        scanStatus = CY_RET_INVALID_STATE;
    }
    else
    {
        /*
         *  Set up widget first widget.
         *  If widget returned error, set up next, continue same until widget does not return error.
         */
        for (wdgtIndex = 0u;
             wdgtIndex < CapSense_TOTAL_WIDGETS;
             wdgtIndex++)
        {

            scanStatus = CapSense_SetupWidget(wdgtIndex);

            if (CY_RET_SUCCESS == scanStatus)
            {
                #if (0u != (CapSense_TOTAL_WIDGETS - 1u))
                    /* If there are more than one widget to be scanned, request callback to scan next widget */
                    if ((CapSense_TOTAL_WIDGETS - 1u) > wdgtIndex)
                    {
                         /* Request callback to scan next widget in ISR */
                        CapSense_requestScanAllWidget = CapSense_ENABLE;
                    }
                    else
                    {
                        /* Request to exit in ISR (Do not scan the next widgets) */
                        CapSense_requestScanAllWidget = CapSense_DISABLE;
                    }
                #else
                    {
                        /* Request to exit in ISR (We have only one widget) */
                        CapSense_requestScanAllWidget = CapSense_DISABLE;
                    }
                #endif  /* (0u != (CapSense_TOTAL_WIDGETS - 1u)) */

                /*  Initiate scan and quit loop */
                scanStatus = CapSense_Scan();

                break;
            }
        }
    }

    return (scanStatus);
}


/*******************************************************************************
* Function Name: CapSense_SsInitialize
****************************************************************************//**
*
* \brief
*   Performs hardware and firmware initialization required for proper operation
*   of the CapSense component. This function is called from
*   the CapSense_Start() API prior to calling any other APIs of the component.
*
* \details
*   Performs hardware and firmware initialization required for proper operation
*   of the CapSense component. This function is called from
*   the CapSense_Start() API prior to calling any other APIs of the component.
*   1. The function initializes immunity offsets when the frequency hopping is
*      enabled.
*   2. Depending on the configuration, the function initializes the CSD block
*      for the CSD2X, CSD, CSX, or CSD+CSX modes.
*   3. The function updates the dsRam.wdgtWorking variable with 1 when Self Test
*      is enabled.
*
*   Calling the CapSense_Start API is the recommended method to initialize
*   the CapSense component at power-up. The CapSense_SsInitialize()
*   API should not be used for initialization, resume, or wake-up operations.
*   The dsRam.wdgtWorking variable is updated.
*
* \return status
*   Returns status of operation:
*   - Zero        - Indicates successful initialization.
*   - Non-zero    - One or more errors occurred in the initialization process.
*
*******************************************************************************/
cy_status CapSense_SsInitialize(void)
{
    cy_status initStatus = CY_RET_SUCCESS;

    #if (CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN)
        (void)CapSense_SsImmunityTblInit(CYDEV_CLK_HFCLK0__HZ);
    #endif /* (CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN) */

    #if((CapSense_ENABLE == CapSense_CSD_EN) ||\
        (CapSense_ENABLE == CapSense_CSX_EN))
        CapSense_SsInitializeSourceSenseClk();
    #endif /* ((CapSense_ENABLE == CapSense_CSD_EN) ||\
               (CapSense_ENABLE == CapSense_CSX_EN)) */
    
    /* Set all IO states to default state  */
    CapSense_SsSetIOsInDefaultState();

    #if ((CapSense_ENABLE == CapSense_CSD_CSX_EN) || \
         (CapSense_ENABLE == CapSense_SELF_TEST_EN) || \
         (CapSense_ENABLE == CapSense_ADC_EN))
        /*
         * CSD hardware block is initialized in the Setup Widget based
         * on widget sensing method. Release previously captured HW resources
         * if it is second call of CapSense_Start() function.
         */
        CapSense_SsSwitchSensingMode(CapSense_UNDEFINED_E);
    #elif (CapSense_ENABLE == CapSense_CSD_EN)
        /*  Initialize CSD block for CSD scanning   */
        CapSense_SsCSDInitialize();

    #elif (CapSense_ENABLE == CapSense_CSX_EN)
        /*  Initialize CSD block for CSX scanning   */
        CapSense_CSXInitialize();

    #else
        #error "No sensing method enabled, component cannot work in this mode"
        initStatus = CY_RET_UNKNOWN;
    #endif /* (CapSense_ENABLE == CapSense_CSD_CSX_EN) */

    return (initStatus);
}


/*******************************************************************************
* Function Name: CapSense_SetPinState
****************************************************************************//**
*
* \brief
*  Sets the state (drive mode and output state) of the port pin used by a sensor.
*  The possible states are GND, Shield, High-Z, Tx or Rx, Sensor. If the sensor
*  specified in the input parameter is a ganged sensor, then the state of all pins
*  associated with the ganged sensor is updated.
*
* \details
*  This function sets a specified state for a specified sensor element. For the 
*  CSD widgets, sensor element is a sensor number, for the CSX widgets, it is either
*  an RX or TX. If the sensor element is a ganged sensor, then the specified state 
*  is also set for all ganged pins of this sensor. Scanning must be completed 
*  before calling this API.
*
*  The CapSense_SHIELD and CapSense_SENSOR states are not
*  allowed if there is no CSD widget configured in the user's project.
*  The CapSense_TX_PIN and CapSense_RX_PIN states are not
*  allowed if there is no CSX widget configured in the user's project.
*
*  Calling this function directly from the application layer is not 
*  recommended. This function is used to implement only the user's specific
*  use cases. Functions that perform a setup and scan of a sensor/widget
*  automatically set the required pin states. They ignore changes
*  in the design made by the CapSense_SetPinState() function.
*  This function neither check wdgtIndex nor sensorElement for the correctness.
*  
*  \param widgetId
*  Specifies the ID number of the widget to change the pin state of the specified
*  sensor.
*  A macro for the widget ID can be found in the CapSense Configuration 
*  header file defined as CapSense_<WidgetName>_WDGT_ID.
*
*  \param sensorElement
*  Specifies the ID number of the sensor element within the widget to change 
*  its pin state. 
*  For the CSD widgets, sensorElement is the sensor ID and can be found in the
*  CapSense Configuration header file defined as
*  CapSense_<WidgetName>_SNS<SensorNumber>_ID.
*  For the CSX widgets, sensorElement is defined either as Rx ID or Tx ID.
*  The first Rx in a widget corresponds to sensorElement = 0, the second
*  Rx in a widget corresponds to sensorElement = 1, and so on.
*  The last Tx in a widget corresponds to sensorElement = (RxNum + TxNum).
*  Macros for Rx and Tx IDs can be found in the 
*  CapSense Configuration header file defined as:
*    - CapSense_<WidgetName>_RX<RXNumber>_ID
*    - CapSense_<WidgetName>_TX<TXNumber>_ID.
*
*  \param state
*   Specifies the state of the sensor to be set:
*     1. CapSense_GROUND - The pin is connected to the ground.
*     2. CapSense_HIGHZ - The drive mode of the pin is set to High-Z
*        Analog.
*     3. CapSense_SHIELD - The shield signal is routed to the pin (only
*        in CSD sensing method when shield electrode is enabled).
*     4. CapSense_SENSOR - The pin is connected to the scanning bus
*        (only in CSD sensing method).
*     5. CapSense_TX_PIN - The TX signal is routed to the sensor
*        (only in CSX sensing method).
*     6. CapSense_RX_PIN - The pin is connected to the scanning bus
*        (only in CSX sensing method).
*
*******************************************************************************/
void CapSense_SetPinState(uint32 widgetId, uint32 sensorElement, uint32 state)
{
    uint32 eltdNum;
    uint32 eltdIndex;
    uint32 interruptState;
    CapSense_FLASH_IO_STRUCT const *ioPtr;
    #if (CapSense_ENABLE == CapSense_GANGED_SNS_EN)
        CapSense_FLASH_SNS_STRUCT const *curFlashSnsPtr;
    #endif

    /* Getting sensor element pointer and number of electrodes */
    #if (CapSense_ENABLE == CapSense_GANGED_SNS_EN)
        /* Check the ganged sns flag */
        if (CapSense_GANGED_SNS_MASK == (CapSense_dsFlash.wdgtArray[widgetId].staticConfig & CapSense_GANGED_SNS_MASK))
        {
            curFlashSnsPtr = CapSense_dsFlash.wdgtArray[widgetId].ptr2SnsFlash;
            curFlashSnsPtr += sensorElement;
            ioPtr = &CapSense_ioList[curFlashSnsPtr->firstPinId];
            eltdNum = curFlashSnsPtr->numPins;
        }
        else
        {
            ioPtr = (CapSense_FLASH_IO_STRUCT const *)CapSense_dsFlash.wdgtArray[widgetId].ptr2SnsFlash + sensorElement;
            eltdNum = 1u;
        }
    #else
        ioPtr = (CapSense_FLASH_IO_STRUCT const *)CapSense_dsFlash.wdgtArray[widgetId].ptr2SnsFlash + sensorElement;
        eltdNum = 1u;
    #endif


    /* Loop through all electrodes of the specified sensor element */
    for (eltdIndex = 0u; eltdIndex < eltdNum; eltdIndex++)
    {
        /* Reset HSIOM and PC registers */
        interruptState = Cy_SysLib_EnterCriticalSection();
        Cy_GPIO_SetHSIOM((GPIO_PRT_Type*)ioPtr->pcPtr, (uint32)ioPtr->pinNumber, (en_hsiom_sel_t)CapSense_HSIOM_SEL_GPIO);
        Cy_SysLib_ExitCriticalSection(interruptState);

        switch (state)
        {
        case CapSense_GROUND:
            interruptState = Cy_SysLib_EnterCriticalSection();
            Cy_GPIO_SetDrivemode((GPIO_PRT_Type*)ioPtr->pcPtr, (uint32)ioPtr->pinNumber, CY_GPIO_DM_STRONG_IN_OFF);
            Cy_GPIO_Clr((GPIO_PRT_Type*)ioPtr->pcPtr, (uint32)ioPtr->pinNumber);
            Cy_SysLib_ExitCriticalSection(interruptState);
            break;

        case CapSense_HIGHZ:
            interruptState = Cy_SysLib_EnterCriticalSection();
            Cy_GPIO_SetDrivemode((GPIO_PRT_Type*)ioPtr->pcPtr, (uint32)ioPtr->pinNumber, CY_GPIO_DM_ANALOG);
            Cy_GPIO_Clr((GPIO_PRT_Type*)ioPtr->pcPtr, (uint32)ioPtr->pinNumber);
            Cy_SysLib_ExitCriticalSection(interruptState);
            break;

        #if (CapSense_ENABLE == CapSense_CSD_EN)
            case CapSense_SENSOR:
                /* Enable sensor */
                CapSense_CSDConnectSns(ioPtr);
                break;
                
            #if (CapSense_ENABLE == CapSense_CSD_SHIELD_EN)
                case CapSense_SHIELD:
                    interruptState = Cy_SysLib_EnterCriticalSection();
                    Cy_GPIO_SetDrivemode((GPIO_PRT_Type*)ioPtr->pcPtr, (uint32)ioPtr->pinNumber, CY_GPIO_DM_STRONG_IN_OFF);
                    Cy_GPIO_SetHSIOM((GPIO_PRT_Type*)ioPtr->pcPtr, (uint32)ioPtr->pinNumber, (en_hsiom_sel_t)CapSense_HSIOM_SEL_CSD_SHIELD);
                    Cy_SysLib_ExitCriticalSection(interruptState);
                    break;
            #endif  /* (CapSense_ENABLE == CapSense_CSD_SHIELD_EN) */
        #endif  /* (CapSense_ENABLE == CapSense_CSD_EN) */

        #if (CapSense_ENABLE == CapSense_CSX_EN)
            case CapSense_TX_PIN:
                interruptState = Cy_SysLib_EnterCriticalSection();
                Cy_GPIO_SetHSIOM((GPIO_PRT_Type*)ioPtr->pcPtr, (uint32)ioPtr->pinNumber, (en_hsiom_sel_t)CapSense_HSIOM_SEL_CSD_SHIELD);
                Cy_GPIO_SetDrivemode((GPIO_PRT_Type*)ioPtr->pcPtr, (uint32)ioPtr->pinNumber, CY_GPIO_DM_STRONG_IN_OFF);
                Cy_SysLib_ExitCriticalSection(interruptState);
                break;

            case CapSense_RX_PIN:
                interruptState = Cy_SysLib_EnterCriticalSection();
                Cy_GPIO_SetHSIOM((GPIO_PRT_Type*)ioPtr->pcPtr, (uint32)ioPtr->pinNumber, (en_hsiom_sel_t)CapSense_HSIOM_SEL_AMUXA);
                Cy_GPIO_SetDrivemode((GPIO_PRT_Type*)ioPtr->pcPtr, (uint32)ioPtr->pinNumber, CY_GPIO_DM_ANALOG);
                Cy_SysLib_ExitCriticalSection(interruptState);
                break;
        #endif  /* (CapSense_ENABLE == CapSense_CSX_EN) */

        default:
            /* Wrong input */
            break;
        }

        ioPtr++;
    }
}


#if ((CapSense_ENABLE == CapSense_CSD_CSX_EN) || \
     (CapSense_ENABLE == CapSense_SELF_TEST_EN) || \
     (CapSense_ENABLE == CapSense_ADC_EN))

    #if (CapSense_ENABLE == CapSense_CSD_EN)
        /*******************************************************************************
        * Function Name: CapSense_SsCSDDisableMode
        ****************************************************************************//**
        *
        * \brief
        *  This function disables CSD mode.
        *
        * \details
        *  To disable CSD mode the following tasks are performed:
        *  1. Disconnect Cmod from AMUXBUS-A.
        *  2. Disconnect previous CSX electrode if it has been connected.
        *  3. Disconnect Csh from AMUXBUS-B.
        *  4. Disable Shield Electrodes.
        *
        *******************************************************************************/
        static void CapSense_SsCSDDisableMode(void)
        {
            uint32 newRegValue;
            Cy_GPIO_SetHSIOM((GPIO_PRT_Type*)CapSense_CSD_CMOD_PORT_PTR, CapSense_CSD_CMOD_PIN,
                                                                             (en_hsiom_sel_t)CapSense_HSIOM_SEL_GPIO);

            #if ((CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN) && \
                 (CapSense_ENABLE == CapSense_CSD_DEDICATED_IDAC_COMP_EN))
                /* Disconnect IDACA and IDACB */
                newRegValue = CY_GET_REG32(CapSense_CSD_SW_REFGEN_SEL_PTR);
                newRegValue &= (uint32)(~CapSense_CSD_SW_REFGEN_SEL_SW_IAIB_MSK);
                CY_SET_REG32(CapSense_CSD_SW_REFGEN_SEL_PTR, newRegValue);
            #endif /* ((CapSense_ENABLE == CapSense_CSD_IDAC_COMP_EN) && \
                       (CapSense_ENABLE == CapSense_CSD_DEDICATED_IDAC_COMP_EN)) */

            /* Disconnect previous CSD electrode if it has been connected */
            CapSense_SsCSDElectrodeCheck();

            /* Disconnect Csh from AMUXBUS-B */
            #if ((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
                 (CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN))

                Cy_GPIO_SetHSIOM((GPIO_PRT_Type*)CapSense_CSD_CTANK_PORT_PTR, CapSense_CSD_CTANK_PIN,
                                                           (en_hsiom_sel_t)CapSense_HSIOM_SEL_GPIO);

            #endif /* ((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
                       (CapSense_ENABLE == CapSense_CSD_SHIELD_TANK_EN)) */

            #if ((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
                 (0u != CapSense_CSD_TOTAL_SHIELD_COUNT))
                CapSense_SsCSDDisableShieldElectrodes();
            #endif /* ((CapSense_ENABLE == CapSense_CSD_SHIELD_EN) && \
                       (0u != CapSense_CSD_TOTAL_SHIELD_COUNT)) */

            if(0uL != newRegValue)
            {
                /* To remove unreferenced local variable warning */
            }

        }
    #endif /* (CapSense_ENABLE == CapSense_CSD_EN) */


    #if (CapSense_ENABLE == CapSense_CSX_EN)
        /*******************************************************************************
        * Function Name: CapSense_SsDisableCSXMode
        ****************************************************************************//**
        *
        * \brief
        *  This function disables CSX mode.
        *
        * \details
        *  To disable CSX mode the following tasks are performed:
        *  1. Disconnect CintA and CintB from AMUXBUS-A.
        *  2. Disconnect previous CSX electrode if it has been connected.
        *
        *******************************************************************************/
        static void CapSense_SsDisableCSXMode(void)
        {
            /* Disconnect previous CSX electrode if it has been connected */
            CapSense_CSXElectrodeCheck();
        }
    #endif /* (CapSense_ENABLE == CapSense_CSX_EN) */


    /*******************************************************************************
    * Function Name: CapSense_SsSwitchSensingMode
    ****************************************************************************//**
    *
    * \brief
    *  This function changes the mode for case when both CSD and CSX widgets are
    *  scanned.
    *
    * \details
    *  To switch to the CSD mode the following tasks are performed:
    *  1. Disconnect CintA and CintB from AMUXBUS-A.
    *  2. Disconnect previous CSD electrode if it has been connected.
    *  3. Initialize CSD mode.
    *
    *  To switch to the CSX mode the following tasks are performed:
    *  1. Disconnect Cmod from AMUXBUS-A.
    *  2. Disconnect previous CSX electrode if it has been connected.
    *  3. Initialize CSX mode.
    *
    * \param mode Specifies the scan mode:
    *           -  (1) CapSense_SENSE_METHOD_CSD_E
    *           -  (2) CapSense_SENSE_METHOD_CSX_E
    *
    *******************************************************************************/
    void CapSense_SsSwitchSensingMode(CapSense_SENSE_METHOD_ENUM mode)
    {
        if (CapSense_currentSenseMethod != mode)
        {
            /* The requested mode differes to the current one. Disable the current mode */
            if (CapSense_SENSE_METHOD_CSD_E ==  CapSense_currentSenseMethod)
            {
                #if (CapSense_ENABLE == CapSense_CSD_EN)
                    CapSense_SsCSDDisableMode();
                #endif /* (CapSense_ENABLE == CapSense_CSD_EN) */
            }
            else if (CapSense_SENSE_METHOD_CSX_E ==  CapSense_currentSenseMethod)
            {
                #if (CapSense_ENABLE == CapSense_CSX_EN)
                    CapSense_SsDisableCSXMode();
                #endif /* (CapSense_ENABLE == CapSense_CSX_EN) */
            }
            else if (CapSense_SENSE_METHOD_BIST_E ==  CapSense_currentSenseMethod)
            {
                #if (CapSense_ENABLE == CapSense_SELF_TEST_EN)
                    CapSense_BistDisableMode();
                #endif /* (CapSense_ENABLE == CapSense_CSX_EN) */
            }
            else
            {
                #if (CapSense_ENABLE == CapSense_ADC_EN)
                    /* Release ADC resources */
                    (void)CapSense_AdcReleaseResources();
                #endif /* (CapSense_ENABLE == CapSense_ADC_EN) */
            }

            /* Enable the specified mode */
            if (CapSense_SENSE_METHOD_CSD_E == mode)
            {
                #if (CapSense_ENABLE == CapSense_CSD_EN)
                    /* Initialize CSD mode to guarantee configured CSD mode */
                    CapSense_SsCSDInitialize();
                    CapSense_currentSenseMethod = CapSense_SENSE_METHOD_CSD_E;
                #endif /* (CapSense_ENABLE == CapSense_CSD_EN) */
            }
            else if (CapSense_SENSE_METHOD_CSX_E == mode)
            {
                #if (CapSense_ENABLE == CapSense_CSX_EN)
                    /* Initialize CSX mode to guarantee configured CSX mode */
                    CapSense_CSXInitialize();
                    CapSense_currentSenseMethod = CapSense_SENSE_METHOD_CSX_E;
                #endif /* (CapSense_ENABLE == CapSense_CSX_EN) */
            }
            else if (CapSense_SENSE_METHOD_BIST_E == mode)
            {
                #if (CapSense_ENABLE == CapSense_SELF_TEST_EN)
                    CapSense_BistInitialize();
                    CapSense_currentSenseMethod = CapSense_SENSE_METHOD_BIST_E;
                #endif /* (CapSense_ENABLE == CapSense_CSX_EN) */
            }
            else
            {
                CapSense_currentSenseMethod = CapSense_UNDEFINED_E;
            }
        }
    }
#endif  /* ((CapSense_ENABLE == CapSense_CSD_CSX_EN) || \
            (CapSense_ENABLE == CapSense_ADC_EN)) */


/*******************************************************************************
* Function Name: CapSense_SsSetIOsInDefaultState
****************************************************************************//**
*
* \brief
*   Sets all electrodes into a default state.
*
* \details
*   Sets all the CSD/CSX IOs into a default state:
*   - HSIOM   - Disconnected, the GPIO mode.
*   - DM      - Strong drive.
*   - State   - Zero.
*
*   Sets all the ADC channels into a default state:
*   - HSIOM   - Disconnected, the GPIO mode.
*   - DM      - HiZ-Analog.
*   - State   - Zero.
*
*   It is not recommended to call this function directly from the application
*   layer.
*
*******************************************************************************/
void CapSense_SsSetIOsInDefaultState(void)
{
    CapSense_FLASH_IO_STRUCT const *ioPtr = &CapSense_ioList[0u];
    uint32 loopIndex;

    /*  Loop through all electrodes */
    for (loopIndex = 0u; loopIndex < CapSense_TOTAL_ELECTRODES; loopIndex++)
    {
        /*  1. Disconnect HSIOM
            2. Set strong DM
            3. Set pin state to logic 0
        */
        Cy_GPIO_Pin_FastInit((GPIO_PRT_Type*)ioPtr->pcPtr, (uint32)ioPtr->pinNumber, CY_GPIO_DM_STRONG, 0u,
                                                        (en_hsiom_sel_t)CapSense_HSIOM_SEL_GPIO);

        /*  Get next electrode  */
        ioPtr++;
    }
    
    #if(CapSense_ENABLE == CapSense_ADC_EN)
        CapSense_ClearAdcChannels();
    #endif /* (CapSense_ENABLE == CapSense_ADC_EN) */
}


#if (CapSense_ENABLE == CapSense_ADC_EN)
/*******************************************************************************
* Function Name: CapSense_SsReleaseResources()
****************************************************************************//**
*
* \brief
*  This function sets the resources that do not belong to the CSDv2 HW block to
*  default state.
*
* \details
*  The function performs following tasks:
*  1. Checks if CSD block busy and returns error if it is busy
*  2. Disconnects integration capacitors (CintA, CintB for CSX mode and
*     Cmod for CSD mode)
*  3. Disconnect electroded if they have been connected.
*
* \return
*   Returns the status of the operation:
*   - Zero        - Resources released successfully.
*   - Non-zero    - One or more errors occurred in releasing process.
*
*******************************************************************************/
cy_status CapSense_SsReleaseResources(void)
{
    cy_status busyStatus = CY_RET_SUCCESS;

    if (CapSense_SW_STS_BUSY == (CapSense_dsRam.status & CapSense_SW_STS_BUSY))
    {
        /* Previous widget is being scanned. Return error. */
        busyStatus = CY_RET_INVALID_STATE;
    }
    else
    {
        #if (CapSense_ENABLE == CapSense_CSX_EN)
            CapSense_SsDisableCSXMode();
        #endif /* (CapSense_ENABLE == CapSense_CSX_EN) */

        #if (CapSense_ENABLE == CapSense_CSD_EN)
            CapSense_SsCSDDisableMode();
        #endif /* (CapSense_ENABLE == CapSense_CSD_EN) */

        #if (CapSense_ENABLE == CapSense_SELF_TEST_EN)
            CapSense_BistDisableMode();
        #endif /* (CapSense_ENABLE == CapSense_SELF_TEST_EN) */
        #if ((CapSense_ENABLE == CapSense_CSD_EN) && \
             (CapSense_ENABLE == CapSense_CSD_SHIELD_EN) &&  \
             (CapSense_SNS_CONNECTION_SHIELD == CapSense_CSD_INACTIVE_SNS_CONNECTION))
            CapSense_SsSetIOsInDefaultState();
        #endif /* ((CapSense_ENABLE == CapSense_CSD_EN) && \
             (CapSense_DISABLE != CapSense_CSD_SHIELD_EN) &&  \
             (CapSense_SNS_CONNECTION_SHIELD == CapSense_CSD_INACTIVE_SNS_CONNECTION)) */

        /*
         * Reset of the currentSenseMethod variable to make sure that the next
         * call of SetupWidget() API setups the correct widget mode
         */
        CapSense_currentSenseMethod = CapSense_UNDEFINED_E;
    }

    return busyStatus;
}
#endif /* (CapSense_ENABLE == CapSense_ADC_EN) */


/*******************************************************************************
* Function Name: CapSense_SsPostAllWidgetsScan
****************************************************************************//**
*
* \brief
*   The ISR function for multiple widget scanning implementation.
*
* \details
*   This is the function used by the CapSense ISR to implement multiple widget
*   scanning.
*   Should not be used by the application layer.
*
*******************************************************************************/
void CapSense_SsPostAllWidgetsScan(void)
{
    /*
    *   1. Increment widget index
    *   2. Check if all the widgets are scanned
    *   3. If all the widgets are not scanned, set up and scan next widget
    */
    #if (1u != CapSense_TOTAL_WIDGETS)
        cy_status postScanStatus;

        do
        {
            CapSense_widgetIndex++;

            postScanStatus = CapSense_SetupWidget((uint32)CapSense_widgetIndex);

            if (CY_RET_SUCCESS == postScanStatus)
            {
                if((CapSense_TOTAL_WIDGETS - 1u) == CapSense_widgetIndex)
                {
                    /* The last widget will be scanned. Reset flag to skip configuring the next widget setup in ISR. */
                    CapSense_requestScanAllWidget = CapSense_DISABLE;
                }
                (void)CapSense_Scan();
            }
            else if((CapSense_TOTAL_WIDGETS - 1u) == CapSense_widgetIndex)
            {
                #if ((CapSense_ENABLE == CapSense_BLOCK_OFF_AFTER_SCAN_EN) && \
                     (CapSense_ENABLE == CapSense_CSD_EN))
                    if (CapSense_SENSE_METHOD_CSD_E ==
                             CapSense_GET_SENSE_METHOD(&CapSense_dsFlash.wdgtArray[CapSense_widgetIndex]))
                    {
                        /*  Disable the CSD block */
                        CY_SET_REG32(CapSense_CSD_CONFIG_PTR, CapSense_configCsd);
                    }
                #endif /* ((CapSense_ENABLE == CapSense_BLOCK_OFF_AFTER_SCAN_EN) && \
                           (CapSense_ENABLE == CapSense_CSD_EN)) */

                /* All widgets are totally processed. Reset BUSY flag */
                CapSense_dsRam.status &= ~CapSense_SW_STS_BUSY;

                /* Update status with with the failure */
                CapSense_dsRam.status &= ~CapSense_STATUS_ERR_MASK;
                CapSense_dsRam.status |= ((postScanStatus & CapSense_STATUS_ERR_SIZE) << CapSense_STATUS_ERR_SHIFT);

                /* Set postScanStatus to exit the while loop */
                postScanStatus = CY_RET_SUCCESS;
            }
            else
            {
                /* Update status with with the failure. Configure the next widget in while() loop */
                CapSense_dsRam.status &= ~CapSense_STATUS_ERR_MASK;
                CapSense_dsRam.status |= ((postScanStatus & CapSense_STATUS_ERR_SIZE) << CapSense_STATUS_ERR_SHIFT);
            }
        } while (CY_RET_SUCCESS != postScanStatus);
    #endif /* (1u != CapSense_TOTAL_WIDGETS) */
}


/*******************************************************************************
* Function Name: CapSense_SsIsrInitialize
****************************************************************************//**
*
* \brief
*   Enables and initializes for the function pointer for a callback for the ISR.
*
* \details
*   The  "address" is a special type cy_israddress defined by syslib. This function
*   is used by component APIs and should not be used by an application program for
*   proper working of the component.
*
* \param  address The address of the function to be called when interrupt is fired.
*
*******************************************************************************/
void CapSense_SsIsrInitialize(cy_israddress address)
{
    /* Disable interrupt */
    #if defined(CapSense_ISR__INTC_ASSIGNED)
        NVIC_DisableIRQ(CapSense_ISR_cfg.intrSrc);
    #endif

    /* Configure interrupt with priority and vector */
    #if defined(CapSense_ISR__INTC_ASSIGNED)
        (void)Cy_SysInt_Init(&CapSense_ISR_cfg, address);
    #endif
    
    /* Enable interrupt */
    #if defined(CapSense_ISR__INTC_ASSIGNED)
        NVIC_EnableIRQ(CapSense_ISR_cfg.intrSrc);
    #endif
}


/*******************************************************************************
* Function Name: CapSense_SsSetSnsClockDivider
****************************************************************************//**
*
* \brief
*   Sets the divider values for the sense clock and then starts
*   the sense clock.
*
* \details
*   It is not recommended to call this function directly by the application layer.
*   It is used by initialization, widget APIs or wakeup functions to
*   enable the clocks.
*
* \param
*   snsClk The divider value for the sense clock.
*
*******************************************************************************/
void CapSense_SsSetSnsClockDivider(uint32 snsClk)
{
    uint32 newRegValue;

    /*
     * Set divider value for sense clock.
     * 1u is subtracted from snsClk because SENSE_DIV value 0 corresponds
     * to dividing by 1.
     */
    newRegValue = CY_GET_REG32(CapSense_CSD_SENSE_PERIOD_PTR);
    newRegValue &= (uint32)(~CapSense_CSD_SENSE_PERIOD_SENSE_DIV_MSK);
    newRegValue |= snsClk - 1u;
    CY_SET_REG32(CapSense_CSD_SENSE_PERIOD_PTR, newRegValue);
}


/*******************************************************************************
* Function Name: CapSense_SsSetSnsFirstPhaseWidth
****************************************************************************//**
*
* \brief
*   Defines the length of the first phase of the sense clock in clk_csd cycles.
*
* \details
*   It is not recommended to call this function directly by the application layer.
*   It is used by initialization, widget APIs or wakeup functions to
*   enable the clocks.
*   At all times it must be assured that the phases are at least 2 clk_csd cycles
*   (1 for non overlap, if used), if this rule is violated the result is undefined.
*
* \param
*   snsClk The divider value for the sense clock.
*
*******************************************************************************/
void CapSense_SsSetSnsFirstPhaseWidth(uint32 phaseWidth)
{
    uint32 newRegValue;
    
    newRegValue = CY_GET_REG32(CapSense_CSD_SENSE_DUTY_PTR);
    newRegValue &= (uint32)(~CapSense_CSD_SENSE_DUTY_SENSE_WIDTH_MSK);
    newRegValue |= phaseWidth;
    CY_SET_REG32(CapSense_CSD_SENSE_DUTY_PTR, newRegValue);
}


/*******************************************************************************
* Function Name: CapSense_SsSetClockDividers
****************************************************************************//**
*
* \brief
*   Sets the divider values for sense and modulator clocks and then starts
*   a modulator clock-phase aligned to HFCLK and sense clock-phase aligned to
*   the modulator clock.
*
* \details
*   It is not recommended to call this function directly by the application layer.
*   It is used by initialization, widget APIs or wakeup functions to
*   enable the clocks.
*
* \param
*   snsClk The divider value for the sense clock.
*   modClk The divider value for the modulator clock.
*
*******************************************************************************/
void CapSense_SsSetClockDividers(uint32 snsClk, uint32 modClk)
{
    /* Configure Mod clock */
    CapSense_ModClk_SetDivider((uint32)modClk - 1uL);

    /* Configure Sns clock */
    CapSense_SsSetSnsClockDivider(snsClk);
}


#if ((CapSense_ENABLE == CapSense_CSD_IDAC_AUTOCAL_EN) || \
     (CapSense_ENABLE == CapSense_CSX_IDAC_AUTOCAL_EN))
    /*******************************************************************************
    * Function Name: CapSense_CalibrateWidget
    ****************************************************************************//**
    *
    * \brief
    *  Calibrates the IDACs for all the sensors in the specified widget to the default
    *  target, this function detects the sensing method used by the
    *  widget prior to calibration.
    *
    * \details
    *  This function performs exactly the same tasks as
    *  CapSense_CalibrateAllWidgets, but only for a specified widget.
    *  This function detects the sensing method used by the widgets and uses
    *  the Enable compensation IDAC parameter.
    *
    *  This function is available when the CSD and/or CSX Enable IDAC
    *  auto-calibration parameter is enabled.

    *
    * \param widgetId
    *  Specifies the ID number of the widget to calibrate its raw count.
    *  A macro for the widget ID can be found in the
    *  CapSense Configuration header file defined as
    *  CapSense_<WidgetName>_WDGT_ID.
    *
    * \return
    *  Returns the status of the specified widget calibration:
    *    - CY_RET_SUCCESS - The operation is successfully completed.
    *    - CY_RET_BAD_PARAM - The input parameter is invalid.
    *    - CY_RET_BAD_DATA - The calibration failed and the component may not
    *      operate as expected.
    *
    *******************************************************************************/
    cy_status CapSense_CalibrateWidget(uint32 widgetId)
    {
        cy_status calibrateStatus = CY_RET_SUCCESS;

        do
        {
            if (CapSense_TOTAL_WIDGETS <= widgetId)
            {
                calibrateStatus = CY_RET_BAD_PARAM;
            }

            /*
             *  Check if widget id is valid, specified widget did not
             *  detect any faults conditions if BIST is enabled.
             */
            #if (CapSense_ENABLE == CapSense_SELF_TEST_EN)
                if (0u != CapSense_GET_WIDGET_EN_STATUS(widgetId))
                {
                    calibrateStatus = CY_RET_SUCCESS;
                }
                else
                {
                    calibrateStatus = CY_RET_INVALID_STATE;
                }
            #endif  /* (CapSense_ENABLE == CapSense_SELF_TEST_EN) */

            if (CY_RET_SUCCESS != calibrateStatus)
            {
                /* Exit from the loop because of a fail */
                break;
            }
            /*  If both CSD and CSX are enabled, calibrate widget using sensing method */
            #if (CapSense_ENABLE == CapSense_CSD_CSX_EN)

                /* Check widget sensing method and call appropriate APIs */
                #if (CapSense_ENABLE == CapSense_CSX_IDAC_AUTOCAL_EN)
                    if (CapSense_SENSE_METHOD_CSX_E ==
                        CapSense_GET_SENSE_METHOD(&CapSense_dsFlash.wdgtArray[widgetId]))
                    {
                        /* Calibrate CSX widget  */
                       CapSense_CSXCalibrateWidget(widgetId, CapSense_CSX_RAWCOUNT_CAL_LEVEL);
                    }
                #endif  /* (CapSense_ENABLE == CapSense_CSX_IDAC_AUTOCAL_EN) */

                #if (CapSense_ENABLE == CapSense_CSD_IDAC_AUTOCAL_EN)
                    if (CapSense_SENSE_METHOD_CSD_E ==
                        CapSense_GET_SENSE_METHOD(&CapSense_dsFlash.wdgtArray[widgetId]))
                    {
                        /* Calibrate CSD widget */
                        calibrateStatus = CapSense_CSDCalibrateWidget(widgetId, CapSense_CSD_RAWCOUNT_CAL_LEVEL);
                    }
                #endif  /* (CapSense_ENABLE == CapSense_CSD_IDAC_AUTOCAL_EN) */

            /*  If only CSD is enabled, calibrate CSD sensor  */
            #elif (CapSense_ENABLE == CapSense_CSD_EN)
                calibrateStatus = CapSense_CSDCalibrateWidget(widgetId, CapSense_CSD_RAWCOUNT_CAL_LEVEL);

            /*  If only CSX is enabled, call CSX scan   */
            #elif (CapSense_ENABLE == CapSense_CSX_EN)
                CapSense_CSXCalibrateWidget(widgetId, CapSense_CSX_RAWCOUNT_CAL_LEVEL);

            #else
                calibrateStatus = CY_RET_UNKNOWN;
            #endif /* (CapSense_ENABLE == CapSense_CSD_CSX_EN) */

             /* Update CRC */
            #if (CapSense_ENABLE ==CapSense_TST_WDGT_CRC_EN)
                CapSense_DsUpdateWidgetCrc(widgetId);
            #endif /* (CapSense_ENABLE ==CapSense_TST_WDGT_CRC_EN) */

        } while (0u);

        return calibrateStatus;
    }


    /*******************************************************************************
    * Function Name: CapSense_CalibrateAllWidgets
    ****************************************************************************//**
    *
    * \brief
    *  Calibrates the IDACs for all the widgets in the component to default target
    *  value, this function detects the sensing method used by the widgets
    *  prior to calibration.
    *
    * \details
    *  Calibrates the IDACs for all the widgets in the component to the default
    *  target value. This function detects
    *  the sensing method used by the widgets and regards the Enable
    *  compensation IDAC parameter.
    *
    *  This function is available when the CSD and/or CSX Enable IDAC
    *  auto-calibration parameter is enabled.
    *
    * \return
    *  Returns the status of the calibration process:
    *    - CY_RET_SUCCESS - The operation is successfully completed.
    *    - CY_RET_BAD_DATA - The calibration failed and the component may not
    *      operate as expected.
    *
    *******************************************************************************/
    cy_status CapSense_CalibrateAllWidgets(void)
    {
        cy_status calibrateStatus = CY_RET_SUCCESS;
        uint32 wdgtIndex;

        for (wdgtIndex = 0u; wdgtIndex < CapSense_TOTAL_WIDGETS; wdgtIndex++)
        {
            calibrateStatus |= CapSense_CalibrateWidget(wdgtIndex);
        }

        return calibrateStatus;
    }
#endif /* ((CapSense_ENABLE == CapSense_CSD_IDAC_AUTOCAL_EN) ||
           (CapSense_ENABLE == CapSense_CSX_IDAC_AUTOCAL_EN)) */


#if (CapSense_CSD_SS_DIS != CapSense_CSD_AUTOTUNE)
    /*******************************************************************************
    * Function Name: CapSense_SsSetDirectClockMode
    ****************************************************************************//**
    *
    * \brief
    *  Sets Direct Clock Mode.
    *
    * \details
    *  For CSDv1: Resets PRS bit in CapSense_configCsd variable;
    *  For CSDv2: Resets CapSense_CSD_SENSE_PERIOD_SEL_LFSR_MSB_MSK and
    *  CapSense_CSD_SENSE_PERIOD_SEL_LFSR_MSB_MSK bits in
    *  CapSense_SENSE_PERIOD register.
    *
    *******************************************************************************/
    static void CapSense_SsSetDirectClockMode(void)
    {
        CY_SET_REG32(CapSense_CSD_SENSE_PERIOD_PTR, CY_GET_REG32(CapSense_CSD_SENSE_PERIOD_PTR) &
                                                             (uint32)~(CapSense_CSD_SENSE_PERIOD_SEL_LFSR_MSB_MSK | \
                                                             CapSense_CSD_SENSE_PERIOD_LFSR_SIZE_MSK));
    }


    /*******************************************************************************
    * Function Name: CapSense_SsAutoTune
    ****************************************************************************//**
    *
    * \brief
    *  This API performs the parameters auto-tuning for the optimal CapSense operation.
    *
    * \details
    *  This API performs the following:
    *  - Calibrates Modulator and Compensation IDACs.
    *  - Tunes the Sense Clock optimal value to get a Sense Clock period greater than
    *     2*5*R*Cp.
    *  - Calculates the resolution for the optimal finger capacitance.
    *
    * \return
    *   Returns the status of the operation:
    *   - Zero     - All the widgets are auto-tuned successfully.
    *   - Non-zero - Auto-tuning failed for any widget.
    *
    *******************************************************************************/
    cy_status CapSense_SsAutoTune(void)
    {
        cy_status autoTuneStatus = CY_RET_SUCCESS;
        uint32 wdgtIndex;
        uint32 cp = 0uL;
        #if (CapSense_CSD_MATRIX_WIDGET_EN | CapSense_CSD_TOUCHPAD_WIDGET_EN)
            uint32 cpRow = 0uL;
        #endif /* (CapSense_CSD_MATRIX_WIDGET_EN | CapSense_CSD_TOUCHPAD_WIDGET_EN) */
        uint32 cpWidget[CapSense_TOTAL_WIDGETS];
        CapSense_RAM_WD_BASE_STRUCT *ptrWdgt;
        AUTO_TUNE_CONFIG_TYPE autoTuneConfig;

        /* Configure common config variables */
        autoTuneConfig.snsClkConstantR = CapSense_CSD_SNSCLK_R_CONST;
        autoTuneConfig.vRef = CapSense_CSD_VREF_MV;
        autoTuneConfig.iDacGain = CapSense_CSD_IDAC_GAIN_VALUE_NA * CapSense_CSD_DUAL_IDAC_FACTOR;
        autoTuneConfig.calTarget = CapSense_CSD_AUTOTUNE_CAL_LEVEL;

        /* Calculate snsClk Input Clock in KHz */
        /*  Dividers are chained */
        autoTuneConfig.snsClkInputClock = (CYDEV_CLK_PERICLK__KHZ / CapSense_dsRam.modCsdClk);

        /* If both CSD and CSX are enabled, calibrate widget using sensing method */
        #if (CapSense_ENABLE == CapSense_CSD_CSX_EN)
            /* Initialize CSD mode */
            CapSense_SsCSDInitialize();
        #endif /* (CapSense_ENABLE == CapSense_CSD_CSX_EN) */

        /* Set flag to calibrate in PWM mode only */
        CapSense_prescalersTuningDone = CapSense_DISABLE;

        /* Switch charge clock source to direct clock mode */
        CapSense_SsSetDirectClockMode();


        /* Tune sense clock for all widgets */
        for (wdgtIndex = 0u; wdgtIndex < CapSense_TOTAL_WIDGETS; wdgtIndex++)
        {
            if (CapSense_SENSE_METHOD_CSD_E ==
                CapSense_GET_SENSE_METHOD(&CapSense_dsFlash.wdgtArray[wdgtIndex]))
            {
                ptrWdgt = (CapSense_RAM_WD_BASE_STRUCT *)
                          CapSense_dsFlash.wdgtArray[wdgtIndex].ptr2WdgtRam;

                /* Set calibration resolution to 12 bits */
                ptrWdgt->resolution = CapSense_CALIBRATION_RESOLUTION;

                /* Set Sense clock frequency to 1.5 MHz */
                #if (CapSense_ENABLE == CapSense_CSD_COMMON_SNS_CLK_EN)
                    CapSense_dsRam.snsCsdClk = (uint8)((uint32)autoTuneConfig.snsClkInputClock /
                                                       CapSense_CALIBRATION_FREQ_KHZ);
                #elif (CapSense_CSD_MATRIX_WIDGET_EN | CapSense_CSD_TOUCHPAD_WIDGET_EN)
                    if ((CapSense_WD_TOUCHPAD_E == (CapSense_WD_TYPE_ENUM)CapSense_dsFlash.wdgtArray[(wdgtIndex)].wdgtType) ||
                        (CapSense_WD_MATRIX_BUTTON_E == (CapSense_WD_TYPE_ENUM)CapSense_dsFlash.wdgtArray[(wdgtIndex)].wdgtType))
                    {
                        ptrWdgt->rowSnsClk = (uint16)((uint32)autoTuneConfig.snsClkInputClock /
                                             CapSense_CALIBRATION_FREQ_KHZ);
                    }
                    ptrWdgt->snsClk = (uint16)((uint32)autoTuneConfig.snsClkInputClock /
                                      CapSense_CALIBRATION_FREQ_KHZ);
                #else
                    ptrWdgt->snsClk = (uint16)((uint32)autoTuneConfig.snsClkInputClock /
                                      CapSense_CALIBRATION_FREQ_KHZ);
                #endif /*  (CapSense_ENABLE == CapSense_CSD_COMMON_SNS_CLK_EN) */

                /* Calibrate CSD widget to 85% */
                (void)CapSense_CSDCalibrateWidget(wdgtIndex, CapSense_CSD_AUTOTUNE_CAL_LEVEL);

                #if (CapSense_CSD_MATRIX_WIDGET_EN | CapSense_CSD_TOUCHPAD_WIDGET_EN)
                    if ((CapSense_WD_TOUCHPAD_E == (CapSense_WD_TYPE_ENUM)CapSense_dsFlash.wdgtArray[(wdgtIndex)].wdgtType) ||
                        (CapSense_WD_MATRIX_BUTTON_E == (CapSense_WD_TYPE_ENUM)CapSense_dsFlash.wdgtArray[(wdgtIndex)].wdgtType))
                    {
                        /* Get pointer to Modulator IDAC for columns */
                        autoTuneConfig.ptrModIDAC = &ptrWdgt->rowIdacMod[0u];

                        /* Get pointer to Sense Clock Divider for columns */
                        autoTuneConfig.ptrSenseClk = &ptrWdgt->rowSnsClk;

                        /* Find correct sense clock value */
                        cpRow = SmartSense_TunePrescalers(&autoTuneConfig);

                        if ((CapSense_CP_MAX + CapSense_CP_ERROR) <= cpRow)
                        {
                            autoTuneStatus = CY_RET_BAD_DATA;
                        }

                        /* Make sure that ModClk >= 4 * rowSnsClk for ModClk <= 12 MHz and rowSnsClk <= 6MHz */
                        if (autoTuneConfig.snsClkInputClock <= CapSense_MOD_CSD_CLK_24000KHZ)
                        {
                            if (ptrWdgt->rowSnsClk < CapSense_FACTOR_FILTER_DELAY_12MHZ)
                            {
                                ptrWdgt->rowSnsClk = CapSense_FACTOR_FILTER_DELAY_12MHZ;
                            }
                        }
                        else if (autoTuneConfig.snsClkInputClock <= CapSense_MOD_CSD_CLK_48000KHZ)
                        {
                            if (ptrWdgt->rowSnsClk < CapSense_FACTOR_MOD_SNS)
                            {
                                ptrWdgt->rowSnsClk = CapSense_FACTOR_MOD_SNS;
                            }
                        }
                        else
                        {
                            /* rowSnsClk is valid*/
                        }
                    }
                #endif /* (CapSense_CSD_MATRIX_WIDGET_EN | CapSense_CSD_TOUCHPAD_WIDGET_EN) */

                /* Get pointer to Modulator IDAC  for rows */
                autoTuneConfig.ptrModIDAC = &ptrWdgt->idacMod[0u];

                /* Get pointer to Sense Clock Divider for columns */
                autoTuneConfig.ptrSenseClk = &ptrWdgt->snsClk;

                /* Find correct sense clock value */
                cp = SmartSense_TunePrescalers(&autoTuneConfig);

                if ((CapSense_CP_MAX + CapSense_CP_ERROR) <= cp)
                {
                    autoTuneStatus = CY_RET_BAD_DATA;
                }

                /* Make sure that ModClk >= 4 * SnsClk for ModClk <= 12 MHz and SnsClk <= 6MHz */
                if (autoTuneConfig.snsClkInputClock <= CapSense_MOD_CSD_CLK_24000KHZ)
                {
                    if (ptrWdgt->snsClk < CapSense_FACTOR_FILTER_DELAY_12MHZ)
                    {
                        ptrWdgt->snsClk = CapSense_FACTOR_FILTER_DELAY_12MHZ;
                    }
                }
                else if (autoTuneConfig.snsClkInputClock <= CapSense_MOD_CSD_CLK_48000KHZ)
                {
                    if (ptrWdgt->snsClk < CapSense_FACTOR_MOD_SNS)
                    {
                        ptrWdgt->snsClk = CapSense_FACTOR_MOD_SNS;
                    }
                }
                else
                {
                    /* SnsClk is valid*/
                }

                cpWidget[wdgtIndex] = cp;

                #if (CapSense_CSD_MATRIX_WIDGET_EN | CapSense_CSD_TOUCHPAD_WIDGET_EN)
                    if ((CapSense_WD_TOUCHPAD_E == (CapSense_WD_TYPE_ENUM)CapSense_dsFlash.wdgtArray[(wdgtIndex)].wdgtType) ||
                        (CapSense_WD_MATRIX_BUTTON_E == (CapSense_WD_TYPE_ENUM)CapSense_dsFlash.wdgtArray[(wdgtIndex)].wdgtType))
                    {
                        if (cpRow > cp)
                        {
                            cpWidget[wdgtIndex] = cpRow;
                        }
                    }
                #endif /* (CapSense_CSD_MATRIX_WIDGET_EN | CapSense_CSD_TOUCHPAD_WIDGET_EN) */
            }
        }

        /* Set flag to indicate that calibration in PWM mode has been performed */
        CapSense_prescalersTuningDone = 1u;

        /* Multiply Clk divider to 2 for PRS mode to take into account average PRS frequency */
        autoTuneConfig.prsFactor = CapSense_PRS_FACTOR_DIV;

        /* Tune sensitivity and resolution for all widgets */
        for (wdgtIndex = 0u; wdgtIndex < CapSense_TOTAL_WIDGETS; wdgtIndex++)
        {
            if (CapSense_SENSE_METHOD_CSD_E ==
                CapSense_GET_SENSE_METHOD(&CapSense_dsFlash.wdgtArray[wdgtIndex]))
            {
                ptrWdgt = (CapSense_RAM_WD_BASE_STRUCT *)
                          CapSense_dsFlash.wdgtArray[wdgtIndex].ptr2WdgtRam;

                /* Get pointer to Modulator IDAC for rows */
                autoTuneConfig.ptrModIDAC = &ptrWdgt->idacMod[0u];

                /* Get pointer to maximum Sense Clock Divider (column or row) */
                autoTuneConfig.ptrSenseClk = &ptrWdgt->snsClk;

                /* Set parasitic capacitance for columns */
                autoTuneConfig.sensorCap = cpWidget[wdgtIndex];

                #if (CapSense_CSD_MATRIX_WIDGET_EN | CapSense_CSD_TOUCHPAD_WIDGET_EN)
                    if ((CapSense_WD_TOUCHPAD_E == (CapSense_WD_TYPE_ENUM)CapSense_dsFlash.wdgtArray[(wdgtIndex)].wdgtType) ||
                        (CapSense_WD_MATRIX_BUTTON_E == (CapSense_WD_TYPE_ENUM)CapSense_dsFlash.wdgtArray[(wdgtIndex)].wdgtType))

                    {
                        /* Set the minimum sense clock frequency to calculate the resolution */
                        if (ptrWdgt->snsClk < ptrWdgt->rowSnsClk)
                        {
                            /* Rewrite pointer to Sense Clock Divider for rows */
                            autoTuneConfig.ptrSenseClk = &ptrWdgt->rowSnsClk;

                            /* Set parasitic capacitance for rows */
                            autoTuneConfig.sensorCap = cpWidget[wdgtIndex];
                        }
                    }
                #endif /* (CapSense_CSD_MATRIX_WIDGET_EN | CapSense_CSD_TOUCHPAD_WIDGET_EN) */

                /* Get finger capacitance */
                autoTuneConfig.fingerCap = ptrWdgt->fingerCap;

                /* Init pointer to sigPFC */
                autoTuneConfig.sigPFC = &ptrWdgt->sigPFC;

                #if (CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSD_SNS_CLK_SOURCE)
                    CapSense_SsSetWidgetSenseClkSrc(wdgtIndex, ptrWdgt);
                #endif /* (CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSD_SNS_CLK_SOURCE) */

                /* Calibrate CSD widget to 85% */
                autoTuneStatus |= CapSense_CSDCalibrateWidget(wdgtIndex, CapSense_CSD_AUTOTUNE_CAL_LEVEL);

                /* Find resolution */
                ptrWdgt->resolution = SmartSense_TuneSensitivity(&autoTuneConfig);

                #if (CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSD_SNS_CLK_SOURCE)
                    CapSense_SsSetWidgetSenseClkSrc(wdgtIndex, ptrWdgt);

                    #if (CapSense_CSD_MATRIX_WIDGET_EN | CapSense_CSD_TOUCHPAD_WIDGET_EN)
                        if (((uint8)CapSense_CLK_SOURCE_DIRECT == ptrWdgt->snsClkSource) ||
                            ((uint8)CapSense_CLK_SOURCE_DIRECT == ptrWdgt->rowSnsClkSource))
                        {
                            /* Recalibrate CSD widget to 85% because source is changed to direct */
                            autoTuneStatus |= CapSense_CSDCalibrateWidget(wdgtIndex, CapSense_CSD_AUTOTUNE_CAL_LEVEL);
                        }
                    #else
                        if ((uint8)CapSense_CLK_SOURCE_DIRECT == ptrWdgt->snsClkSource)
                        {
                            autoTuneStatus |= CapSense_CSDCalibrateWidget(wdgtIndex, CapSense_CSD_AUTOTUNE_CAL_LEVEL);
                        }
                    #endif /* (CapSense_CSD_MATRIX_WIDGET_EN | CapSense_CSD_TOUCHPAD_WIDGET_EN) */
                #endif /* (CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSD_SNS_CLK_SOURCE) */

                #if (CapSense_ENABLE == CapSense_TST_WDGT_CRC_EN)
                    CapSense_DsUpdateWidgetCrc(wdgtIndex);
                #endif /* (CapSense_ENABLE == CapSense_TST_WDGT_CRC_EN) */
            }
        }

        return autoTuneStatus;
    }
#endif /* (CapSense_CSD_SS_DIS != CapSense_CSD_AUTOTUNE)) */

#if (CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN)

    /*******************************************************************************
    * Function Name: CapSense_SsImmunityTblInit
    ****************************************************************************//**
    *
    * \brief
    *  Initializes the CapSense_immunity[] table with appropriate FLL/PLL
    *  dividers for each frequency channel.
    *
    * \details
    *  This is an internal function that initializes the CapSense_immunity[]
    *  table with appropriate FLL/PLL dividers for each frequency channel.
    *
    * \return
    *  Returns the status of the initialization process. If CY_RET_SUCCESS is not
    *  received, some of the initialization fails and the component may not operate
    *  as expected.
    *
    *******************************************************************************/
    cy_status CapSense_SsImmunityTblInit(uint32 clkSrcFreqHz)
    {
        uint32 pathMuxFreqHz;
        uint32 reference;
        uint32 feedback;
        uint32 output;

        uint32 freqShift;
        cy_en_fll_pll_output_mode_t outputMode;
        cy_en_hf_clk_sources_t hfClk0Source;
        cy_stc_fll_manual_config_t fllManualCfg;

        cy_status ch1Result = CY_RET_UNKNOWN;
        cy_status ch2Result = CY_RET_UNKNOWN;
        cy_status retVal;

        /* The value of freqShift is 5% of FLL/PLL output frequency. */
        freqShift = clkSrcFreqHz / 20uL;
        hfClk0Source = Cy_SysClk_HfClockGetSource(CapSense_P6_PERI_SRC_HFCLK);

        if(CY_SYSCLK_HFCLK_IN_CLKPATH0 == hfClk0Source)
        {
            Cy_SysClk_FllGetConfiguration(&fllManualCfg);

            pathMuxFreqHz = (clkSrcFreqHz * reference * (output + 1uL)) / fllManualCfg.fllMult;

            /* FLL is used as the source of High Frequency Clock 0. */
            CapSense_immunity[CapSense_FREQ_CHANNEL_0].fllManualCfg = fllManualCfg;

            if((clkSrcFreqHz + freqShift) > CapSense_P6_MAX_FLL_FREQ_HZ)
            {
                ch1Result = CapSense_SsCalcFllDividers(pathMuxFreqHz, (clkSrcFreqHz - freqShift), &CapSense_immunity[CapSense_FREQ_CHANNEL_1]);
                ch2Result = CapSense_SsCalcFllDividers(pathMuxFreqHz, (clkSrcFreqHz - (freqShift * 2uL)), &CapSense_immunity[CapSense_FREQ_CHANNEL_2]);
            }
            else if((clkSrcFreqHz - freqShift) < CapSense_P6_MIN_FLL_FREQ_HZ)
            {
                ch1Result = CapSense_SsCalcFllDividers(pathMuxFreqHz, (clkSrcFreqHz + freqShift), &CapSense_immunity[CapSense_FREQ_CHANNEL_1]);
                ch2Result = CapSense_SsCalcFllDividers(pathMuxFreqHz, (clkSrcFreqHz + (freqShift * 2uL)), &CapSense_immunity[CapSense_FREQ_CHANNEL_2]);
            }
            else
            {
                ch1Result = CapSense_SsCalcFllDividers(pathMuxFreqHz, (clkSrcFreqHz + freqShift), &CapSense_immunity[CapSense_FREQ_CHANNEL_1]);
                ch2Result = CapSense_SsCalcFllDividers(pathMuxFreqHz, (clkSrcFreqHz - freqShift), &CapSense_immunity[CapSense_FREQ_CHANNEL_2]);
            }
        }
        else if(CY_SYSCLK_HFCLK_IN_CLKPATH1 == hfClk0Source)
        {
            /* PLL is used as the source of High Frequency Clock 0. */
            (void)Cy_SysClk_PllGetConfiguration(0uL, &feedback, &reference, &output, &outputMode);
            pathMuxFreqHz = (clkSrcFreqHz * reference * (output + 1uL)) / feedback;

            CapSense_immunity[CapSense_FREQ_CHANNEL_0].feedback     = feedback;
            CapSense_immunity[CapSense_FREQ_CHANNEL_0].reference    = reference;
            CapSense_immunity[CapSense_FREQ_CHANNEL_0].outputMode   = outputMode;
            CapSense_immunity[CapSense_FREQ_CHANNEL_0].output       = output;

            if((clkSrcFreqHz + freqShift) > CapSense_P6_MAX_PLL_FREQ_HZ)
            {
                ch1Result = CapSense_SsCalcPllDividers(pathMuxFreqHz, (clkSrcFreqHz - freqShift), &CapSense_immunity[CapSense_FREQ_CHANNEL_1]);
                ch2Result = CapSense_SsCalcPllDividers(pathMuxFreqHz, (clkSrcFreqHz - (freqShift * 2uL)), &CapSense_immunity[CapSense_FREQ_CHANNEL_2]);
            }
            else if((clkSrcFreqHz - freqShift) < CapSense_P6_MIN_PLL_FREQ_HZ)
            {
                ch1Result = CapSense_SsCalcPllDividers(pathMuxFreqHz, (clkSrcFreqHz + freqShift), &CapSense_immunity[CapSense_FREQ_CHANNEL_1]);
                ch2Result = CapSense_SsCalcPllDividers(pathMuxFreqHz, (clkSrcFreqHz + (freqShift * 2uL)), &CapSense_immunity[CapSense_FREQ_CHANNEL_2]);
            }
            else
            {
                ch1Result = CapSense_SsCalcPllDividers(pathMuxFreqHz, (clkSrcFreqHz + freqShift), &CapSense_immunity[CapSense_FREQ_CHANNEL_1]);
                ch2Result = CapSense_SsCalcPllDividers(pathMuxFreqHz, (clkSrcFreqHz - freqShift), &CapSense_immunity[CapSense_FREQ_CHANNEL_2]);
            }
        }
        else
        {
            /* Not supported SYSCLK_HFCLK_IN_CLKPATH */
            CY_ASSERT(0u);
        }

        if((CY_RET_SUCCESS != ch1Result) || (CY_RET_SUCCESS != ch2Result))
        {
            retVal = CY_RET_UNKNOWN;
            CapSense_immunity[CapSense_FREQ_CHANNEL_1] = CapSense_immunity[CapSense_FREQ_CHANNEL_0];
            CapSense_immunity[CapSense_FREQ_CHANNEL_2] = CapSense_immunity[CapSense_FREQ_CHANNEL_0];
        }
        else
        {
            retVal = CY_RET_SUCCESS;
        }

        return(retVal);
    }


    /*******************************************************************************
    * Function Name: CapSense_SsCalcFllDividers
    ****************************************************************************//**
    *
    * \brief
    *  Calculates the FLL dividers based on the input frequency and desired output
    *  frequency.
    *
    * \details
    *  Calculates the FLL dividers based on the input frequency and desired output
    *  frequency.
    *
    * \param inputFreq
    *  FLL input frequency.
    *
    * \param outputFreq
    *  Desired output frequency.
    *
    * \param multiplier
    *  Pointer to variable to store feedback divider value.
    *
    * \param reference
    *  Pointer to variable to store reference divider value.
    *
    * \param output
    *  Pointer to variable to store output divider value.
    *
    * \return
    *  Returns the status of the calculation process. If CY_RET_SUCCESS is not
    *  received, then the calculation failed.
    *
    *******************************************************************************/
    cy_status CapSense_SsCalcFllDividers(uint32 inputFreq, uint32 outputFreq, CapSense_PLL_FLL_CFG_TYPE *fllCfg)
    {
        /* for each possible value of FLL_OUTPUT_DIV and FLL_REF_DIV, try
           to find a value for FLL_MULT that gives an output frequency as
           close as possible to the desired output frequency. */
        uint32 outTmp, refTmp, multTmp, ffllTmp;
        uint32 ffllBest = 0uL; /* to ensure at least one pass through the for loops below */
        cy_status retVal = CY_RET_UNKNOWN;

        /* FLL_OUTPUT_DIV is 1 bit; range is 0 to 1. Must be 1 if output
           frequency is less than 48 MHz. */
        for (outTmp = (outputFreq < 48000000ul) ? 1uL : 0uL;
             (outTmp < 2uL) && (ffllBest != outputFreq); outTmp++)
        {
            /* FLL_REF_DIV is 13 bits; allowed range is 1 to 2^13 - 1 */
            for (refTmp = 1uL; (refTmp < 8192uL) && (ffllBest != outputFreq); refTmp++)
            {
                /* calculate a test value for FLL_MULT */
                multTmp = CY_SYSCLK_DIV_ROUND(outputFreq * refTmp * (outTmp + 1uL), inputFreq);
                /* FLL_MULT is 18 bits; allowed range is 1 to 2^18 - 1 */
                if ((1uL <= multTmp) && (multTmp < (1uL << 18)))
                {
                    /* Calculate what output frequency will actually be produced.
                       If it's closer to the target than what we have so far, then
                       save it. */
                    ffllTmp = ((multTmp * inputFreq) / refTmp) / (outTmp + 1uL);
                    if ((uint32)abs((int32)ffllTmp - (int32)outputFreq) <
                        (uint32)abs((int32)ffllBest - (int32)outputFreq))
                    {
                        ffllBest = ffllTmp;
                        fllCfg->fllManualCfg.fllMult = multTmp;
                        fllCfg->fllManualCfg.outputMode = (cy_en_fll_pll_output_mode_t)outTmp;
                        fllCfg->fllManualCfg.refDiv = refTmp;
                        retVal = CY_RET_SUCCESS;
                    }
                }
                /* exit loops if ffllBest equals outputFreq */
            }
        }
        return(retVal);
    }


    /*******************************************************************************
    * Function Name: CapSense_SsCalcPllDividers
    ****************************************************************************//**
    *
    * \brief
    *  Calculates the PLL dividers based on the input frequency and desired output
    *  frequency.
    *
    * \details
    *  Calculates the PLL dividers based on the input frequency and desired output
    *  frequency.
    *
    * \param inputFreq
    *  FLL input frequency.
    *
    * \param outputFreq
    *  Desired output frequency.
    *
    * \param clkPath
    *  Selects which PLL to get dividers from.
    *
    * \param feedback
    *  Pointer to variable to store feedback divider value.
    *
    * \param reference
    *  Pointer to variable to store reference divider value.
    *
    * \param output
    *  Pointer to variable to store output divider value.
    *
    * \return
    *  Returns the status of the calculation process. If CY_RET_SUCCESS is not
    *  received, then the calculation failed.
    *
    *******************************************************************************/
    cy_status CapSense_SsCalcPllDividers(uint32 inputFreq, uint32 outputFreq, CapSense_PLL_FLL_CFG_TYPE *pllCfg)
    {
        /* for each possible value of OUTPUT_DIV and REFERENCE_DIV (refDivTmp), try
           to find a value for FEEDBACK_DIV (fbDivTmp) that gives an output frequency
           as close as possible to the desired output frequency. */
        uint32 fbDivTmp, refDivTmp, outDivTmp, foutTmp;
        uint32 foutBest = 0uL; /* to ensure at least one pass through the for loops below */
        cy_status retVal = CY_RET_UNKNOWN;

        /* OUTPUT_DIV is 9 bits; range is 2 to 2^9 - 1 */
        for (outDivTmp = 2uL; (outDivTmp < 512uL) && (foutBest != outputFreq); outDivTmp++)
        {
            /* REFERENCE_DIV (refDivTmp) is 5 bits; allowed range is 1 to 2^5 - 1 */
            for (refDivTmp = 1uL; (refDivTmp < 31uL) && (foutBest != outputFreq); refDivTmp++)
            {
                /* calculate a test value for FEEDBACK_DIV (fbDivTmp) */
                fbDivTmp = CY_SYSCLK_DIV_ROUND(outputFreq * refDivTmp * outDivTmp, inputFreq);
                /* FEEDBACK_DIV is 7 bits; allowed range is 1 to 50 */
                if ((1uL <= fbDivTmp) && (fbDivTmp <= 50uL))
                {
                    /* Calculate what output frequency will actually be produced.
                       If it's closer to the target than what we have so far, then
                       save it. */
                    foutTmp = ((fbDivTmp * inputFreq) / refDivTmp) / outDivTmp;
                    if ((uint32)abs((int32)foutTmp - (int32)outputFreq) <
                        (uint32)abs((int32)foutBest - (int32)outputFreq))
                    {
                        foutBest = foutTmp;
                        pllCfg->feedback = fbDivTmp;
                        pllCfg->reference = refDivTmp;
                        pllCfg->output = outDivTmp;
                        retVal = CY_RET_SUCCESS;
                    }
                }
                /* exit loops if foutBest equals outputFreq */
            }
        }
        return(retVal);
    }

    /*******************************************************************************
    * Function Name: CapSense_SsChangeClkFreq
    ****************************************************************************//**
    *
    * \brief
    *  This function changes the FLL/PLL frequency.
    *
    * \details
    *  The FLL/PLL frequency can have three offsets: 0%, -5% and +5%. The FLL/PLL
    *  settings are contained in the CapSense_immunity[value] array for each
    *  frequency channel.
    *
    * \param value The frequency channel ID.
    *
    *******************************************************************************/
    void CapSense_SsChangeClkFreq(uint32 value)
    {
        cy_en_hf_clk_sources_t hfClk0Source;

        hfClk0Source = Cy_SysClk_HfClockGetSource(CapSense_P6_PERI_SRC_HFCLK);

        if(CY_SYSCLK_HFCLK_IN_CLKPATH0 == hfClk0Source)
        {
            /* FLL is used as the source of High Frequency Clock 0. */
            (void)Cy_SysClk_FllManualConfigure(&CapSense_immunity[value].fllManualCfg);
        }
        else if(CY_SYSCLK_HFCLK_IN_CLKPATH1 == hfClk0Source)
        {
            /* PLL is used as the source of High Frequency Clock 0. */
            (void)Cy_SysClk_PllManualConfigure(CapSense_P6_PLL_INDEX,
                                         CapSense_immunity[value].feedback,
                                         CapSense_immunity[value].reference,
                                         CapSense_immunity[value].output,
                                         CapSense_immunity[value].outputMode);

        }
        else
        {
            /* Not supported SYSCLK_HFCLK_IN_CLKPATH */
            CY_ASSERT(0u);
        }
    }
#endif  /* (CapSense_ENABLE == CapSense_MULTI_FREQ_SCAN_EN) */


#if((CapSense_ENABLE == CapSense_CSD_EN) ||\
    (CapSense_ENABLE == CapSense_CSX_EN))
/*******************************************************************************
* Function Name: CapSense_SsInitializeSourceSenseClk
****************************************************************************//**
*
* \brief
*  Sets a source for Sense Clk for all CSD widgets.
*
* \details
*  Updates snsClkSource and rowSnsClkSource with a source for the sense Clk.
*  for all CSD widgets.
*
*******************************************************************************/
void CapSense_SsInitializeSourceSenseClk(void)
{
    uint32 wdgtIndex;
    CapSense_RAM_WD_BASE_STRUCT *ptrWdgt;

    for (wdgtIndex = 0u; wdgtIndex < CapSense_TOTAL_WIDGETS; wdgtIndex++)
    {
        ptrWdgt = (CapSense_RAM_WD_BASE_STRUCT *)CapSense_dsFlash.wdgtArray[wdgtIndex].ptr2WdgtRam;
        
        switch(CapSense_GET_SENSE_METHOD(&CapSense_dsFlash.wdgtArray[wdgtIndex]))
        {
        #if (0u != CapSense_TOTAL_CSD_WIDGETS)
            case CapSense_SENSE_METHOD_CSD_E:
                CapSense_SsSetWidgetSenseClkSrc(wdgtIndex, ptrWdgt);
                break;
        #endif /* #if (0u != CapSense_TOTAL_CSD_WIDGETS) */

        #if (0u != CapSense_TOTAL_CSX_WIDGETS)
            case CapSense_SENSE_METHOD_CSX_E:
                CapSense_SsSetWidgetTxClkSrc(wdgtIndex, ptrWdgt);
                break;
        #endif /* #if (0u != CapSense_TOTAL_CSX_WIDGETS) */

        default:
            CY_ASSERT(0u);
            break;
        }
        
        #if (CapSense_ENABLE == CapSense_TST_WDGT_CRC_EN)
            CapSense_DsUpdateWidgetCrc(wdgtIndex);
        #endif /* (CapSense_ENABLE == CapSense_TST_WDGT_CRC_EN) */
    }
}
#endif /* ((CapSense_ENABLE == CapSense_CSD_EN) ||\
           (CapSense_ENABLE == CapSense_CSX_EN)) */


#if (CapSense_ENABLE == CapSense_CSD_EN)
    /*******************************************************************************
    * Function Name: CapSense_SsSetWidgetSenseClkSrc
    ****************************************************************************//**
    *
    * \brief
    *  Sets a source for the sense clock for a widget.
    *
    * \param wdgtIndex
    *   Specifies the ID of the widget.
    * \param ptrWdgt
    *   The pointer to the RAM_WD_BASE_STRUCT structure.
    *
    * \details
    *  Updates snsClkSource and rowSnsClkSource with a source for the sense Clk for a
    *  widget.
    *
    *******************************************************************************/
    static void CapSense_SsSetWidgetSenseClkSrc(uint32 wdgtIndex, CapSense_RAM_WD_BASE_STRUCT * ptrWdgt)
    {
        uint8 lfsrSize;
        uint8 lfsrScale;

        #if(CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSD_SNS_CLK_SOURCE)
            uint32 conversionsNum;
            uint32 snsClkDivider;
        #endif /* (CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSD_SNS_CLK_SOURCE) */

        #if(CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSD_SNS_CLK_SOURCE)
            snsClkDivider = CapSense_SsCSDGetColSnsClkDivider(wdgtIndex);

                conversionsNum = CapSense_SsCSDGetNumberOfConversions(snsClkDivider, (uint32)ptrWdgt->resolution,
                                                                                             CapSense_CLK_SOURCE_DIRECT);
                lfsrSize = CapSense_SsCalcLfsrSize(snsClkDivider, conversionsNum);
                if (CapSense_CLK_SOURCE_DIRECT == lfsrSize)
                {
                    lfsrSize = CapSense_SsCSDCalcPrsSize(snsClkDivider, (uint32)ptrWdgt->resolution);
                }
                lfsrScale = CapSense_SsCalcLfsrScale(snsClkDivider, lfsrSize);
        #else
            lfsrSize = (uint8)CapSense_DEFAULT_MODULATION_MODE;
            lfsrScale = 0u;
            (void)wdgtIndex; /* This parameter is unused in such configurations */
        #endif /* (CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSD_SNS_CLK_SOURCE) */

        ptrWdgt->snsClkSource = lfsrSize | (uint8)(lfsrScale << CapSense_CLK_SOURCE_LFSR_SCALE_OFFSET);

        #if (CapSense_ENABLE == (CapSense_CSD_MATRIX_WIDGET_EN | CapSense_CSD_TOUCHPAD_WIDGET_EN))
            if ((CapSense_WD_TOUCHPAD_E == (CapSense_WD_TYPE_ENUM)CapSense_dsFlash.wdgtArray[wdgtIndex].wdgtType) ||
                (CapSense_WD_MATRIX_BUTTON_E == (CapSense_WD_TYPE_ENUM)CapSense_dsFlash.wdgtArray[wdgtIndex].wdgtType))
            {
                #if(CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSD_SNS_CLK_SOURCE)
                    snsClkDivider = CapSense_SsCSDGetRowSnsClkDivider(wdgtIndex);

                        lfsrSize = CapSense_SsCalcLfsrSize(snsClkDivider, conversionsNum);
                        if (CapSense_CLK_SOURCE_DIRECT == lfsrSize)
                        {
                            lfsrSize = CapSense_SsCSDCalcPrsSize(snsClkDivider, (uint32)ptrWdgt->resolution);
                        }
                        lfsrScale = CapSense_SsCalcLfsrScale(snsClkDivider, lfsrSize);
                #else
                    lfsrSize = (uint8)CapSense_DEFAULT_MODULATION_MODE;
                    lfsrScale = 0u;
                #endif /* (CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSD_SNS_CLK_SOURCE) */
                ptrWdgt->rowSnsClkSource = lfsrSize | (uint8)(lfsrScale << CapSense_CLK_SOURCE_LFSR_SCALE_OFFSET);
            }
        #endif /* (CapSense_ENABLE == (CapSense_CSD_MATRIX_WIDGET_EN | CapSense_CSD_TOUCHPAD_WIDGET_EN)) */
    }
#endif /* (CapSense_ENABLE == CapSense_CSD_EN) */


#if (CapSense_ENABLE == CapSense_CSX_EN)
    /*******************************************************************************
    * Function Name: CapSense_SsSetWidgetTxClkSrc
    ****************************************************************************//**
    *
    * \brief
    *  Sets a source for the Tx clock for a widget.
    *
    * \param wdgtIndex
    *   Specifies the ID of the widget.
    * \param ptrWdgt
    *   The pointer to the RAM_WD_BASE_STRUCT structure.
    *
    * \details
    *  Updates snsClkSource with with a source for Tx Clk for a widget.
    *
    *******************************************************************************/
    static void CapSense_SsSetWidgetTxClkSrc(uint32 wdgtIndex, CapSense_RAM_WD_BASE_STRUCT * ptrWdgt)
    {
        uint8 lfsrSize;
        uint8 lfsrScale;

        #if ((CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSX_TX_CLK_SOURCE) && \
             (CapSense_DISABLE == CapSense_CSX_SKIP_OVSMPL_SPECIFIC_NODES))
            uint32 conversionsNum;
            uint32 snsClkDivider;
        #endif

        #if(CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSX_TX_CLK_SOURCE)
            #if (CapSense_DISABLE == CapSense_CSX_SKIP_OVSMPL_SPECIFIC_NODES)
                conversionsNum = (uint32)ptrWdgt->resolution;
                snsClkDivider = CapSense_SsCSXGetTxClkDivider(wdgtIndex);
                lfsrSize = CapSense_SsCalcLfsrSize(snsClkDivider, conversionsNum);
                lfsrScale = CapSense_SsCalcLfsrScale(snsClkDivider, lfsrSize);
            #else
                lfsrSize = (uint8)CapSense_CLK_SOURCE_DIRECT;
                lfsrScale = 0u;
            #endif /* (CapSense_ENABLE == CapSense_CSX_SKIP_OVSMPL_SPECIFIC_NODES) */
        #else
            lfsrSize = (uint8)CapSense_CSX_TX_CLK_SOURCE;
            lfsrScale = 0u;
        #endif /* (CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSX_TX_CLK_SOURCE) */

        ptrWdgt->snsClkSource = lfsrSize | (uint8)(lfsrScale << CapSense_CLK_SOURCE_LFSR_SCALE_OFFSET);
    }
#endif /* (CapSense_ENABLE == CapSense_CSX_EN) */


#if(((CapSense_ENABLE == CapSense_CSX_EN) && \
     (CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSX_TX_CLK_SOURCE)) ||\
    ((CapSense_ENABLE == CapSense_CSD_EN) && \
     (CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSD_SNS_CLK_SOURCE)))
/*******************************************************************************
* Function Name: CapSense_SsCalcLfsrSize
****************************************************************************//**
*
* \brief
*   This is an internal function that finds a SSC polynomial size in the Auto mode.
*
* \details
*   The SSC polynomial size in the auto mode is found based on the following
*   requirements:
*   - an LFSR value should be selected so that the max clock dither is limited with +/-10%.
*   - at least one full spread spectrum polynomial should pass during the scan time.
*   - the value of the number of conversions should be an integer multiple of the
*     repeat period of the programmed LFSR_SIZE.
*
* \param
*  snsClkDivider The divider value for the sense clock.
*  resolution The widget resolution.
*
* \return lfsrSize The LFSRSIZE value for the SENSE_PERIOD register.
*
*******************************************************************************/
__STATIC_INLINE  uint8 CapSense_SsCalcLfsrSize(uint32 snsClkDivider, uint32 conversionsNum)
{
    uint8 lfsrSize = 0u;

    /* Find LFSR value */
    if((CapSense_SNSCLK_SSC4_THRESHOLD <= snsClkDivider) &&
       (CapSense_SNSCLK_SSC4_PERIOD <= conversionsNum) &&
       (0uL == (conversionsNum % CapSense_SNSCLK_SSC4_PERIOD)))
    {
        lfsrSize = CapSense_CLK_SOURCE_SSC4;
    }
    else if((CapSense_SNSCLK_SSC3_THRESHOLD <= snsClkDivider) &&
            (CapSense_SNSCLK_SSC3_PERIOD <= conversionsNum) &&
            (0uL == (conversionsNum % CapSense_SNSCLK_SSC3_PERIOD)))
    {
        lfsrSize = CapSense_CLK_SOURCE_SSC3;
    }
    else if((CapSense_SNSCLK_SSC2_THRESHOLD <= snsClkDivider) &&
            (CapSense_SNSCLK_SSC2_PERIOD <= conversionsNum) &&
            (0uL == (conversionsNum % CapSense_SNSCLK_SSC2_PERIOD)))
    {
        lfsrSize = CapSense_CLK_SOURCE_SSC2;
    }
    else if((CapSense_SNSCLK_SSC1_THRESHOLD <= snsClkDivider) &&
            (CapSense_SNSCLK_SSC1_PERIOD <= conversionsNum) &&
            (0uL == (conversionsNum % CapSense_SNSCLK_SSC1_PERIOD)))
    {
        lfsrSize = CapSense_CLK_SOURCE_SSC1;
    }
    else
    {
        lfsrSize = (uint8)CapSense_CLK_SOURCE_DIRECT;
    }

    return (lfsrSize);
}


/*******************************************************************************
* Function Name: CapSense_SsCalcLfsrScale
****************************************************************************//**
*
* \brief
*   This is an internal function that calculates the LFSR scale value.
*
* \details
*   The LFSR scale value is used to increase the clock dither if the desired dither
*   is wider than can be achieved with the current Sense Clock Divider and current LFSR
*   period.
*
*   This returns the LFSR scale value needed to keep the clock dither in
*   range +/-10%.
*
* \param
*  snsClkDivider The divider value for the sense clock.
*  lfsrSize The period of the LFSR sequence.
*           CapSense_CLK_SOURCE_DIRECT The spreadspectrum is not used.
*           CapSense_CLK_SOURCE_SSC1   The length of LFSR sequence is 63 cycles.
*           CapSense_CLK_SOURCE_SSC2   The length of LFSR sequence is 127 cycles.
*           CapSense_CLK_SOURCE_SSC3   The length of LFSR sequence is 255 cycles.
*           CapSense_CLK_SOURCE_SSC4   The length of LFSR sequence is 511 cycles.
*
* \return The LFSR scale value needed to keep the clock dither in range +/-10%.
*
*******************************************************************************/
__STATIC_INLINE uint8 CapSense_SsCalcLfsrScale(uint32 snsClkDivider, uint8 lfsrSize)
{
    uint32 lfsrScale;
    uint32 lfsrRange;
    uint32 lfsrDither;

    /* Initialize the lfsrSize variable with the LFSR Range for given Lfsr Size. */
    switch(lfsrSize)
    {
        case CapSense_CLK_SOURCE_SSC1:
        {
            lfsrRange = CapSense_SNSCLK_SSC1_RANGE;
            break;
        }
        case CapSense_CLK_SOURCE_SSC2:
        {
            lfsrRange = CapSense_SNSCLK_SSC2_RANGE;
            break;
        }
        case CapSense_CLK_SOURCE_SSC3:
        {
            lfsrRange = CapSense_SNSCLK_SSC3_RANGE;
            break;
        }
        case CapSense_CLK_SOURCE_SSC4:
        {
            lfsrRange = CapSense_SNSCLK_SSC4_RANGE;
            break;
        }
        default:
        {
            lfsrRange = 0u;
            break;
        }
    }

    /* Calculate the LFSR Scale value that is required to keep the Clock dither
     * as close as possible to the +/-10% limit of the used frequency.
     */
    if((lfsrSize != CapSense_CLK_SOURCE_DIRECT) && (0u != lfsrRange))
    {
        /* Calculate the LFSR Dither in percents. */
        lfsrDither  = ((lfsrRange * 100uL) / snsClkDivider);
        lfsrScale = 0uL;

        while(lfsrDither < CapSense_LFSR_DITHER_PERCENTAGE)
        {
            lfsrScale++;
            lfsrDither <<=1uL;
        }

        if(lfsrDither > CapSense_LFSR_DITHER_PERCENTAGE)
        {
            lfsrScale--;
        }
    }
    else
    {
        lfsrScale = 0uL;
    }

    return ((uint8)lfsrScale);
}
#endif /* (((CapSense_ENABLE == CapSense_CSX_EN) &&  \
            (CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSX_TX_CLK_SOURCE)) ||\
          ((CapSense_ENABLE == CapSense_CSD_EN) &&  \
           (CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSD_SNS_CLK_SOURCE))) */


#if (CapSense_ENABLE == CapSense_CSD_EN)
    /*******************************************************************************
    * Function Name: CapSense_SsClearCSDSensors
    ****************************************************************************//**
    *
    * \brief
    *  Resets all the CSD sensors to the non-sampling state by sequentially
    *  disconnecting all the sensors from the Analog MUX bus and putting them to
    *  an inactive state.
    *
    * \details
    *   The function goes through all the widgets and updates appropriate bits in
    *   the IO HSIOM, PC and DR registers depending on the Inactive sensor connection
    *   parameter. DR register bits are set to zero when the Inactive sensor
    *   connection is Ground or Hi-Z.
    *
    *******************************************************************************/
    void CapSense_SsClearCSDSensors(void)
    {
        uint32 wdgtIndex;
        uint32 snsIndex;

        #if (CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN)
            uint32 pinIndex;
        #endif /* (CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN)  */

        #if (CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN)
            /* Declare ptr to sensor IO structure */
            CapSense_FLASH_IO_STRUCT const *curDedicatedSnsIOPtr;
            /*  Pointer to Flash structure holding info of sensor to be scanned  */
            CapSense_FLASH_SNS_STRUCT const *curFlashSnsPtr;
        #endif /* (CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN)  */
        CapSense_FLASH_IO_STRUCT const *curSnsIOPtr;

        for (wdgtIndex = 0u; wdgtIndex < CapSense_TOTAL_WIDGETS; wdgtIndex++)
        {
            if (CapSense_SENSE_METHOD_CSD_E ==
                CapSense_GET_SENSE_METHOD(&CapSense_dsFlash.wdgtArray[wdgtIndex]))
            {
                curSnsIOPtr = (CapSense_FLASH_IO_STRUCT const *)
                                                CapSense_dsFlash.wdgtArray[wdgtIndex].ptr2SnsFlash;

                /* Go through all sensors in widget */
                for (snsIndex = 0u; snsIndex < (uint8)CapSense_dsFlash.wdgtArray[(wdgtIndex)].totalNumSns; snsIndex++)
                {
                    #if (CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN)
                        /* Check ganged sns flag  */
                        if (CapSense_GANGED_SNS_MASK == (CapSense_dsFlash.wdgtArray[wdgtIndex].staticConfig &
                                                                 CapSense_GANGED_SNS_MASK))
                        {
                            /* Get sns pointer */
                            curFlashSnsPtr = (CapSense_FLASH_SNS_STRUCT const *)
                                                               CapSense_dsFlash.wdgtArray[wdgtIndex].ptr2SnsFlash +
                                                               snsIndex;

                            for(pinIndex = 0u; pinIndex < curFlashSnsPtr->numPins; pinIndex++)
                            {
                                /* Get IO pointer for dedicated pin */
                                curDedicatedSnsIOPtr = &CapSense_ioList[curFlashSnsPtr->firstPinId + pinIndex];

                                /* Disconnect dedicated pin */
                                CapSense_CSDDisconnectSns(curDedicatedSnsIOPtr);
                            }
                        }
                        else
                        {
                            /* Disable sensor */
                            CapSense_CSDDisconnectSns(&curSnsIOPtr[snsIndex]);
                        }
                    #else
                        /* Disable sensor */
                        CapSense_CSDDisconnectSns(&curSnsIOPtr[snsIndex]);
                    #endif /* (CapSense_ENABLE == CapSense_CSD_GANGED_SNS_EN)  */
                }
            }
        }
    }

    /*******************************************************************************
    * Function Name: CapSense_SsCSDGetColSnsClkDivider
    ****************************************************************************//**
    *
    * \brief
    *  This function gets the Sense Clock Divider value for one-dimension widgets
    *  and Column Sense Clock divider value for two-dimension widgets.
    *
    * \details
    *  This function gets the Sense Clock Divider value based on the component
    *  configuration. The function is applicable for one-dimension widgets and for
    *  two-dimension widgets.
    *
    * \param
    *  widgetId Specifies the ID of the widget.
    *
    * \return The Sense Clock Divider value for one-dimension widgets
    *         and the Column Sense Clock divider value for two-dimension widgets.
    *
    *******************************************************************************/
    uint32 CapSense_SsCSDGetColSnsClkDivider(uint32 widgetId)
    {
        uint32 retVal;

        /* Get sense divider based on configuration */
        #if (CapSense_ENABLE != CapSense_CSD_COMMON_SNS_CLK_EN)
            CapSense_RAM_WD_BASE_STRUCT *ptrWdgt;

            ptrWdgt = (CapSense_RAM_WD_BASE_STRUCT *)
            CapSense_dsFlash.wdgtArray[widgetId].ptr2WdgtRam;

            retVal = (uint32)(ptrWdgt->snsClk);
        #else
            retVal = (uint32)CapSense_dsRam.snsCsdClk;

            (void)widgetId; /* This parameter is unused in such configurations */
        #endif /* (CapSense_ENABLE == CapSense_CSD_COMMON_SNS_CLK_EN) */

        return (retVal);
    }


    #if (CapSense_ENABLE == (CapSense_CSD_MATRIX_WIDGET_EN | CapSense_CSD_TOUCHPAD_WIDGET_EN))
        /*******************************************************************************
        * Function Name: CapSense_SsCSDGetRowSnsClkDivider
        ****************************************************************************//**
        *
        * \brief
        *  This function gets the Sense Clock Divider value for one-dimension widgets
        *  and the Column Sense Clock divider value for two-dimension widgets.
        *
        * \details
        *  This function gets the Sense Clock Divider value based on the component
        *  configuration. The function is applicable for one-dimension widgets and for
        *  two-dimension widgets.
        *
        * \param
        *  widgetId Specifies the ID of the widget.
        *
        * \return The Sense Clock Divider value for one-dimension widgets
        *         and Column Sense Clock divider value for two-dimension widgets.
        *
        *******************************************************************************/
        uint32 CapSense_SsCSDGetRowSnsClkDivider(uint32 widgetId)
        {
            uint32 retVal;

            /* Get sense divider based on configuration */
            #if (CapSense_ENABLE != CapSense_CSD_COMMON_SNS_CLK_EN)
                CapSense_RAM_WD_BASE_STRUCT *ptrWdgt;

                ptrWdgt = (CapSense_RAM_WD_BASE_STRUCT *)
                CapSense_dsFlash.wdgtArray[widgetId].ptr2WdgtRam;

                retVal = ptrWdgt->rowSnsClk;
            #else
                retVal = (uint32)CapSense_dsRam.snsCsdClk;

                (void)widgetId; /* This parameter is unused in such configurations */
            #endif /* (CapSense_ENABLE == CapSense_CSD_COMMON_SNS_CLK_EN) */

            return (retVal);
        }
    #endif /* (CapSense_ENABLE == (CapSense_CSD_MATRIX_WIDGET_EN | CapSense_CSD_TOUCHPAD_WIDGET_EN)) */

    #if (CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSD_SNS_CLK_SOURCE)
        /*******************************************************************************
        * Function Name: CapSense_SsCSDCalcPrsSize
        ****************************************************************************//**
        *
        * \brief
        *   The function finds PRS polynomial size in the Auto mode.
        *
        * \details
        *   The PRS polynomial size in the Auto mode is found based on the following
        *   requirements:
        *   - at least one full spread spectrum polynomial should pass during scan time.
        *
        * \param
        *  snsClkDivider The divider value for the sense clock.
        *  resolution The widget resolution.
        *
        * \return prsSize PRS value for SENSE_PERIOD register.
        *
        *******************************************************************************/
        uint8 CapSense_SsCSDCalcPrsSize(uint32 snsClkDivider, uint32 resolution)
        {
            uint32 prsSize;
            uint32 modClkDivider;

            modClkDivider = 1uL;

            if ((snsClkDivider * CapSense_PRS_LENGTH_12_BITS) <
                (modClkDivider * ((0x00000001Lu << resolution) - 1u)))
            {
                /* Set PRS12 mode */
                prsSize = CapSense_CLK_SOURCE_PRS12;
            }
            else if ((snsClkDivider * CapSense_PRS_LENGTH_8_BITS) <
                     (modClkDivider * ((0x00000001Lu << resolution) - 1u)))
            {
                /* Set PRS8 mode */
                prsSize = CapSense_CLK_SOURCE_PRS8;
            }
            else
            {
                /* Set Direct clock mode */
                prsSize = CapSense_CLK_SOURCE_DIRECT;
            }

            return (uint8)prsSize;
        }
    #endif /* (CapSense_CLK_SOURCE_PRSAUTO == CapSense_CSD_SNS_CLK_SOURCE) */
#endif /* (CapSense_ENABLE == CapSense_CSD_EN) */


/* [] END OF FILE */
