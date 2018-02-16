/*******************************************************************************
* File Name: host_main.c
*
* Version: 1.0
*
* Description:
* This project continuously scans for up to four devices that have environmental
* sensors.  It connects to these devices and displays the information via a
* serial port.
*
* Note:
*
* Hardware Dependency:
*  CY8CKIT-062 PSoC6 BLE Pioneer Kit
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

#include "common.h"
#include "user_interface.h"
#include "wifi.h"

static app_stc_sensor_data_t        sensorData[CY_BLE_MAX_CENTRAL_CONN_NUM];

static app_stc_connection_info_t    appConnInfo;
static app_stc_scan_device_list_t   appScanDevInfo;

static volatile uint32_t            mainTimer           = 1u;
static volatile uint32_t            systemClock         = 0u;

static bool                         enableNotification  = false;
static volatile bool                writeRsp            = false;


cy_stc_ble_gattc_write_req_t writeReqParam = { .handleValPair.attrHandle = 0u};
/* Private Function Prototypes */
static uint32_t CheckAdvPacketForServiceUuid128(cy_stc_ble_gapc_adv_report_param_t *eventParam, void * uuid128);
static void EnableNotifications(void);
static void DisplayData(app_stc_sensor_data_t * sensorData);
static void DisplayData2(app_stc_sensor_data_t * sensorData);

/*******************************************************************************
* Function Name: App_IsDeviceInBondList()
********************************************************************************
*
* Summary:
*  This function check if device with bdHandle is in the bond list
*
* Parameters:
*   bdHandle - bond device handler
*
* Return:
*   bool - true value when bdHandle exists in bond list
*
*******************************************************************************/
bool App_IsDeviceInBondList(uint32_t bdHandle)
{
    cy_en_ble_api_result_t apiResult;
    cy_stc_ble_gap_peer_addr_info_t bondedDeviceInfo[CY_BLE_MAX_BONDED_DEVICES];
    cy_stc_ble_gap_bonded_device_list_info_t bondedDeviceList =
    {
        .bdHandleAddrList = bondedDeviceInfo
    };
    bool deviceIsDetected = false;
    uint32_t deviceCount;
    
    /* Find out whether the device has bonding information stored already or not */
    apiResult = Cy_BLE_GAP_GetBondList(&bondedDeviceList);
    if(apiResult != CY_BLE_SUCCESS)
    {
        DBG_PRINTF("Cy_BLE_GAP_GetBondedDevicesList API Error: 0x%x \r\n", apiResult);
    }
    else
    {
        deviceCount = bondedDeviceList.noOfDevices;

        if(deviceCount != 0u)
        {
            do
            {
                deviceCount--;
                if(bdHandle == bondedDeviceList.bdHandleAddrList[deviceCount].bdHandle)
                {
                    deviceIsDetected = 1u;
                }
            } while(deviceCount != 0u);
        }
    }
    return(deviceIsDetected);
}

