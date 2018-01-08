/***************************************************************************//**
* \file cy_flash.h
* \version 2.0
*
* Provides the API declarations of the Flash driver.
*
********************************************************************************
* \copyright
* Copyright 2016-2017, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_FLASH_H)
#define CY_FLASH_H

/**
* \defgroup group_flash Flash System Routine (Flash)
* \{
* Internal flash memory programming
*
* Flash memory in PSoC devices provides non-volatile storage for user firmware,
* user configuration data, and bulk data storage.
*
* Flash operations are implemented as system calls. System calls are executed
* out of SROM in the privileged mode of operation. Users have no access to read
* or modify the SROM code. The driver API requests the system call by acquiring
* the Inter-processor communication (IPC) and writing the SROM function opcode
* and parameters to its input registers. As a result, an NMI interrupt is invoked
* and the requested SROM API is executed. The operation status is returned to the
* driver context and a release interrupt is triggered.
*
* Writing to flash can take up to 20 milliseconds. During this time,
* the device should not be reset (including XRES pin, software  reset, and
* watchdog) or unexpected changes may be made to portions of the flash.
* Also, the low-voltage detect circuits should be configured to generate an
* interrupt instead of a reset.
*
* A Read while Write violation occurs when a flash Read operation is initiated
* in the same or neighboring flash sector where the flash Write, Erase, or
* Program operation is working. This violation may cause a HardFault exception.
* To avoid the Read while Write violation, the user must carefully split the
* Read and Write operation on flash sectors which are not neighboring,
* considering both cores in the multi-processor device. The flash is divided
* into four equal sectors. You may edit the linker script to place the code
* into neighboring sectors. For example, use sectors number 0 and 1 for code
* and sectors 2 and 3 for data storage.
*
* \section group_flash_configuration Configuration Considerations
*
* \subsection group_flash_config_intro Introduction:
* The PSoC 63/62/61/60 user-programmable Flash consists of four User Flash
* sectors (0 through 3) and an EEPROM emulation sector. Write operations are
* performed on a per-sector basis and may be done as Blocking or Partially
* Non-Blocking, defined as follows:
*
* \subsection group_flash_config_blocking Blocking:
* In this case, the entire Flash block is not available for the duration of the
* Write (16ms). Therefore no Flash accesses (from any Bus Master) can occur
* during that time. CPU execution can be performed from SRAM. All pre-fetching
* must be disabled.
*
* \subsection group_flash_config_block_const Constraints for Blocking Flash operations:
* -# During write to flash, the device should not be reset (including XRES pin,
*    software reset, and watchdog) or unexpected changes may be made to portions
*    of the flash.
* -# The low-voltage detect circuits should be configured to generate an interrupt
*    instead of a reset.
* -# The part must be in LP mode for Flash write operations. Flash write cannot
*    be performed in ULP mode.
* -# Interrupts must be enabled on both active cores. Do not enter a critical
*    section during flash operation.
* -# User must guarantee that during flash write operation no flash read
*    operations are performed by bus masters other than CM0+ and CM4 (DMA and
*    Crypto).
* -# If you do not use the default startup, ensure that firmware calls the
*    following functions before any flash write/erase operations:
* \snippet Flash_sut_01.cydsn/main_cm0p.c Flash Initialization
*
* \subsection group_flash_config_rww Partially Non-Blocking:
* At this time, Partially Non-Blocking operations are available for EEPROM-emulation only.
* This method has a much shorter time window when Flash accesses are not allowed.
* Instead of being blocked for 16ms, there is a window of non-availability of
* 2.1ms after the Write operation commences and then two windows of 150 uS
* duration each. This results in Flash availability for about 85% of the Blocking
* interval - see Figure 1 - Blocking Intervals in Flash Write operation.
*
* \image html flash-rww-diagram.png Figure 1 - Blocking Intervals in Flash Write operation
*
* This capability is important for communication protocols that rely on fast
* response. Some constraints must be planned for in the Partially Non-Blocking
* mode which are described in detail as follows:
*
* \subsection group_flash_config_rww_const Constraints for Partially Non-Blocking Flash operations:
* -# Do not use Partially Non-Blocking Flash operation if you write in User
*    Flash. It should be used for writing in EmEEPROM only.
* -# During write to flash, the device should not be reset (including XRES pin,
*    software reset, and watchdog) or unexpected changes may be made to portions
*    of the flash.
* -# The low-voltage detect circuits should be configured to generate an interrupt
*    instead of a reset.
* -# Do not write to and read/execute from the same flash sector at the same time.
*    This is true for all sectors. (In this context, read means read of any bus
*    master: CM0+, CM4, DMA, Crypto, etc.)
* -# The part must be in LP mode for Flash write operations. Flash write cannot
*    be performed in ULP mode.
* -# Interrupts must be enabled on both active cores. Do not enter a Critical
*    Section during flash operation.
* -# User Interrupts must have priority lower than 0 (Flash Macro interrupt).
* -# User must guarantee that during flash write operation no flash read
*    operations are performed by bus masters other than CM0+ and CM4 (DMA and
*    Crypto).
* -# If you do not use the default startup, ensure that firmware calls the
*    following functions before any flash write/erase operations:
* \snippet Flash_sut_01.cydsn/main_cm0p.c Flash Initialization
*
* \section group_flash_more_information More Information
*
* See the technical reference manual (TRM) for more information about the Flash
* architecture.
*
* \section group_flash_MISRA MISRA-C Compliance
*
* <table class="doxtable">
*   <tr>
*     <th>MISRA Rule</th>
*     <th>Rule Class (Required/Advisory)</th>
*     <th>Rule Description</th>
*     <th style="width: 50%;">Description of Deviation(s)</th>
*   </tr>
*   <tr>
*     <td>11.4</td>
*     <td>A</td>
*     <td>Casting to different object pointer type.</td>
*     <td>The cast of the uint32_t pointer to pipe message structure pointer
*         is used to get transmitted data via the \ref group_ipc channel.
*         We cast only one pointer, so there is no way to avoid this cast.</td>
*   </tr>
*   <tr>
*     <td>11.5</td>
*     <td>R</td>
*     <td>Not performed, the cast that removes any const or volatile qualification from the type addressed by a pointer.</td>
*     <td>The removal of the volatile qualification inside the function has no side effects.</td>
*   </tr>
* </table>
*
* \section group_flash_changelog Changelog
*
* <table class="doxtable">
*   <tr><th>Version</th><th>Changes</th><th>Reason for Change</th></tr>
*   <tr>
*     <td rowspan="3"> 2.0</td>
*     <td>Added non-blocking erase function - Cy_Flash_StartErase().
*         Removed the clear cache function call.</td>
*     <td>The clear cache operation is removed from the blocking Write/Erase
*         function because in this case it is performed by the hardware.
*         Otherwise it is documented that it is the user's responsibility to
*         clear the cache after executing the non-blocking Write/Erase flash
*         operation.</td>
*   </tr>
*   <tr>
*     <td>Added new Cy_Flash_IsOperationComplete() function to check completeness.
*         Obsoleted Cy_Flash_IsWriteComplete(), Cy_Flash_IsProgramComplete(),
*         and Cy_Flash_IsEraseComplete() functions.<br>
*         Added Cy_Flash_GetExternalStatus() function to get unparsed status where
*         flash driver will be used in security applications with other modules
*         as SecureImage.<br>
*         Added Cy_Flash_Init() function to initialize all needed prerequisites
*         for Erase/Write operations.</td>
*     <td>Updated driver design to improve user experience.</td>
*   </tr>
*   <tr>
*     <td>Updated driver implementation to remove MISRA rules deviations.</td>
*     <td>Driver implementation quality improvement.</td>
*   </tr>
*   <tr>
*     <td>1.0</td>
*     <td>Initial version</td>
*     <td></td>
*   </tr>
* </table>
*
* \defgroup group_flash_macros Macros
* \{
*     \defgroup group_flash_general_macros Flash general parameters
*         Provides general information about flash
*
*     \defgroup group_flash_config_macros  Flash configuration
*         Specifies the parameter values passed to SROM API
* \}
* \defgroup group_flash_functions Functions
* \defgroup group_flash_enumerated_types Enumerated Types
*/

