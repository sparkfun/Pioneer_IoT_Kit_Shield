/***************************************************************************//**
* \file CY_BLE.c
* \version 2.0
* 
* \brief
*  This file contains the source code for the Common APIs of the BLE Component.
* 
********************************************************************************
* \copyright
* Copyright 2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "BLE_config.h"

#if CY_BLE_MODE_PROFILE
#include "flash/cy_flash.h"
#include "ble/cy_ble_event_handler.h"

    
/***************************************
* Global Variables
***************************************/
/* Initializes the cy_stc_ble_gapp_disc_mode_info_t cy_ble_discoveryModeInfo  structure */
#if(CY_BLE_GAP_ROLE_PERIPHERAL || CY_BLE_GAP_ROLE_BROADCASTER)
static cy_stc_ble_gapp_adv_params_t cy_ble_gappAdvConfig[0x01u] = {

    /* Peripheral configuration 0 */
    {
        .fastAdvIntervalMin                 = 0x0120u, 
        .fastAdvIntervalMax                 = 0x0140u, 
        .fastAdvTimeOut                     = 0x0000u, 
        .slowAdvEnable                      = 0x00u, 
        .slowAdvIntervalMin                 = 0x0640u, 
        .slowAdvIntervalMax                 = 0x4000u, 
        .slowAdvTimeOut                     = 0x0096u, 
    },
};


cy_stc_ble_gapp_disc_param_t cy_ble_discoveryParam[0x01u] = {

    /* Peripheral configuration 0 */
    {
        0x0120u, /* uint16_t advertising_interval_min */ 
        0x0140u, /* uint16_t advertising_interval_max */ 
        CY_BLE_GAPP_CONNECTABLE_UNDIRECTED_ADV, /* uint8_t advertising_type */ 
        0x00u, /* uint8_t own_addr_type */ 
        0x00u, /* uint8_t direct_addr_type */ 
        {0x34u, 0x81u, 0x21u, 0x50u, 0xA0u, 0x00u}, /* uint8_t* direct_addr */ 
        0x07u, /* uint8_t advertising_channel_map */ 
        0x00u, /* uint8_t advertising_filter_policy */ 
    },
};


cy_stc_ble_gapp_disc_data_t cy_ble_discoveryData[0x01u] = {

    /* Peripheral configuration 0 */
    {
        { 0x02u, 0x01u, 0x06u, 0x13u, 0x09u, 0x50u, 0x53u,
        0x6Fu, 0x43u, 0x36u, 0x20u, 0x43u, 0x61u, 0x70u,
        0x53u, 0x65u, 0x6Eu, 0x73u, 0x65u, 0x20u, 0x4Cu,
        0x45u, 0x44u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u }, /* uint8_t advertising_data[CY_BLE_MAX_ADV_DATA_LEN] */ 
        0x17u, /* uint8_t adv_data_length */ 
    },
};


cy_stc_ble_gapp_scan_rsp_data_t cy_ble_scanRspData[0x01u] = {

    /* Peripheral configuration 0 */
    {
        { 0x02u, 0x0Au, 0x00u, 0x03u, 0x19u, 0x00u, 0x02u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
        0x00u, 0x00u, 0x00u }, /* uint8_t scan_rsp_data[CY_BLE_MAX_SCAN_RSP_DATA_LEN] */ 
        0x07u, /* uint8_t scan_rsp_data_length */ 
    },
};


/* This array of type cy_stc_ble_gapp_disc_mode_info_t is present only when the 
   BLE component is configured for Peripheral or Broadcaster GAP roles. 
   It contains the Advertisement settings and also the Advertisement and 
   Scan response data parameters entered in the customizer. This variable
   can be used by advanced users to change Advertisement settings,
   Advertisement or Scan response data in runtime. 
*/
cy_stc_ble_gapp_disc_mode_info_t cy_ble_discoveryModeInfo[0x01u] = {

    /* Peripheral configuration 0 */
    {
        0x02u, /* uint8_t discMode */ 
        &cy_ble_discoveryParam[0], 
        &cy_ble_discoveryData[0], 
        &cy_ble_scanRspData[0], 
        0x0000u, /* uint16_t advTo */ 
    },
};


