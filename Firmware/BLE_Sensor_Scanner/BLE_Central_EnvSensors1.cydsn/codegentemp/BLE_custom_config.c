/***************************************************************************//**
* \file CY_BLE_custom_config.c
* \version 2.0
* 
* \brief
*  This file contains the source code of initialization of the config structure for
*  the Custom Service.
********************************************************************************
* \copyright
* Copyright 2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#include "ble/cy_ble_custom.h"

#if(CY_BLE_MODE_PROFILE && defined(CY_BLE_CUSTOM))

#ifdef CY_BLE_CUSTOM_SERVER
/* If any Custom Service with custom characteristics is defined in the
* customizer's GUI, their handles will be present in this array.
*/
/* This array contains attribute handles for the defined Custom Services and their characteristics and descriptors.
   The array index definitions are located in the BLE_custom.h file. */


#endif /* (CY_BLE_CUSTOM_SERVER) */

#ifdef CY_BLE_CUSTOM_CLIENT

static const cy_stc_ble_uuid128_t cy_ble_customUuid128[] = {
    /* BME280_Temp */
    { {0x00u, 0x00u, 0xFBu, 0x34u, 0x9Bu, 0x5Fu, 0x80u, 0x00u, 0x00u, 0x80u, 0x00u, 0x10u, 0x01u, 0x50u, 0xA0u, 0x00u} },
    /* Custom Characteristic */
    { {0x00u, 0x00u, 0xFBu, 0x34u, 0x9Bu, 0x5Fu, 0x80u, 0x00u, 0x00u, 0x80u, 0x00u, 0x10u, 0x02u, 0x50u, 0xA0u, 0x00u} },
    /* BME280_Pressure */
    { {0x00u, 0x00u, 0xFBu, 0x34u, 0x9Bu, 0x5Fu, 0x80u, 0x00u, 0x00u, 0x80u, 0x00u, 0x10u, 0x11u, 0x50u, 0xA0u, 0x00u} },
    /* BME280_Pressure */
    { {0x00u, 0x00u, 0xFBu, 0x34u, 0x9Bu, 0x5Fu, 0x80u, 0x00u, 0x00u, 0x80u, 0x00u, 0x10u, 0x12u, 0x50u, 0xA0u, 0x00u} },
    /* BME280_Humidity */
    { {0x00u, 0x00u, 0xFBu, 0x34u, 0x9Bu, 0x5Fu, 0x80u, 0x00u, 0x00u, 0x80u, 0x00u, 0x10u, 0x21u, 0x50u, 0xA0u, 0x00u} },
    /* BME280_Humidity */
    { {0x00u, 0x00u, 0xFBu, 0x34u, 0x9Bu, 0x5Fu, 0x80u, 0x00u, 0x00u, 0x80u, 0x00u, 0x10u, 0x22u, 0x50u, 0xA0u, 0x00u} },
    /* CCS811_TVOC */
    { {0x00u, 0x00u, 0xFBu, 0x34u, 0x9Bu, 0x5Fu, 0x80u, 0x00u, 0x00u, 0x80u, 0x00u, 0x10u, 0x31u, 0x50u, 0xA0u, 0x00u} },
    /* CCS811_TVOC */
    { {0x00u, 0x00u, 0xFBu, 0x34u, 0x9Bu, 0x5Fu, 0x80u, 0x00u, 0x00u, 0x80u, 0x00u, 0x10u, 0x32u, 0x50u, 0xA0u, 0x00u} },
    /* CCS811_ECO2 */
    { {0x00u, 0x00u, 0xFBu, 0x34u, 0x9Bu, 0x5Fu, 0x80u, 0x00u, 0x00u, 0x80u, 0x00u, 0x10u, 0x41u, 0x50u, 0xA0u, 0x00u} },
    /* CCS811_ECO2 */
    { {0x00u, 0x00u, 0xFBu, 0x34u, 0x9Bu, 0x5Fu, 0x80u, 0x00u, 0x00u, 0x80u, 0x00u, 0x10u, 0x42u, 0x50u, 0xA0u, 0x00u} },
    /* EnvSensors Service */
    { {0x00u, 0x00u, 0xFBu, 0x34u, 0x9Bu, 0x5Fu, 0x80u, 0x00u, 0x00u, 0x80u, 0x00u, 0x10u, 0x51u, 0x50u, 0xA0u, 0x00u} },
    /* EnvSenors */
    { {0x00u, 0x00u, 0xFBu, 0x34u, 0x9Bu, 0x5Fu, 0x80u, 0x00u, 0x00u, 0x80u, 0x00u, 0x10u, 0x52u, 0x50u, 0xA0u, 0x00u} },
};