/*******************************************************************************
* Function Name: AppCallBack
********************************************************************************
*
* Summary:
*   This is an event callback function to receive events from the BLE Component.
*
* Parameters:
*  event      - The event code.
*  eventParam - The event parameters.
*
*
*******************************************************************************/
void AppCallBack(uint32_t event, void* eventParam)
{
    static cy_stc_ble_gap_sec_key_info_t keyInfo =
    {
        .localKeysFlag    = CY_BLE_GAP_SMP_INIT_ENC_KEY_DIST | 
                            CY_BLE_GAP_SMP_INIT_IRK_KEY_DIST | 
                            CY_BLE_GAP_SMP_INIT_CSRK_KEY_DIST,
        .exchangeKeysFlag = CY_BLE_GAP_SMP_INIT_ENC_KEY_DIST | 
                            CY_BLE_GAP_SMP_INIT_IRK_KEY_DIST | 
                            CY_BLE_GAP_SMP_INIT_CSRK_KEY_DIST |
                            CY_BLE_GAP_SMP_RESP_ENC_KEY_DIST |
                            CY_BLE_GAP_SMP_RESP_IRK_KEY_DIST |
                            CY_BLE_GAP_SMP_RESP_CSRK_KEY_DIST,
    };
    
    cy_en_ble_api_result_t          apiResult;  
    uint32_t                        i;

    switch (event)
    {
        /**********************************************************
        *                       General Events
        ***********************************************************/
        case CY_BLE_EVT_STACK_ON: /* This event is received when the component is started */
            DBG_PRINTF("CY_BLE_EVT_STACK_ON \r\n");
            
            /* Generates the security keys */
            apiResult = Cy_BLE_GAP_GenerateKeys(&keyInfo);
            if(apiResult != CY_BLE_SUCCESS)
            {
                DBG_PRINTF("Cy_BLE_GAP_GenerateKeys API Error: 0x%x \r\n", apiResult);
            }
            
            /* Print the operation menu */
            PrintMenu();          
            break;
            
       case CY_BLE_EVT_GAP_KEYS_GEN_COMPLETE:
            DBG_PRINTF("CY_BLE_EVT_GAP_KEYS_GEN_COMPLETE \r\n");
            keyInfo.SecKeyParam = (*(cy_stc_ble_gap_sec_key_param_t *)eventParam);
            Cy_BLE_GAP_SetIdAddress(&cy_ble_deviceAddress);
            break;
            
        case CY_BLE_EVT_TIMEOUT:
            DBG_PRINTF("CY_BLE_EVT_TIMEOUT \r\n");
            break;
            
        case CY_BLE_EVT_HARDWARE_ERROR:    /* This event indicates that an internal HW error occurred. */
            DBG_PRINTF("Hardware Error \r\n");
            ShowError();
            break;
        
        /* This event will be triggered by the host stack in both cases: the BLE stack is busy or not busy.
         *  The parameter that corresponds to this event will be the state of the BLE stack.
         *  The BLE stack is busy = CY_BLE_STACK_STATE_BUSY,
         *  The BLE stack is not busy = CY_BLE_STACK_STATE_FREE.
         */
        case CY_BLE_EVT_STACK_BUSY_STATUS:
            DBG_PRINTF("CY_BLE_EVT_STACK_BUSY_STATUS: %x\r\n", *(uint8_t *)eventParam);
            break;
            
        case CY_BLE_EVT_SET_TX_PWR_COMPLETE:
            DBG_PRINTF("CY_BLE_EVT_SET_TX_PWR_COMPLETE \r\n");
            break;
            
        case CY_BLE_EVT_LE_SET_EVENT_MASK_COMPLETE:
            DBG_PRINTF("CY_BLE_EVT_LE_SET_EVENT_MASK_COMPLETE \r\n");
            break;
            
        case CY_BLE_EVT_SET_DEVICE_ADDR_COMPLETE:
            DBG_PRINTF("CY_BLE_EVT_SET_DEVICE_ADDR_COMPLETE \r\n");
            
            /* Reads the BD device address from BLE Controller's memory */
            apiResult = Cy_BLE_GAP_GetBdAddress();
            if(apiResult != CY_BLE_SUCCESS)
            {   
                DBG_PRINTF("Cy_BLE_GAP_GetBdAddress API Error: 0x%x \r\n", apiResult);
            }
            break;
            
        case CY_BLE_EVT_GET_DEVICE_ADDR_COMPLETE:
            DBG_PRINTF("CY_BLE_EVT_GET_DEVICE_ADDR_COMPLETE: ");
            for(i = CY_BLE_GAP_BD_ADDR_SIZE; i > 0u; i--)
            {
                DBG_PRINTF("%2.2x", ((cy_stc_ble_bd_addrs_t *)
                                    ((cy_stc_ble_events_param_generic_t *)eventParam)->eventParams)->publicBdAddr[i-1]);
            }
            DBG_PRINTF("\r\n");
            break;
            
        case CY_BLE_EVT_STACK_SHUTDOWN_COMPLETE:
            DBG_PRINTF("CY_BLE_EVT_STACK_SHUTDOWN_COMPLETE \r\n");
            DBG_PRINTF("Hibernate\r\n");
            UART_DEB_WAIT_TX_COMPLETE();
            /* Hibernate */
            Cy_SysPm_Hibernate();   
            break;
            
        /**********************************************************
        *                       GAP Events
        ***********************************************************/
        case CY_BLE_EVT_GAP_AUTH_REQ:
            /* This event is received by Peripheral and Central devices. When it is received by a peripheral, 
             * that peripheral must Call Cy_BLE_GAPP_AuthReqReply() to reply to the authentication request
             * from Central. */
            DBG_PRINTF("CY_BLE_EVT_GAP_AUTH_REQ: bdHandle=%x, security=%x, bonding=%x, ekeySize=%x, err=%x \r\n", 
                (*(cy_stc_ble_gap_auth_info_t *)eventParam).bdHandle, (*(cy_stc_ble_gap_auth_info_t *)eventParam).security, 
                (*(cy_stc_ble_gap_auth_info_t *)eventParam).bonding, (*(cy_stc_ble_gap_auth_info_t *)eventParam).ekeySize, 
                (*(cy_stc_ble_gap_auth_info_t *)eventParam).authErr);
            
            if(cy_ble_configPtr->authInfo[CY_BLE_SECURITY_CONFIGURATION_0_INDEX].security == 
                (CY_BLE_GAP_SEC_MODE_1 | CY_BLE_GAP_SEC_LEVEL_1))
            {
                cy_ble_configPtr->authInfo[CY_BLE_SECURITY_CONFIGURATION_0_INDEX].authErr = 
                    CY_BLE_GAP_AUTH_ERROR_PAIRING_NOT_SUPPORTED;
            }    
            
            cy_ble_configPtr->authInfo[CY_BLE_SECURITY_CONFIGURATION_0_INDEX].bdHandle = 
                ((cy_stc_ble_gap_auth_info_t *)eventParam)->bdHandle;

            /* Pass security information for authentication in reply to an authentication request 
             * from the master device */
            apiResult = Cy_BLE_GAPP_AuthReqReply(&cy_ble_configPtr->authInfo[CY_BLE_SECURITY_CONFIGURATION_0_INDEX]);            
            if(apiResult != CY_BLE_SUCCESS)
            {
                Cy_BLE_GAP_RemoveOldestDeviceFromBondedList();
                apiResult = Cy_BLE_GAPP_AuthReqReply(&cy_ble_configPtr->authInfo[CY_BLE_SECURITY_CONFIGURATION_0_INDEX]);            
                if(apiResult != CY_BLE_SUCCESS)
                {
                    DBG_PRINTF("Cy_BLE_GAPP_AuthReqReply API Error: 0x%x \r\n", apiResult);
                }
            }
            break;  
        
        case CY_BLE_EVT_GAPC_SCAN_START_STOP:
            DBG_PRINTF("CY_BLE_EVT_GAPC_SCAN_START_STOP\r\n");
            if(Cy_BLE_GetScanState() == CY_BLE_SCAN_STATE_STOPPED)
            {
                DBG_PRINTF("Scan complete! \r\n \r\n");
                
            }
            else
            {
                DBG_PRINTF("GAPC_START_SCANNING\r\n");
            }
            break;
        
        case CY_BLE_EVT_GAP_DEVICE_CONNECTED:
        {
            DBG_PRINTF("CY_BLE_EVT_GAP_DEVICE_CONNECTED: status = %x,  connIntv = %d ms \r\n",
                        ((cy_stc_ble_gap_connected_param_t *)eventParam)->status,
                        ((cy_stc_ble_gap_connected_param_t *)eventParam)->connIntv * 5u /4u); /* in milliseconds / 1.25ms */

            if(((cy_stc_ble_gap_connected_param_t *)eventParam)->status == 0u)
            {
              
    #if (CY_BLE_CONFIG_GAP_SECURITY_LEVEL != 0x0u)                  
    
                /* Set security keys for new device which is not already bonded */
            #if(CY_BLE_BONDING_REQUIREMENT == CY_BLE_BONDING_YES)
                if(App_IsDeviceInBondList((*(cy_stc_ble_gap_connected_param_t *)eventParam).bdHandle) == 0u)
            #endif /* (CY_BLE_BONDING_REQUIREMENT == CY_BLE_BONDING_YES) */
                {
                    keyInfo.SecKeyParam.bdHandle = (*(cy_stc_ble_gap_connected_param_t *)eventParam).bdHandle;
                    apiResult = Cy_BLE_GAP_SetSecurityKeys(&keyInfo);
                    if(apiResult != CY_BLE_SUCCESS)
                    {
                        DBG_PRINTF("Cy_BLE_GAP_SetSecurityKeys API Error: 0x%x \r\n", apiResult);
                    }
                }  
                
                /* Check if connected as Central or Peripheral */
                if(((cy_stc_ble_gap_connected_param_t *)eventParam)->role == CY_BLE_GAP_LL_ROLE_MASTER)
                {
                    /* Connected as Central (Master role) */    
                    DBG_PRINTF("Connected as Central (master role) \r\n");
                
                    /* Send an authorization request */
                    cy_ble_authInfo[CY_BLE_SECURITY_CONFIGURATION_0_INDEX].bdHandle = 
                        ((cy_stc_ble_gap_connected_param_t *)eventParam)->bdHandle;
                        
                    apiResult = Cy_BLE_GAP_AuthReq(&cy_ble_authInfo[CY_BLE_SECURITY_CONFIGURATION_0_INDEX]);
                    if(apiResult != CY_BLE_SUCCESS)
                    {
                        DBG_PRINTF("Cy_BLE_GAP_AuthReq API Error: 0x%x \r\n", apiResult);
                    }
                }
                
    #else /* No Security */
            /* Start Discovery */          
            DBG_PRINTF("Cy_BLE_GATTC_StartDiscovery \r\n");
            apiResult = Cy_BLE_GATTC_StartDiscovery(
                        Cy_BLE_GetConnHandleByBdHandle(((cy_stc_ble_gap_connected_param_t *)eventParam)->bdHandle));
            if(apiResult != CY_BLE_SUCCESS)
            {
                DBG_PRINTF("StartDiscovery API Error: 0x%x \r\n", apiResult);
            }
          
    #endif /* (CY_BLE_CONFIG_GAP_SECURITY_LEVEL != 0x0u) */
            }       
        } 
        break;
        
        case CY_BLE_EVT_GAP_DEVICE_DISCONNECTED:
            DBG_PRINTF("CY_BLE_EVT_GAP_DEVICE_DISCONNECTED: bdHandle=%x, reason=%x, status=%x\r\n",
               (*(cy_stc_ble_gap_disconnect_param_t *)eventParam).bdHandle, 
               (*(cy_stc_ble_gap_disconnect_param_t *)eventParam).reason, 
               (*(cy_stc_ble_gap_disconnect_param_t *)eventParam).status);
            break;
            
        case CY_BLE_EVT_GAP_SMP_NEGOTIATED_AUTH_INFO:
            DBG_PRINTF("CY_BLE_EVT_GAP_SMP_NEGOTIATED_AUTH_INFO:"
                       " bdHandle=%x, security=%x, bonding=%x, ekeySize=%x, err=%x \r\n", 
                (*(cy_stc_ble_gap_auth_info_t *)eventParam).bdHandle, 
                (*(cy_stc_ble_gap_auth_info_t *)eventParam).security, 
                (*(cy_stc_ble_gap_auth_info_t *)eventParam).bonding, 
                (*(cy_stc_ble_gap_auth_info_t *)eventParam).ekeySize, 
                (*(cy_stc_ble_gap_auth_info_t *)eventParam).authErr);
            break;

    #if (CY_BLE_CONFIG_GAP_SECURITY_LEVEL != 0u)            
        case CY_BLE_EVT_GAP_AUTH_FAILED:    
        case CY_BLE_EVT_GAP_AUTH_COMPLETE:
            {    
                bool startDiscovery = false;
                if(event == CY_BLE_EVT_GAP_AUTH_COMPLETE) 
                {
                   DBG_PRINTF("CY_BLE_EVT_GAP_AUTH_COMPLETE: bdHandle=%x, security=%x, bonding=%x, ekeySize=%x, err=%x \r\n", 
                        (*(cy_stc_ble_gap_auth_info_t *)eventParam).bdHandle, 
                        (*(cy_stc_ble_gap_auth_info_t *)eventParam).security, 
                        (*(cy_stc_ble_gap_auth_info_t *)eventParam).bonding, 
                        (*(cy_stc_ble_gap_auth_info_t *)eventParam).ekeySize, 
                        (*(cy_stc_ble_gap_auth_info_t *)eventParam).authErr);
                
                    startDiscovery = true;
                }
                else /* CY_BLE_EVT_GAP_AUTH_FAILED */
                {
                    DBG_PRINTF("CY_BLE_EVT_GAP_AUTH_FAILED: bdHandle=%x, authErr=%x\r\n", 
                        (*(cy_stc_ble_gap_auth_info_t *)eventParam).bdHandle, 
                        (*(cy_stc_ble_gap_auth_info_t *)eventParam).authErr);
                     
                    /* Pairing is not supported by the device. */
                    if((*(cy_stc_ble_gap_auth_info_t *)eventParam).authErr == CY_BLE_GAP_AUTH_ERROR_PAIRING_NOT_SUPPORTED)   
                    {
                        startDiscovery = true;
                    }
                }
                
                if(startDiscovery == true)
                {
                    /* Start discovery only for the Central connection */
                    for(i = 0; i < CY_BLE_CONN_COUNT; i++)
                    {
                        if((appConnInfo.connHandle[i].bdHandle == ((cy_stc_ble_gap_auth_info_t *)eventParam)->bdHandle) &&
                           (Cy_BLE_GetDeviceRole(&appConnInfo.connHandle[i]) == CY_BLE_GAP_LL_ROLE_MASTER))
                        {
                            DBG_PRINTF("Cy_BLE_GATTC_StartDiscovery \r\n");
                            apiResult = Cy_BLE_GATTC_StartDiscovery(appConnInfo.connHandle[i]);
                            if(apiResult != CY_BLE_SUCCESS)
                            {
                                DBG_PRINTF("StartDiscovery API Error: 0x%x \r\n", apiResult);
                            }
                            break;
                        }
                    }
                }
            }
            break;    
    #endif /* (CY_BLE_CONFIG_GAP_SECURITY_LEVEL != 0u) */

        case CY_BLE_EVT_GAP_ENCRYPT_CHANGE:
            DBG_PRINTF("CY_BLE_EVT_GAP_ENCRYPT_CHANGE: %x \r\n", *(uint8_t *)eventParam);
            break;
            
        case CY_BLE_EVT_GAPC_SCAN_PROGRESS_RESULT:
        {
            cy_stc_ble_gapc_adv_report_param_t  *advReport = (cy_stc_ble_gapc_adv_report_param_t *)eventParam;
            
            bool isNewAddress = true;
                        
            
            if((appScanDevInfo.pauseScanProgress == false) && (Cy_BLE_IsPeerConnected(advReport->peerBdAddr) == false) &&
               (advReport->eventType == CY_BLE_GAPC_CONN_UNDIRECTED_ADV))
            {

                isNewAddress = false;
            
                /* Filter and add to the connect list only nodes that advertise ENVSENSOR in ADV payload */
                if(CheckAdvPacketForServiceUuid128(advReport, (void *)cy_ble_customConfigPtr->customc[CY_BLE_CUSTOMC_ENVSENSORS_SERVICE_SERVICE_INDEX].uuid) == true)
                {
                    memcpy(&appScanDevInfo.address[0u].bdAddr, advReport->peerBdAddr, CY_BLE_GAP_BD_ADDR_SIZE); 
                    appScanDevInfo.address[appScanDevInfo.count].type = advReport->peerAddrType;
                    
                    isNewAddress = true;
                    
                    appScanDevInfo.connReq = 1u;
                    (void)Cy_BLE_GAPC_StopScan();
                }
                
          
                /* Display scanned device ADV data */
                if(isNewAddress == true)
                {
                    DBG_PRINTF("\r\n-------------------------------------------------------------------------\r\n");
                    DBG_PRINTF("uuid: ENV SERVICE - YES, added to the connect list  \r\n");
                
                
                    DBG_PRINTF("ADV type: 0x%x address: ", advReport->eventType );
                    for(i = CY_BLE_GAP_BD_ADDR_SIZE; i > 0u; i--)
                    {
                        DBG_PRINTF("%2.2x", advReport->peerBdAddr[i-1]);
                    }
                    DBG_PRINTF(", rssi - %d dBm, data - ", advReport->rssi);
                    for( i = 0; i < advReport->dataLen; i++)
                    {
                        DBG_PRINTF("%2.2x ", advReport->data[i]);
                    }
                    DBG_PRINTF("\r\n");
                            
                    DBG_PRINTF("-------------------------------------------------------------------------\r\n\r\n");
                }  
            }
        
        }
        break; 
                    
        /**********************************************************
        *                       GATT Events
        ***********************************************************/
        case CY_BLE_EVT_GATT_CONNECT_IND:         
            DBG_PRINTF("CY_BLE_EVT_GATT_CONNECT_IND: attId=%x, bdHandle=%x \r\n", 
                (*(cy_stc_ble_conn_handle_t *)eventParam).attId, 
                (*(cy_stc_ble_conn_handle_t *)eventParam).bdHandle);
            
            appConnInfo.connHandle[(*(cy_stc_ble_conn_handle_t *)eventParam).attId ] = 
                           *(cy_stc_ble_conn_handle_t *)eventParam;
            break;
                
        case CY_BLE_EVT_GATT_DISCONNECT_IND:
                      
            DBG_PRINTF("CY_BLE_EVT_GATT_DISCONNECT_IND: attId=%x, bdHandle=%x \r\n", 
                            (*(cy_stc_ble_conn_handle_t *)eventParam).attId, 
                            (*(cy_stc_ble_conn_handle_t *)eventParam).bdHandle);
                
            /* Remove the connected device from the connection handle array */ 
            memset(&appConnInfo.connHandle[(*(cy_stc_ble_conn_handle_t*)eventParam).attId], 
                   CY_BLE_INVALID_CONN_HANDLE_VALUE, sizeof(cy_stc_ble_conn_handle_t));
            
            Cy_BLE_GAPC_StopScan();
            
            break;
        
        case CY_BLE_EVT_GATTS_XCNHG_MTU_REQ:
        { 
            cy_stc_ble_gatt_xchg_mtu_param_t mtu = 
            {
                .connHandle = ((cy_stc_ble_gatt_xchg_mtu_param_t *)eventParam)->connHandle
            };
            Cy_BLE_GATT_GetMtuSize(&mtu);
            DBG_PRINTF("CY_BLE_EVT_GATTS_XCNHG_MTU_REQ %x, %x, final mtu= %d \r\n",  
                        mtu.connHandle.attId, mtu.connHandle.bdHandle, mtu.mtu);
        }
        break;
        
        case CY_BLE_EVT_GATTS_READ_CHAR_VAL_ACCESS_REQ:
            /* Triggered on the Server's side when the Client sends a Read request and when    
            * the characteristic has the CY_BLE_GATT_DB_ATTR_CHAR_VAL_RD_EVENT property set.
            * This event can be ignored by the application except when an error response                
            * is needed and it must be set in the gattErrorCode field 
            * of the event parameter. */
            
            DBG_PRINTF("CY_BLE_EVT_GATTS_READ_CHAR_VAL_ACCESS_REQ %x %x: handle: %x \r\n", 
                ((cy_stc_ble_gatts_char_val_read_req_t *)eventParam)->connHandle.attId,
                ((cy_stc_ble_gatts_char_val_read_req_t *)eventParam)->connHandle.bdHandle,
                ((cy_stc_ble_gatts_char_val_read_req_t *)eventParam)->attrHandle);
            break;
                
        case CY_BLE_EVT_GATTC_DISCOVERY_COMPLETE:
            {
                DBG_PRINTF("Discovery complete: attId=%x, bdHandle=%x \r\n", 
                                (*(cy_stc_ble_conn_handle_t *)eventParam).attId, 
                                (*(cy_stc_ble_conn_handle_t *)eventParam).bdHandle);   
            #if (DEBUG_FULL_ENABLED == ENABLED)
                {
                    uint32_t discIdx = Cy_BLE_GetDiscoveryIdx(*(cy_stc_ble_conn_handle_t *)eventParam);
                    for(i = 0u; i < CY_BLE_SRVI_COUNT; i++)
                    {
                        DBG_PRINTF("  service with UUID 0x%x has range from 0x%x to 0x%x\r\n", 
                               cy_ble_serverInfo[discIdx][i].uuid,
                               cy_ble_serverInfo[discIdx][i].range.startHandle,
                               cy_ble_serverInfo[discIdx][i].range.endHandle);
                    }
                }
            #endif /* (DEBUG_FULL_ENABLED == ENABLED) */     
            
                /* Set flag to sEnable notifications */
                enableNotification = true;
                
                /* Fill all fields of the Write Request structure ... */
                writeReqParam.connHandle = *(cy_stc_ble_conn_handle_t *)eventParam;
                
                DBG_PRINTF("\r\n");
            }
            break;

        case CY_BLE_EVT_GATTC_ERROR_RSP:
            DBG_PRINTF("opCode: %x,  errorCode: %x,\r\n", 
                            ((cy_stc_ble_gatt_err_param_t *)eventParam)->errInfo.opCode,
                            ((cy_stc_ble_gatt_err_param_t *)eventParam)->errInfo.errorCode);
            break;
            
        case CY_BLE_EVT_GATTC_READ_RSP:
            DBG_PRINTF("CY_BLE_EVT_GATTC_READ_RSP: ");
            for(i = 0; i < ((cy_stc_ble_gattc_read_rsp_param_t *)eventParam)->value.len; i++)
            {
                DBG_PRINTF("%2.2x ", ((cy_stc_ble_gattc_read_rsp_param_t *)eventParam)->value.val[i]);
            }
            DBG_PRINTF("\r\n");
            break;

        case CY_BLE_EVT_GATTC_WRITE_RSP:
            DBG_PRINTF("<- CY_BLE_EVT_GATTC_WRITE_RSP: ");
            DBG_PRINTF("bdHandle: 0x%2.2x\r\n", ((cy_stc_ble_conn_handle_t *)eventParam)->bdHandle);
            writeRsp = true;
            break;

        case CY_BLE_EVT_GATTC_HANDLE_VALUE_NTF:
            {
                uint32_t reading;
                uint16_t attrHandle;
                uint16_t readingLen = 0;
                uint8_t attId;
                cy_stc_ble_gattc_handle_value_ntf_param_t *ntfParam = eventParam;
                uint32_t discIdx = Cy_BLE_GetDiscoveryIdx(ntfParam->connHandle);
                if(enableNotification == false)
                {
                    if((attId >=0) || (attId < CY_BLE_MAX_CENTRAL_CONN_NUM))
                    {
                        readingLen = ((cy_stc_ble_gattc_read_rsp_param_t *)eventParam)->value.len;
                        if (readingLen == 2)
                        {
                            reading = (((cy_stc_ble_gattc_read_rsp_param_t *)eventParam)->value.val[1] << 8) + ((cy_stc_ble_gattc_read_rsp_param_t *)eventParam)->value.val[0];
                        }
                        else
                        {
                            reading = (((cy_stc_ble_gattc_read_rsp_param_t *)eventParam)->value.val[3] << 24) +
                                      (((cy_stc_ble_gattc_read_rsp_param_t *)eventParam)->value.val[2] << 16) +
                                      (((cy_stc_ble_gattc_read_rsp_param_t *)eventParam)->value.val[1] << 8) + 
                                       ((cy_stc_ble_gattc_read_rsp_param_t *)eventParam)->value.val[0];
                        }
                        attrHandle = ((cy_stc_ble_gattc_handle_value_ntf_param_t *)eventParam)->handleValPair.attrHandle;
                        attId = ((cy_stc_ble_gattc_handle_value_ntf_param_t *)eventParam)->connHandle.attId;

                        if(attrHandle == cy_ble_customConfigPtr->customc[CY_BLE_CUSTOMC_BME280_TEMP_SERVICE_INDEX].customServChar[0].
                                                      customServCharHandle[discIdx])
                        {
                            sensorData[attId].temperature = reading;   
                        }
                        else if(attrHandle == cy_ble_customConfigPtr->customc[CY_BLE_CUSTOMC_BME280_PRESSURE_SERVICE_INDEX].customServChar[0].
                                                      customServCharHandle[discIdx])
                        {
                             sensorData[attId].pressure = reading;   
                        }
                        else if (attrHandle == cy_ble_customConfigPtr->customc[CY_BLE_CUSTOMC_BME280_HUMIDITY_SERVICE_INDEX].customServChar[0].
                                                      customServCharHandle[discIdx])
                        {
                            sensorData[attId].humidity = reading;   
                        }
                        else if (attrHandle == cy_ble_customConfigPtr->customc[CY_BLE_CUSTOMC_CCS811_TVOC_SERVICE_INDEX].customServChar[0].
                                                      customServCharHandle[discIdx])
                        {
                             sensorData[attId].tvoc = reading;  
                        }
                         else if (attrHandle ==  cy_ble_customConfigPtr->customc[CY_BLE_CUSTOMC_CCS811_ECO2_SERVICE_INDEX].customServChar[0].
                                                      customServCharHandle[discIdx])
                        {
                            sensorData[attId].eco2 = reading;
                            sensorData[attId].deviceNumber = attId;
                            DisplayData2(&sensorData[attId]);   
                        }
                    }
                    else
                    {
                        DBG_PRINTF("CY_BLE_EVT_GATTC_HANDLE_VALUE_NTF: Error AttId out of range.\r\n");   
                    }
                }
            }
            break;
            
        case CY_BLE_EVT_GATTC_HANDLE_VALUE_IND:
            DBG_PRINTF("CY_BLE_EVT_GATTC_HANDLE_VALUE_IND\r\n");
            break;
            
        case CY_BLE_EVT_GATTC_EXEC_WRITE_RSP:
            DBG_PRINTF("CY_BLE_EVT_GATTC_EXEC_WRITE_RSP\r\n");
            break;
                       
        /**********************************************************
        *                       L2CAP Events 
        ***********************************************************/    
        case CY_BLE_EVT_L2CAP_CONN_PARAM_UPDATE_REQ:
            DBG_PRINTF("CY_BLE_EVT_L2CAP_CONN_PARAM_UPDATE_REQ \r\n");
            break;

        /**********************************************************
        *                       Other Events
        ***********************************************************/
        case CY_BLE_EVT_PENDING_FLASH_WRITE:
            /* Inform the application that a flash Write is pending. The Stack internal data 
             * structures are modified and must be stored in the flash using
             * Cy_BLE_StoreBondingData() */
            DBG_PRINTF("CY_BLE_EVT_PENDING_FLASH_WRITE\r\n");
            break;
            
        case CY_BLE_EVT_LE_PING_AUTH_TIMEOUT:
            DBG_PRINTF("CY_BLE_EVT_LE_PING_AUTH_TIMEOUT\r\n");
            break;
            
        default:
            DBG_PRINTF("Other event: 0x%lx \r\n", event);
            break;
    }
}