#endif /* CY_BLE_GAP_ROLE_PERIPHERAL || CY_BLE_GAP_ROLE_BROADCASTER */

/* Initializes the cy_stc_ble_gapc_disc_info_t  cy_ble_discoveryInfo  structure */
#if(CY_BLE_GAP_ROLE_CENTRAL || CY_BLE_GAP_ROLE_OBSERVER)

#endif /* CY_BLE_GAP_ROLE_CENTRAL || CY_BLE_GAP_ROLE_OBSERVER */

#if ((CY_BLE_MODE_PROFILE) && (CY_BLE_BONDING_REQUIREMENT == CY_BLE_BONDING_YES))

#if(CY_BLE_MODE_PROFILE)
    CY_SECTION(".cy_wflash") CY_ALIGN(CY_FLASH_SIZEOF_ROW) static const cy_stc_ble_flash_storage_t cy_ble_flashStorage =
    {
        { 0x00u }, 
        {{
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u /* CRC */
        },
        {
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u /* CRC */
        },
        {
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u /* CRC */
        },
        {
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u /* CRC */
        },
        {
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u /* CRC */
        },
        {
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u /* CRC */
        },
        {
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u /* CRC */
        },
        {
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u /* CRC */
        },
        {
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u /* CRC */
        },
        {
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u /* CRC */
        },
        {
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u /* CRC */
        },
        {
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u /* CRC */
        },
        {
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u /* CRC */
        },
        {
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u /* CRC */
        },
        {
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u /* CRC */
        },
        {
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u /* CRC */
        },
        {
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u /* CRC */
        },
        {
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u /* CRC */
        },
        {
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u /* CRC */
        },
        {
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u, 0x00u, 
            0x00u /* CRC */
        }}, 
        0x08u, /* CY_BLE_GATT_DB_CCCD_COUNT */ 
        0x14u, 
    };
#endif /* (CY_BLE_MODE_PROFILE) */

#endif  /* (CY_BLE_MODE_PROFILE) && (CY_BLE_BONDING_REQUIREMENT == CY_BLE_BONDING_YES) */

#if(CY_BLE_GATT_ROLE_SERVER)
static const cy_stc_ble_gatts_t cy_ble_gatts =
{
    0x000Cu,    /* Handle of the GATT service */
    0x000Eu,    /* Handle of the Service Changed characteristic */
    0x000Fu,    /* Handle of the Client Characteristic Configuration descriptor */
};
static const cy_stc_ble_gaps_t cy_ble_gaps =
{
    0x0001u,    /* Handle of the GAP service */
    0x0003u,    /* Handle of the Device Name characteristic */
    0x0005u,    /* Handle of the Appearance characteristic */
    0x0007u,    /* Handle of the Peripheral Preferred Connection Parameters characteristic */
    0x0009u,    /* Handle of the Central Address Resolution characteristic */
    0x000Bu,    /* Handle of the Resolvable Private Address Only characteristic */
};
static uint8_t cy_ble_attValues[0x57u] = {
    /* Device Name */
    (uint8_t)'P', (uint8_t)'S', (uint8_t)'o', (uint8_t)'C', (uint8_t)'6', (uint8_t)' ', (uint8_t)'C', (uint8_t)'a',
(uint8_t)'p', (uint8_t)'S', (uint8_t)'e', (uint8_t)'n', (uint8_t)'s', (uint8_t)'e', (uint8_t)' ', (uint8_t)'L',
(uint8_t)'E', (uint8_t)'D', 

    /* Appearance */
    0x00u, 0x02u, 

    /* Peripheral Preferred Connection Parameters */
    0x3Cu, 0x00u, 0x50u, 0x00u, 0x00u, 0x00u, 0xC8u, 0x00u, 

    /* Central Address Resolution */
    0x00u, 

    /* Resolvable Private Address Only */
    0x00u, 

    /* Service Changed */
    0x00u, 0x00u, 0x00u, 0x00u, 

    /* CapSense Slider */
    0x00u, 

    /* Characteristic User Description */
    (uint8_t)'C', (uint8_t)'a', (uint8_t)'p', (uint8_t)'S', (uint8_t)'e', (uint8_t)'n', (uint8_t)'s', (uint8_t)'e',
(uint8_t)' ', (uint8_t)'S', (uint8_t)'l', (uint8_t)'i', (uint8_t)'d', (uint8_t)'e', (uint8_t)'r', 

    /* RGB LED Control */
    0x00u, 0x00u, 0x00u, 0x00u, 

    /* Characteristic User Description */
    (uint8_t)'R', (uint8_t)'G', (uint8_t)'B', (uint8_t)' ', (uint8_t)'L', (uint8_t)'E', (uint8_t)'D', (uint8_t)' ',
(uint8_t)'C', (uint8_t)'o', (uint8_t)'n', (uint8_t)'t', (uint8_t)'r', (uint8_t)'o', (uint8_t)'l', 

    /* CapSense Button */
    0x02u, 0x00u, 0x00u, 

    /* Characteristic User Description */
    (uint8_t)'C', (uint8_t)'a', (uint8_t)'p', (uint8_t)'S', (uint8_t)'e', (uint8_t)'n', (uint8_t)'s', (uint8_t)'e',
(uint8_t)' ', (uint8_t)'B', (uint8_t)'u', (uint8_t)'t', (uint8_t)'t', (uint8_t)'o', (uint8_t)'n', 

};
#if(CY_BLE_GATT_DB_CCCD_COUNT != 0u)
static uint8_t cy_ble_attValuesCCCD[CY_BLE_GATT_DB_CCCD_COUNT];
#endif /* CY_BLE_GATT_DB_CCCD_COUNT != 0u */