static const cy_ble_uuid16_t cy_ble_customUuid16[] = {
    /* Client Characteristic Configuration */
    0x2902u,
    /* Client Characteristic Configuration */
    0x2902u,
    /* Client Characteristic Configuration */
    0x2902u,
    /* Client Characteristic Configuration */
    0x2902u,
    /* Client Characteristic Configuration */
    0x2902u,
    /* Client Characteristic Configuration */
    0x2902u,
};

static cy_stc_ble_customc_desc_t cy_ble_customCDesc[0x06u] = {

    /* Client Characteristic Configuration */
    {
        .descHandle = {CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE}, /* Descriptor handle */ 
        .uuid       = (const cy_ble_uuid16_t *) &cy_ble_customUuid16[0], /* 16-bit UUID */ 
        .uuidFormat = 0x01u, /* UUID Format - 16-bit (0x01) or 128-bit (0x02) */ 
    },

    /* Client Characteristic Configuration */
    {
        .descHandle = {CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE}, /* Descriptor handle */ 
        .uuid       = (const cy_ble_uuid16_t *) &cy_ble_customUuid16[1], /* 16-bit UUID */ 
        .uuidFormat = 0x01u, /* UUID Format - 16-bit (0x01) or 128-bit (0x02) */ 
    },

    /* Client Characteristic Configuration */
    {
        .descHandle = {CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE}, /* Descriptor handle */ 
        .uuid       = (const cy_ble_uuid16_t *) &cy_ble_customUuid16[2], /* 16-bit UUID */ 
        .uuidFormat = 0x01u, /* UUID Format - 16-bit (0x01) or 128-bit (0x02) */ 
    },

    /* Client Characteristic Configuration */
    {
        .descHandle = {CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE}, /* Descriptor handle */ 
        .uuid       = (const cy_ble_uuid16_t *) &cy_ble_customUuid16[3], /* 16-bit UUID */ 
        .uuidFormat = 0x01u, /* UUID Format - 16-bit (0x01) or 128-bit (0x02) */ 
    },

    /* Client Characteristic Configuration */
    {
        .descHandle = {CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE}, /* Descriptor handle */ 
        .uuid       = (const cy_ble_uuid16_t *) &cy_ble_customUuid16[4], /* 16-bit UUID */ 
        .uuidFormat = 0x01u, /* UUID Format - 16-bit (0x01) or 128-bit (0x02) */ 
    },

    /* Client Characteristic Configuration */
    {
        .descHandle = {CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE}, /* Descriptor handle */ 
        .uuid       = (const cy_ble_uuid16_t *) &cy_ble_customUuid16[5], /* 16-bit UUID */ 
        .uuidFormat = 0x01u, /* UUID Format - 16-bit (0x01) or 128-bit (0x02) */ 
    },
};