/*******************************************************************************
* Function Name: Connect
********************************************************************************
*
* Summary:
*   Connects to a peer device.
*
* Parameters:
*  \param peerAddrType: The type of the peer address.
*  \param peerBdAddr: The pointer to the peer address array
*
*******************************************************************************/
void Connect(uint8_t peerAddrType, uint8_t *peerBdAddr)
{
    cy_en_ble_api_result_t apiResult;
    cy_stc_ble_gap_bd_addr_t bdAddr = {.type = peerAddrType };    
    if(Cy_BLE_GetNumOfActiveConn() >= CY_BLE_MAX_CENTRAL_CONN_NUM)
    {
        DBG_PRINTF("MAX peers(%x) are connected \r\n", CY_BLE_MAX_CENTRAL_CONN_NUM);
    }
    else
    {
        memcpy(&bdAddr.bdAddr, peerBdAddr, CY_BLE_GAP_BD_ADDR_SIZE); 
        apiResult = Cy_BLE_GAPC_ConnectDevice(&bdAddr, CY_BLE_CENTRAL_CONFIGURATION_0_INDEX);
        if(apiResult != CY_BLE_SUCCESS)
        {
            DBG_PRINTF("ConnectDevice API Error: 0x%x ", apiResult);
        }
        else
        {
            DBG_PRINTF("Connecting to the device ");
        } 
        DBG_PRINTF("(address  - %2.2x%2.2x%2.2x%2.2x%2.2x%2.2x) \r\n", bdAddr.bdAddr[5u], bdAddr.bdAddr[4u],
                                                                       bdAddr.bdAddr[3u], bdAddr.bdAddr[2u], 
                                                                       bdAddr.bdAddr[1u], bdAddr.bdAddr[0u]);    
    }
}