static const uint8_t cy_ble_attUuid128[][16u] = {
    /* CapSense Slider */
    { 0x3Eu, 0x0Du, 0x83u, 0xF5u, 0xEBu, 0xAEu, 0x77u, 0xB5u, 0xD4u, 0x49u, 0x27u, 0xA3u, 0x84u, 0x68u, 0xC6u, 0x53u },
    /* CapSense Slider */
    { 0x90u, 0xE4u, 0x56u, 0x90u, 0x34u, 0x5Du, 0xBAu, 0x8Au, 0x40u, 0x4Du, 0x99u, 0x65u, 0x76u, 0xACu, 0x73u, 0xF9u },
    /* RGB LED */
    { 0xB8u, 0x50u, 0xA6u, 0xFAu, 0x86u, 0xC2u, 0x49u, 0xB3u, 0x38u, 0x49u, 0xB9u, 0xECu, 0x79u, 0xC4u, 0x28u, 0x77u },
    /* RGB LED Control */
    { 0x25u, 0xC8u, 0x60u, 0x25u, 0xD3u, 0x66u, 0x39u, 0x94u, 0x39u, 0x4Cu, 0x5Fu, 0x7Eu, 0x04u, 0x0Bu, 0x7Bu, 0xE7u },
    /* CapSense Button */
    { 0x8Bu, 0xE0u, 0x99u, 0x14u, 0x04u, 0x13u, 0x72u, 0x96u, 0x77u, 0x4Du, 0x2Au, 0xDEu, 0xE8u, 0x35u, 0x77u, 0xDFu },
    /* CapSense Button */
    { 0x31u, 0x01u, 0x9Bu, 0x5Fu, 0x80u, 0x00u, 0x00u, 0x80u, 0x00u, 0x10u, 0x00u, 0x00u, 0xA3u, 0xCAu, 0x03u, 0x00u },
};