#include <cy_device_headers.h>
#include "syslib/cy_syslib.h"

#if defined(__cplusplus)
extern "C" {
#endif

/***************************************
* Macro definitions
***************************************/
/**
* \addtogroup group_flash_macros
* \{
*/

/** Driver major version */
#define CY_FLASH_DRV_VERSION_MAJOR       2

/** Driver minor version */
#define CY_FLASH_DRV_VERSION_MINOR       0

#define CY_FLASH_ID               (CY_PDL_DRV_ID(0x14UL))                          /**< FLASH PDL ID */

#define CY_FLASH_ID_INFO          (uint32_t)( CY_FLASH_ID | CY_PDL_STATUS_INFO )   /**< Return prefix for FLASH driver function status codes */
#define CY_FLASH_ID_WARNING       (uint32_t)( CY_FLASH_ID | CY_PDL_STATUS_WARNING) /**< Return prefix for FLASH driver function warning return values */
#define CY_FLASH_ID_ERROR         (uint32_t)( CY_FLASH_ID | CY_PDL_STATUS_ERROR)   /**< Return prefix for FLASH driver function error return values */

/** \} group_flash_macros */


/**
* \addtogroup group_flash_general_macros
* \{
*/

/** Flash row size */
#define CY_FLASH_SIZEOF_ROW                (CPUSS_FLASHC_PA_SIZE * 4u)
/** Number of flash rows */
#define CY_FLASH_NUMBER_ROWS               (CY_FLASH_SIZE / CY_FLASH_SIZEOF_ROW)
/** Long words flash row size */
#define CY_FLASH_SIZEOF_ROW_LONG_UNITS     (CY_FLASH_SIZEOF_ROW / sizeof(uint32_t))

/** \} group_flash_general_macros */


/**
* \addtogroup group_flash_enumerated_types
* \{
*/

/** This enum has the return values of the Flash driver */
typedef enum cy_en_flashdrv_status
{
    CY_FLASH_DRV_SUCCESS                  =   0x00UL,  /**< Success */
    CY_FLASH_DRV_INV_PROT                 =   ( CY_FLASH_ID_ERROR + 0x0UL),  /**< Invalid device protection state */
    CY_FLASH_DRV_INVALID_FM_PL            =   ( CY_FLASH_ID_ERROR + 0x1UL),  /**< Invalid flash page latch address */
    CY_FLASH_DRV_INVALID_FLASH_ADDR       =   ( CY_FLASH_ID_ERROR + 0x2UL),  /**< Invalid flash address */
    CY_FLASH_DRV_ROW_PROTECTED            =   ( CY_FLASH_ID_ERROR + 0x3UL),  /**< Row is write protected */
    CY_FLASH_DRV_IPC_BUSY                 =   ( CY_FLASH_ID_ERROR + 0x5UL),  /**< IPC structure is already locked by another process */
    CY_FLASH_DRV_INVALID_INPUT_PARAMETERS =   ( CY_FLASH_ID_ERROR + 0x6UL),  /**< Input parameters passed to Flash API are not valid */
    CY_FLASH_DRV_PL_ROW_COMP_FA           =   ( CY_FLASH_ID_ERROR + 0x22UL), /**< Comparison between Page Latches and FM row failed */
    CY_FLASH_DRV_ERR_UNC                  =   ( CY_FLASH_ID_ERROR + 0xFFUL), /**< Unknown error code. See \ref Cy_Flash_GetExternalStatus() */
    CY_FLASH_DRV_PROGRESS_NO_ERROR        =   ( CY_FLASH_ID_INFO  + 0x0UL),  /**< Command in progress; no error */
    CY_FLASH_DRV_OPERATION_STARTED        =   ( CY_FLASH_ID_INFO  + 0x1UL),  /**< Flash operation is successfully initiated */
    CY_FLASH_DRV_OPCODE_BUSY              =   ( CY_FLASH_ID_INFO  + 0x2UL)   /**< Flash is under operation */
} cy_en_flashdrv_status_t;

/** \} group_flash_enumerated_types */

/***************************************
* Function Prototypes
***************************************/

/**
* \addtogroup group_flash_functions
* \{
*/
void Cy_Flash_Init(void);
cy_en_flashdrv_status_t Cy_Flash_EraseRow(uint32_t rowAddr);
cy_en_flashdrv_status_t Cy_Flash_WriteRow(uint32_t rowAddr, const uint32_t* data);
cy_en_flashdrv_status_t Cy_Flash_StartWrite(uint32_t rowAddr, const uint32_t* data);
cy_en_flashdrv_status_t Cy_Flash_StartProgram(uint32_t rowAddr, const uint32_t* data);
cy_en_flashdrv_status_t Cy_Flash_StartErase(uint32_t rowAddr);
cy_en_flashdrv_status_t Cy_Flash_IsOperationComplete(void);
cy_en_flashdrv_status_t Cy_Flash_RowChecksum(uint32_t rowNum, uint32_t* checksumPtr);
cy_en_flashdrv_status_t Cy_Flash_CalculateHash(const uint32_t* data, uint32_t numberOfBytes, uint32_t* hashPtr);
uint32_t Cy_Flash_GetExternalStatus(void);
/** \} group_flash_functions */

/** \cond INTERNAL */
/* Macros to backward compatibility */
#define     Cy_Flash_IsWriteComplete(...)    Cy_Flash_IsOperationComplete()
#define     Cy_Flash_IsProgramComplete(...)  Cy_Flash_IsOperationComplete()
#define     Cy_Flash_IsEraseComplete(...)    Cy_Flash_IsOperationComplete()
/** \endcond */

#if defined(__cplusplus)
}
#endif


#endif /* #if !defined(CY_FLASH_H) */

/** \} group_flash */


/* [] END OF FILE */