/*******************************************************************************
* Function Name: Disconnect
********************************************************************************
*
* Summary:
*   Disconnects a peer device.
*
* Parameters:
*   \param connHandle: The connection handle of a peer device.
*
*******************************************************************************/
void Disconnect(cy_stc_ble_conn_handle_t connHandle)
{
    cy_en_ble_api_result_t apiResult;
    cy_stc_ble_gap_disconnect_info_t param =
    {
        .bdHandle = connHandle.bdHandle,
        .reason   = CY_BLE_HCI_ERROR_OTHER_END_TERMINATED_USER
    };
    
    DBG_PRINTF("Cy_BLE_GAP_Disconnect param: bdHandle:%x, reason:%x \r\n", param.bdHandle, 
                                                                         param.reason);
    apiResult = Cy_BLE_GAP_Disconnect(&param);
    if(apiResult != CY_BLE_SUCCESS)
    {
        DBG_PRINTF("Cy_BLE_GAP_Disconnect API Error: 0x%x \r\n", apiResult);
    }
    else
    {
        DBG_PRINTF("Cy_BLE_GAP_Disconnect API Success \r\n");
    } 
}

/****************************************************************************** 
* Function Name: CheckAdvPacketForServiceUuid128
*******************************************************************************
*
* Summary:
*   This function parses advertising packet and returns nonzero value 
*   when the packet contains service UUID equal to input parameter.
*
* Parameters:
*   eventParam: the pointer to a data structure specified by the event.
*   uuid: 16-bit UUID of the service.
*
* Return:
*   Nonzero value when the advertising packet contains service UUID.
*
******************************************************************************/
static uint32_t CheckAdvPacketForServiceUuid128(cy_stc_ble_gapc_adv_report_param_t *eventParam, void * uuid128)
{
    bool servicePresent = false; 
    uint32_t advIndex   = 0u;
    uint32_t i;

    DBG_PRINTF(".");
    do
    {
        /* Find complete or incomplete Service UUID list field type. */
        if((eventParam->data[advIndex + 1u] == (uint8_t)CY_BLE_GAP_ADV_INCOMPL_128_UUID) ||
           (eventParam->data[advIndex + 1u] == (uint8_t)CY_BLE_GAP_ADV_COMPL_128_UUID))
        {                                                   
            /* Compare uuid values with input parameter */
            for(i = 0u; (i < (eventParam->data[advIndex] - 1u)) && (servicePresent == false); i += CY_BLE_GATT_128_BIT_UUID_SIZE)
            {
                if((memcmp(uuid128, &eventParam->data[advIndex + 2u + i], CY_BLE_GATT_128_BIT_UUID_SIZE)) == 0u)
                {
                    servicePresent = true;
                }
            }
        }
        advIndex += eventParam->data[advIndex] + 1u;
    }while((advIndex < eventParam->dataLen) && (servicePresent == false));    
    
    return(servicePresent);
}