static cy_stc_ble_gatts_att_gen_val_len_t cy_ble_attValuesLen[0x16u] = {
    { 0x0012u, (void *)&cy_ble_attValues[0] }, /* Device Name */
    { 0x0002u, (void *)&cy_ble_attValues[18] }, /* Appearance */
    { 0x0008u, (void *)&cy_ble_attValues[20] }, /* Peripheral Preferred Connection Parameters */
    { 0x0001u, (void *)&cy_ble_attValues[28] }, /* Central Address Resolution */
    { 0x0001u, (void *)&cy_ble_attValues[29] }, /* Resolvable Private Address Only */
    { 0x0004u, (void *)&cy_ble_attValues[30] }, /* Service Changed */
    { 0x0002u, (void *)&cy_ble_attValuesCCCD[0] }, /* Client Characteristic Configuration */
    { 0x0010u, (void *)&cy_ble_attUuid128[0] }, /* CapSense Slider UUID */
    { 0x0010u, (void *)&cy_ble_attUuid128[1] }, /* CapSense Slider UUID */
    { 0x0001u, (void *)&cy_ble_attValues[34] }, /* CapSense Slider */
    { 0x0002u, (void *)&cy_ble_attValuesCCCD[2] }, /* Client Characteristic Configuration */
    { 0x000Fu, (void *)&cy_ble_attValues[35] }, /* Characteristic User Description */
    { 0x0010u, (void *)&cy_ble_attUuid128[2] }, /* RGB LED UUID */
    { 0x0010u, (void *)&cy_ble_attUuid128[3] }, /* RGB LED Control UUID */
    { 0x0004u, (void *)&cy_ble_attValues[50] }, /* RGB LED Control */
    { 0x0002u, (void *)&cy_ble_attValuesCCCD[4] }, /* Client Characteristic Configuration */
    { 0x000Fu, (void *)&cy_ble_attValues[54] }, /* Characteristic User Description */
    { 0x0010u, (void *)&cy_ble_attUuid128[4] }, /* CapSense Button UUID */
    { 0x0010u, (void *)&cy_ble_attUuid128[5] }, /* CapSense Button UUID */
    { 0x0003u, (void *)&cy_ble_attValues[69] }, /* CapSense Button */
    { 0x0002u, (void *)&cy_ble_attValuesCCCD[6] }, /* Client Characteristic Configuration */
    { 0x000Fu, (void *)&cy_ble_attValues[72] }, /* Characteristic User Description */
};