static cy_stc_ble_customc_char_t cy_ble_customCChar[0x06u] = {

    /* Custom Characteristic */
    {
        .customServCharHandle    = {CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE}, /* Characteristic handle */ 
        .customServCharEndHandle = {CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE}, /* Characteristic end handle */ 
        .uuid                    = (const cy_stc_ble_uuid128_t *) &cy_ble_customUuid128[1], /* 128-bit UUID */ 
        .uuidFormat              = 0x02u, /* UUID Format - 16-bit (0x01) or 128-bit (0x02) */ 
        .properties              = {0x00u}, /* Characteristic properties */ 
        .descCount               = 0x01u, /* Number of descriptors */ 
        .customServCharDesc      = &cy_ble_customCDesc[0], /* Characteristic Descriptors */ 
    },

    /* BME280_Pressure */
    {
        .customServCharHandle    = {CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE}, /* Characteristic handle */ 
        .customServCharEndHandle = {CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE}, /* Characteristic end handle */ 
        .uuid                    = (const cy_stc_ble_uuid128_t *) &cy_ble_customUuid128[3], /* 128-bit UUID */ 
        .uuidFormat              = 0x02u, /* UUID Format - 16-bit (0x01) or 128-bit (0x02) */ 
        .properties              = {0x00u}, /* Characteristic properties */ 
        .descCount               = 0x01u, /* Number of descriptors */ 
        .customServCharDesc      = &cy_ble_customCDesc[1], /* Characteristic Descriptors */ 
    },

    /* BME280_Humidity */
    {
        .customServCharHandle    = {CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE}, /* Characteristic handle */ 
        .customServCharEndHandle = {CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE}, /* Characteristic end handle */ 
        .uuid                    = (const cy_stc_ble_uuid128_t *) &cy_ble_customUuid128[5], /* 128-bit UUID */ 
        .uuidFormat              = 0x02u, /* UUID Format - 16-bit (0x01) or 128-bit (0x02) */ 
        .properties              = {0x00u}, /* Characteristic properties */ 
        .descCount               = 0x01u, /* Number of descriptors */ 
        .customServCharDesc      = &cy_ble_customCDesc[2], /* Characteristic Descriptors */ 
    },

    /* CCS811_TVOC */
    {
        .customServCharHandle    = {CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE}, /* Characteristic handle */ 
        .customServCharEndHandle = {CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE}, /* Characteristic end handle */ 
        .uuid                    = (const cy_stc_ble_uuid128_t *) &cy_ble_customUuid128[7], /* 128-bit UUID */ 
        .uuidFormat              = 0x02u, /* UUID Format - 16-bit (0x01) or 128-bit (0x02) */ 
        .properties              = {0x00u}, /* Characteristic properties */ 
        .descCount               = 0x01u, /* Number of descriptors */ 
        .customServCharDesc      = &cy_ble_customCDesc[3], /* Characteristic Descriptors */ 
    },

    /* CCS811_ECO2 */
    {
        .customServCharHandle    = {CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE}, /* Characteristic handle */ 
        .customServCharEndHandle = {CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE}, /* Characteristic end handle */ 
        .uuid                    = (const cy_stc_ble_uuid128_t *) &cy_ble_customUuid128[9], /* 128-bit UUID */ 
        .uuidFormat              = 0x02u, /* UUID Format - 16-bit (0x01) or 128-bit (0x02) */ 
        .properties              = {0x00u}, /* Characteristic properties */ 
        .descCount               = 0x01u, /* Number of descriptors */ 
        .customServCharDesc      = &cy_ble_customCDesc[4], /* Characteristic Descriptors */ 
    },

    /* EnvSenors */
    {
        .customServCharHandle    = {CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE}, /* Characteristic handle */ 
        .customServCharEndHandle = {CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE}, /* Characteristic end handle */ 
        .uuid                    = (const cy_stc_ble_uuid128_t *) &cy_ble_customUuid128[11], /* 128-bit UUID */ 
        .uuidFormat              = 0x02u, /* UUID Format - 16-bit (0x01) or 128-bit (0x02) */ 
        .properties              = {0x00u}, /* Characteristic properties */ 
        .descCount               = 0x01u, /* Number of descriptors */ 
        .customServCharDesc      = &cy_ble_customCDesc[5], /* Characteristic Descriptors */ 
    },
};