/****************************************************************************** 
* Function Name: EnableNotifications
*******************************************************************************
*
* Summary:
*   This function enable all notifications for discovered services:
*       - CY_BLE_CUSTOMC_BME280_TEMP_SERVICE_INDEX       (0)
*       - CY_BLE_CUSTOMC_BME280_PRESSURE_SERVICE_INDEX   (1)
*       - CY_BLE_CUSTOMC_BME280_HUMIDITY_SERVICE_INDEX   (2)
*       - CY_BLE_CUSTOMC_CCS811_TVOC_SERVICE_INDEX       (3)
*       - CY_BLE_CUSTOMC_CCS811_ECO2_SERVICE_INDEX       (4)
*
******************************************************************************/
static void EnableNotifications(void)
{
    cy_en_ble_api_result_t apiResult;
    uint32_t discIdx = Cy_BLE_GetDiscoveryIdx(writeReqParam.connHandle);
    uint32_t i;
    uint16_t cccd = CY_BLE_CCCD_NOTIFICATION;
    
    /* Enable notifications */
    for(i = CY_BLE_CUSTOMC_BME280_TEMP_SERVICE_INDEX; i <= CY_BLE_CUSTOMC_CCS811_ECO2_SERVICE_INDEX; i++)
    {
        /*                                                   |-> Service index     |-> CCCD index          */
        if(cy_ble_customConfigPtr->customc[i].customServChar[0].customServCharDesc[0].descHandle[discIdx] != 
            CY_BLE_GATT_INVALID_ATTR_HANDLE_VALUE )       /*  descHandle multiple for 4 conections <-|     */
        {
            DBG_PRINTF("-> Enable notifications for service 0x%lx [handle: 0x%x]: ", i,
                        cy_ble_customConfigPtr->customc[i].customServChar[0].customServCharDesc[0].descHandle[discIdx]);
    
            writeReqParam.handleValPair.value.val  = (uint8 *)&cccd;
            writeReqParam.handleValPair.value.len  = sizeof(cccd);
            writeReqParam.handleValPair.attrHandle = cy_ble_customConfigPtr->customc[i].customServChar[0].
                                                                             customServCharDesc[0].descHandle[discIdx];
            apiResult = Cy_BLE_GATTC_WriteCharacteristicDescriptors( &writeReqParam );

            if(apiResult != CY_BLE_SUCCESS)
            {
                DBG_PRINTF("failed. Error code: %x \r\n", apiResult);   
            }
            else
            {
                DBG_PRINTF("successful. \r\n");
            }
            
            while(!writeRsp)
            {
                Cy_BLE_ProcessEvents();               
            }

            writeRsp = false;
        }
        
        Cy_BLE_ProcessEvents();
    }
    DBG_PRINTF("\r\n");
    
    enableNotification = false;    
    appScanDevInfo.pauseScanProgress = 0u; /* Reset pauseScanProgress, so we can enable Scan procedure */
}


