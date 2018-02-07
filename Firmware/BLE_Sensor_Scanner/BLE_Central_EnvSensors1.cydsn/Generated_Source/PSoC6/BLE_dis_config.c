/***************************************************************************//**
* \file CY_BLE_dis_config.c
* \version 2.0
* 
* \brief
*  This file contains the source code of initialization of the config structure
*  for the Device Information Service
* 
********************************************************************************
* \copyright
* Copyright 2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#include "ble/cy_ble_dis.h"

#if(CY_BLE_MODE_PROFILE && defined(CY_BLE_DIS))

#ifdef CY_BLE_DIS_SERVER
static const cy_stc_ble_diss_t cy_ble_diss =
{
    0x000Au,    /* Handle of the DIS service */
    {
        0x000Cu, /* Handle of the Manufacturer Name String characteristic */ 
        0x000Eu, /* Handle of the Model Number String characteristic */ 
        CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the Serial Number String characteristic */ 
        0x0010u, /* Handle of the Hardware Revision String characteristic */ 
        CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the Firmware Revision String characteristic */ 
        CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the Software Revision String characteristic */ 
        CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the System ID characteristic */ 
        CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the IEEE 11073-20601 Regulatory Certification Data List characteristic */ 
        CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE, /* Handle of the PnP ID characteristic */ 
    },
};
#endif /* CY_BLE_DIS_SERVER */

/**
* \addtogroup group_globals
* @{
*/

/** The configuration structure for the Device Information Service. */
cy_stc_ble_dis_config_t cy_ble_disConfig =
{
    /* Service GATT DB handles structure */
    #ifdef CY_BLE_DIS_SERVER
    .diss = &cy_ble_diss,
    #else
    .diss = NULL,
    #endif /* CY_BLE_DIS_SERVER */

    /* An application layer event callback function to receive service events from the BLE Component. */
    .callbackFunc = NULL,
};

/** @} group_globals */

#endif /* (CY_BLE_MODE_PROFILE && defined(CY_BLE_DIS)) */

/* [] END OF FILE */