cy_stc_ble_customc_t cy_ble_customCServ[CY_BLE_CUSTOMC_SERVICE_COUNT] = {

    /* BME280_Temp */
    {
        .customServHandle = {CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE}, /* Custom Service handle */ 
        .uuid             = (const cy_stc_ble_uuid128_t *) &cy_ble_customUuid128[0], /* 128-bit UUID */ 
        .uuidFormat       = 0x02u, /* UUID Format - 16-bit (0x01) or 128-bit (0x02) */ 
        .charCount        = 0x01u, /* Number of characteristics */ 
        .customServChar   = &cy_ble_customCChar[0], /* Custom Service Characteristics */ 
    },

    /* BME280_Pressure */
    {
        .customServHandle = {CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE}, /* Custom Service handle */ 
        .uuid             = (const cy_stc_ble_uuid128_t *) &cy_ble_customUuid128[2], /* 128-bit UUID */ 
        .uuidFormat       = 0x02u, /* UUID Format - 16-bit (0x01) or 128-bit (0x02) */ 
        .charCount        = 0x01u, /* Number of characteristics */ 
        .customServChar   = &cy_ble_customCChar[1], /* Custom Service Characteristics */ 
    },

    /* BME280_Humidity */
    {
        .customServHandle = {CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE}, /* Custom Service handle */ 
        .uuid             = (const cy_stc_ble_uuid128_t *) &cy_ble_customUuid128[4], /* 128-bit UUID */ 
        .uuidFormat       = 0x02u, /* UUID Format - 16-bit (0x01) or 128-bit (0x02) */ 
        .charCount        = 0x01u, /* Number of characteristics */ 
        .customServChar   = &cy_ble_customCChar[2], /* Custom Service Characteristics */ 
    },

    /* CCS811_TVOC */
    {
        .customServHandle = {CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE}, /* Custom Service handle */ 
        .uuid             = (const cy_stc_ble_uuid128_t *) &cy_ble_customUuid128[6], /* 128-bit UUID */ 
        .uuidFormat       = 0x02u, /* UUID Format - 16-bit (0x01) or 128-bit (0x02) */ 
        .charCount        = 0x01u, /* Number of characteristics */ 
        .customServChar   = &cy_ble_customCChar[3], /* Custom Service Characteristics */ 
    },

    /* CCS811_ECO2 */
    {
        .customServHandle = {CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE}, /* Custom Service handle */ 
        .uuid             = (const cy_stc_ble_uuid128_t *) &cy_ble_customUuid128[8], /* 128-bit UUID */ 
        .uuidFormat       = 0x02u, /* UUID Format - 16-bit (0x01) or 128-bit (0x02) */ 
        .charCount        = 0x01u, /* Number of characteristics */ 
        .customServChar   = &cy_ble_customCChar[4], /* Custom Service Characteristics */ 
    },

    /* EnvSensors Service */
    {
        .customServHandle = {CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE}, /* Custom Service handle */ 
        .uuid             = (const cy_stc_ble_uuid128_t *) &cy_ble_customUuid128[10], /* 128-bit UUID */ 
        .uuidFormat       = 0x02u, /* UUID Format - 16-bit (0x01) or 128-bit (0x02) */ 
        .charCount        = 0x01u, /* Number of characteristics */ 
        .customServChar   = &cy_ble_customCChar[5], /* Custom Service Characteristics */ 
    },
};


#endif /* (CY_BLE_CUSTOM_CLIENT) */

/**
* \addtogroup group_globals
* @{
*/

/** The configuration structure for the Custom Services. */
cy_stc_ble_custom_config_t cy_ble_customConfig =
{
    /* Custom Services GATT DB handles structure */
    #ifdef CY_BLE_CUSTOM_SERVER
    .customs      = cy_ble_customs,
    #endif /* (CY_BLE_CUSTOM_SERVER) */

    /* Custom Service handle */
    #ifdef CY_BLE_CUSTOM_CLIENT
    .customc  = cy_ble_customCServ,
    #endif /* (CY_BLE_CUSTOM_CLIENT) */
};

/** @} group_globals */

#endif /* (CY_BLE_MODE_PROFILE && defined(CY_BLE_CUSTOM)) */

/* [] END OF FILE */