/*******************************************************************************
* Function Name: GetAppConnInfoPtr
********************************************************************************
*
* Summary:
*  Returns a pointer to appConnInfo structure
*
*******************************************************************************/
app_stc_connection_info_t* GetAppConnInfoPtr(void)
{ 
    return(&appConnInfo);
}


/*******************************************************************************
* Function Name: GetAppScanDevInfoPtr
********************************************************************************
*
* Summary:
*  Returns a pointer to appScanDevInfo structure
*
*******************************************************************************/
app_stc_scan_device_list_t* GetAppScanDevInfoPtr(void)
{ 
    return(&appScanDevInfo);
}


/*******************************************************************************
* Function Name: Timer_Interrupt
********************************************************************************
*
* Summary:
*  Handles the Interrupt Service Routine for the MCWDT timer.
*
*******************************************************************************/
void Timer_Interrupt(void)
{
    /* Update Led State */
    UpdateLedState();
    
    /* Indicate that timer is raised to the main loop */
    mainTimer++;
    systemClock++;
       
    MCWDT_ClearInterrupt(CY_MCWDT_CTR0);
}

/*******************************************************************************
* Function Name: DisplayData
********************************************************************************
*
* Summary:
*  Prints sensor data.
*
*******************************************************************************/
/*void DisplayData(app_stc_sensor_data_t * sensorData)
{
    DBG_PRINTF("\r\nSensor data from device=%d \r\n", sensorData->deviceNumber);
    DBG_PRINTF(" -- Temp =%d\n\r", sensorData->temperature);
    DBG_PRINTF(" -- Pres =%d\n\r", sensorData->pressure);
        DBG_PRINTF(" -- Humid=%d\n\r", sensorData->humidity);
    DBG_PRINTF(" -- TVOC =%d\n\r", sensorData->tvoc);
    DBG_PRINTF(" -- ECO2 =%d\n\r", sensorData->eco2);
}*/