static const cy_stc_ble_gatts_db_t cy_ble_gattDB[0x1Eu] = {
    { 0x0001u, 0x2800u /* Primary service                     */, 0x00000001u /*           */, 0x000Bu, {{0x1800u, NULL}}                           },
    { 0x0002u, 0x2803u /* Characteristic                      */, 0x00020001u /* rd        */, 0x0003u, {{0x2A00u, NULL}}                           },
    { 0x0003u, 0x2A00u /* Device Name                         */, 0x01020001u /* rd        */, 0x0003u, {{0x0012u, (void *)&cy_ble_attValuesLen[0]}} },
    { 0x0004u, 0x2803u /* Characteristic                      */, 0x00020001u /* rd        */, 0x0005u, {{0x2A01u, NULL}}                           },
    { 0x0005u, 0x2A01u /* Appearance                          */, 0x01020001u /* rd        */, 0x0005u, {{0x0002u, (void *)&cy_ble_attValuesLen[1]}} },
    { 0x0006u, 0x2803u /* Characteristic                      */, 0x00020001u /* rd        */, 0x0007u, {{0x2A04u, NULL}}                           },
    { 0x0007u, 0x2A04u /* Peripheral Preferred Connection Par */, 0x01020001u /* rd        */, 0x0007u, {{0x0008u, (void *)&cy_ble_attValuesLen[2]}} },
    { 0x0008u, 0x2803u /* Characteristic                      */, 0x00020001u /* rd        */, 0x0009u, {{0x2AA6u, NULL}}                           },
    { 0x0009u, 0x2AA6u /* Central Address Resolution          */, 0x01020001u /* rd        */, 0x0009u, {{0x0001u, (void *)&cy_ble_attValuesLen[3]}} },
    { 0x000Au, 0x2803u /* Characteristic                      */, 0x00020001u /* rd        */, 0x000Bu, {{0x2AC9u, NULL}}                           },
    { 0x000Bu, 0x2AC9u /* Resolvable Private Address Only     */, 0x01020001u /* rd        */, 0x000Bu, {{0x0001u, (void *)&cy_ble_attValuesLen[4]}} },
    { 0x000Cu, 0x2800u /* Primary service                     */, 0x00000001u /*           */, 0x000Fu, {{0x1801u, NULL}}                           },
    { 0x000Du, 0x2803u /* Characteristic                      */, 0x00200001u /* ind       */, 0x000Fu, {{0x2A05u, NULL}}                           },
    { 0x000Eu, 0x2A05u /* Service Changed                     */, 0x01200000u /* ind       */, 0x000Fu, {{0x0004u, (void *)&cy_ble_attValuesLen[5]}} },
    { 0x000Fu, 0x2902u /* Client Characteristic Configuration */, 0x030A0101u /* rd,wr     */, 0x000Fu, {{0x0002u, (void *)&cy_ble_attValuesLen[6]}} },
    { 0x0010u, 0x2800u /* Primary service                     */, 0x08000001u /*           */, 0x0014u, {{0x0010u, (void *)&cy_ble_attValuesLen[7]}} },
    { 0x0011u, 0x2803u /* Characteristic                      */, 0x00100001u /* ntf       */, 0x0014u, {{0x0010u, (void *)&cy_ble_attValuesLen[8]}} },
    { 0x0012u, 0xAC76u /* CapSense Slider                     */, 0x09100000u /* ntf       */, 0x0014u, {{0x0001u, (void *)&cy_ble_attValuesLen[9]}} },
    { 0x0013u, 0x2902u /* Client Characteristic Configuration */, 0x030A0101u /* rd,wr     */, 0x0013u, {{0x0002u, (void *)&cy_ble_attValuesLen[10]}} },
    { 0x0014u, 0x2901u /* Characteristic User Description     */, 0x01020001u /* rd        */, 0x0014u, {{0x000Fu, (void *)&cy_ble_attValuesLen[11]}} },
    { 0x0015u, 0x2800u /* Primary service                     */, 0x08000001u /*           */, 0x0019u, {{0x0010u, (void *)&cy_ble_attValuesLen[12]}} },
    { 0x0016u, 0x2803u /* Characteristic                      */, 0x001A0001u /* rd,wr,ntf */, 0x0019u, {{0x0010u, (void *)&cy_ble_attValuesLen[13]}} },
    { 0x0017u, 0x0B04u /* RGB LED Control                     */, 0x091A0101u /* rd,wr,ntf */, 0x0019u, {{0x0004u, (void *)&cy_ble_attValuesLen[14]}} },
    { 0x0018u, 0x2902u /* Client Characteristic Configuration */, 0x030A0101u /* rd,wr     */, 0x0018u, {{0x0002u, (void *)&cy_ble_attValuesLen[15]}} },
    { 0x0019u, 0x2901u /* Characteristic User Description     */, 0x01020001u /* rd        */, 0x0019u, {{0x000Fu, (void *)&cy_ble_attValuesLen[16]}} },
    { 0x001Au, 0x2800u /* Primary service                     */, 0x08000001u /*           */, 0x001Eu, {{0x0010u, (void *)&cy_ble_attValuesLen[17]}} },
    { 0x001Bu, 0x2803u /* Characteristic                      */, 0x00100001u /* ntf       */, 0x001Eu, {{0x0010u, (void *)&cy_ble_attValuesLen[18]}} },
    { 0x001Cu, 0xCAA3u /* CapSense Button                     */, 0x09100000u /* ntf       */, 0x001Eu, {{0x0003u, (void *)&cy_ble_attValuesLen[19]}} },
    { 0x001Du, 0x2902u /* Client Characteristic Configuration */, 0x030A0101u /* rd,wr     */, 0x001Du, {{0x0002u, (void *)&cy_ble_attValuesLen[20]}} },
    { 0x001Eu, 0x2901u /* Characteristic User Description     */, 0x01020001u /* rd        */, 0x001Eu, {{0x000Fu, (void *)&cy_ble_attValuesLen[21]}} },
};

#endif /* (CY_BLE_GATT_ROLE_SERVER) */

/* Default device security */
#if (CY_BLE_MODE_PROFILE) 
    cy_stc_ble_gap_auth_info_t cy_ble_authInfo[0x01u] = {

    /* Security configuration 0 */
    {
        .security = (CY_BLE_GAP_SEC_MODE_1 | CY_BLE_GAP_SEC_LEVEL_1 ), 
        .bonding = CY_BLE_GAP_BONDING, 
        .ekeySize = 0x10u, 
        .authErr = CY_BLE_GAP_AUTH_ERROR_NONE, 
        .pairingProperties = 0x01u, 
    },
};

#endif /* CY_BLE_MODE_PROFILE */

#if(CY_BLE_GAP_ROLE_CENTRAL || CY_BLE_GAP_ROLE_PERIPHERAL)