/*******************************************************************************
* Function Name: DisplayData2
********************************************************************************
*
* Summary:
*  Prints sensor data.
*
*******************************************************************************/
void DisplayData2(app_stc_sensor_data_t * sensorData)
{
    app_stc_connection_info_t *appConnInfoPtr = GetAppConnInfoPtr();
    static uint32_t tick = 0;
    
    if(appConnInfoPtr != NULL)
    {
        if(Cy_BLE_GetConnectionState(appConnInfoPtr->connHandle[sensorData->deviceNumber]) >= CY_BLE_CONN_STATE_CONNECTED)
        {
            /* Get BdAddress from bdHandle of the connected device */
            cy_stc_ble_gap_peer_addr_info_t param = {.bdHandle = appConnInfoPtr->connHandle[sensorData->deviceNumber].bdHandle};
            Cy_BLE_GAP_GetPeerBdAddr(&param);
            DBG_PRINTF("\r\n");
            PRINT_DEVICE_ADDR(param.bdAddr, appConnInfoPtr->connHandle[sensorData->deviceNumber] ); 
            DBG_PRINTF(" -- Temp =%i\n\r", sensorData->temperature);
            DBG_PRINTF(" -- Pres =%u\n\r", sensorData->pressure);
            DBG_PRINTF(" -- Humid=%u\n\r", sensorData->humidity);
            DBG_PRINTF(" -- TVOC =%d\n\r", sensorData->tvoc);
            DBG_PRINTF(" -- ECO2 =%d\n\r", sensorData->eco2);
            DBG_PRINTF(" -- Time =%d\n\r", systemClock);
            if (systemClock - tick > 20)
            {
              tick = systemClock;
              char buffer[100];
              DBG_PRINTF("Posted!\n\r");
              // In the following line, replace the Xs with your API key from ThingSpeak.
              sprintf(buffer, "GET /update?key=KB7PA9GHQVC0O9PF&field1=%i&field2=%u&field3=%u&field4=%d&field5=%d HTTP/1.1\r\n",
                      sensorData->temperature, sensorData->pressure, sensorData->humidity, sensorData->tvoc, sensorData->eco2);
              XBEE_UART_PutString(buffer);
              XBEE_UART_PutString("Host: api.thingspeak.com\r\n\r\n");
            }

        }
    }
}

/*******************************************************************************
* Function Name: HostMain
********************************************************************************
* Summary:
*  The main function for the project.
*
* Theory:
*  The function starts the BLE and UART components.
*  This function processes all BLE events and also implements the low power 
*  functionality.
*
*******************************************************************************/
int HostMain(void)
{
    cy_en_ble_api_result_t   apiResult;
    char8                    command;
    
    /* Initialization the user interface: LEDs, SW2, ect.  */
    InitUserInterface();
    
    Cy_SysTick_Init(CY_SYSTICK_CLOCK_SOURCE_CLK_IMO, 8000000);
    Cy_SysTick_DisableInterrupt();
    
    /* Initialize Debug UART */
    UART_DEB_Start();
    XBEE_UART_Start();
    XBeeWiFiInit();
    DBG_PRINTF("BLE Multi Master Single Slave Example \r\n");
             
    /* Start the BLE component and register the generic event handler */
    apiResult = Cy_BLE_Start(AppCallBack);
    if(apiResult != CY_BLE_SUCCESS)
    {
        DBG_PRINTF("Cy_BLE_Start API Error: 0x%x \r\n", apiResult);
    }

    DBG_PRINTF("Cy_SysLib_GetDeviceRevision(): %u [%u] \r\n", Cy_SysLib_GetDeviceRevision(), CY_SYSLIB_DEVICE_REV_0A);
    
    /* Print stack version */
    PrintStackVersion();

    /***************************************************************************
    * Main polling loop
    ***************************************************************************/
    while(1u) 
    {   
        /* Cy_BLE_ProcessEvents() allows the BLE stack to process pending events */
        Cy_BLE_ProcessEvents();
                
        /* Restart the timer (1 sev) */
        if(mainTimer != 0u)
        {
            mainTimer = 0u;
        }  
                        
        /* Process command from debug terminal */
        if((command = UART_DEB_GET_CHAR()) != UART_DEB_NO_DATA) 
        {
            ProcessUartCommands(command);     
        }
       
    
        /*******************************************************************************************/
        /*    Scan / Connect / Enable notification handling                                        */
        /*******************************************************************************************/
        
        /** 
         * Enabled Notification for connected device 
         */
        if(enableNotification == true)
        { 
            EnableNotifications(); 
        }  
        
        /** 
         * Connect to peer device 
         */
        if((appScanDevInfo.connReq != 0u) && (Cy_BLE_GetScanState() == CY_BLE_SCAN_STATE_STOPPED))
        {
            
            apiResult = Cy_BLE_GAPC_ConnectDevice(&appScanDevInfo.address[appScanDevInfo.connReq - 1u], 
                                                           CY_BLE_CENTRAL_CONFIGURATION_0_INDEX);
            if(apiResult != CY_BLE_SUCCESS)
            {
                DBG_PRINTF("ConnectDevice API Error: 0x%x ", apiResult);
            }
            else
            {
                DBG_PRINTF("Connecting to the device ");
                /* Print the devices Addr of the detected Client */
                DBG_PRINTF("%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x \r\n",
                                            appScanDevInfo.address[appScanDevInfo.connReq - 1u].bdAddr[5u], 
                                            appScanDevInfo.address[appScanDevInfo.connReq - 1u].bdAddr[4u],
                                            appScanDevInfo.address[appScanDevInfo.connReq - 1u].bdAddr[3u], 
                                            appScanDevInfo.address[appScanDevInfo.connReq - 1u].bdAddr[2u],
                                            appScanDevInfo.address[appScanDevInfo.connReq - 1u].bdAddr[1u],
                                            appScanDevInfo.address[appScanDevInfo.connReq - 1u].bdAddr[0u]); 
                appScanDevInfo.connReq = 0;
                appScanDevInfo.pauseScanProgress = true;
                
            } 
        }
       
        /**
         * Auto Start scan for detect next device 
         */   
        if((Cy_BLE_GetNumOfActiveConn() < CY_BLE_MAX_CENTRAL_CONN_NUM) && (!appScanDevInfo.pauseScanProgress) && 
           (Cy_BLE_GetScanState() == CY_BLE_SCAN_STATE_STOPPED) &&
           (Cy_BLE_GetState() == CY_BLE_STATE_ON))
        {
            apiResult = Cy_BLE_GAPC_StartScan(CY_BLE_SCANNING_FAST, 0u);
            if(apiResult != CY_BLE_SUCCESS)
            {
                DBG_PRINTF("Cy_BLE_GAPC_StartScan API Error: 0x%x \r\n", apiResult);
            }
            else
            {
                DBG_PRINTF("Cy_BLE_GAPC_StartScan API Success \r\n");
            }                   
        }
                
    #if(CY_BLE_BONDING_REQUIREMENT == CY_BLE_BONDING_YES)
        /* Store bonding data to flash only when all debug information has been sent */    
        if(cy_ble_pendingFlashWrite != 0u) 
        {   
            apiResult = Cy_BLE_StoreBondingData();   
            if((apiResult == CY_BLE_SUCCESS) || (/* out any ERROR */apiResult & (1u << CY_PDL_STATUS_TYPE_Pos)) != 0u)
            {
                DBG_PRINTF("Store bonding data, status: %x, pending: %x \r\n", apiResult, cy_ble_pendingFlashWrite);
                
            }
        }
    #endif /* CY_BLE_BONDING_REQUIREMENT == CY_BLE_BONDING_YES */
           
    }  
}

    
/* [] END OF FILE */