/** Initialize BLE configuration parameters structure */
static const cy_stc_ble_params_t cy_ble_params =
{
        .txPowerLevelAdv                    = CY_BLE_LL_PWR_LVL_0_DBM,
        .txPowerLevelConn                   = CY_BLE_LL_PWR_LVL_0_DBM,

        .securityIoCapability               = CY_BLE_GAP_IOCAP_DISPLAY_ONLY,
        .securityPairingMethod              = 0x00u,
    
        .siliconDeviceAddressEnabled        = 0x00u,
    
        .gattDbIndexCount                   = 0x001Eu,
};
#endif  /* (CY_BLE_GAP_ROLE_CENTRAL || CY_BLE_GAP_ROLE_PERIPHERAL) */

cy_stc_ble_gap_bd_addr_t cy_ble_deviceAddress = {{0x34u, 0x81u, 0x21u, 0x50u, 0xA0u, 0x00u}, 0x00u };

/**
* \addtogroup group_globals
* @{
*/

/** The configuration structure for BLE */
cy_stc_ble_config_t cy_ble_config =
{
    /* Initialize the GAPP structures */
    /* Initialize the cy_stc_ble_gapp_disc_mode_info_t cy_ble_discoveryModeInfo structure */
    #if(CY_BLE_GAP_ROLE_PERIPHERAL || CY_BLE_GAP_ROLE_BROADCASTER)
        .discoveryModeInfo = cy_ble_discoveryModeInfo,
        .gappAdvParams = cy_ble_gappAdvConfig,
    #else
        .discoveryModeInfo = NULL,
    #endif /* CY_BLE_GAP_ROLE_PERIPHERAL || CY_BLE_GAP_ROLE_BROADCASTER */

    /* Initialize the cy_stc_ble_gapc_disc_info_t  cy_ble_discoveryInfo  structure */
    #if(CY_BLE_GAP_ROLE_CENTRAL || CY_BLE_GAP_ROLE_OBSERVER)
        .discoveryInfo = cy_ble_discoveryInfo,
        .gapcScanParams = cy_ble_gapcScanConfig,
    #else
        .discoveryInfo = NULL,
    #endif /* CY_BLE_GAP_ROLE_CENTRAL || CY_BLE_GAP_ROLE_OBSERVER */

    /* Initialize the GATT structures */
    #if((CY_BLE_GAP_ROLE_CENTRAL || CY_BLE_GAP_ROLE_PERIPHERAL) && (CY_BLE_BONDING_REQUIREMENT == CY_BLE_BONDING_YES))
        .flashStorage = &cy_ble_flashStorage,
    #else
        .flashStorage = NULL,
    #endif /* CY_BLE_GAP_ROLE_CENTRAL || CY_BLE_GAP_ROLE_PERIPHERAL) && (CY_BLE_BONDING_REQUIREMENT == CY_BLE_BONDING_YES */

    #if(CY_BLE_GATT_ROLE_SERVER)
        .gatts            = &cy_ble_gatts,
        .gaps             = &cy_ble_gaps,
        .attValuesCCCD    = cy_ble_attValuesCCCD,
        .gattDB           = cy_ble_gattDB,
    #else
        .gatts            = NULL,
        .attValuesCCCD    = NULL,
        .gattDB           = NULL,
    #endif /* CY_BLE_GATT_ROLE_SERVER */

    #if(CY_BLE_GAP_ROLE_CENTRAL || CY_BLE_GAP_ROLE_PERIPHERAL)
        /* Initialize the device security structure */
        .authInfo = cy_ble_authInfo,
    #else
        .authInfo = NULL,
    #endif /* (CY_BLE_GAP_ROLE_CENTRAL || CY_BLE_GAP_ROLE_PERIPHERAL */

    #if(CY_BLE_GAP_ROLE_CENTRAL || CY_BLE_GAP_ROLE_PERIPHERAL)
        /* Initialize the BLE configuration parameters structure */
        .params   = &cy_ble_params,
    #else
        .params = NULL,
    #endif /* (CY_BLE_GAP_ROLE_CENTRAL || CY_BLE_GAP_ROLE_PERIPHERAL */

    /* An application layer event callback function to receive service events from the BLE Component. */
    .callbackFunc   = NULL,

    .deviceAddress  = &cy_ble_deviceAddress,
};

/** @} group_globals */

#endif /* CY_BLE_MODE_PROFILE */

/* [] END OF FILE */
