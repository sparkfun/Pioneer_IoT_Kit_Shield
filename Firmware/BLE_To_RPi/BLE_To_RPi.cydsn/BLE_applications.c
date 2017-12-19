/******************************************************************************
* File Name: BLE_applications.c
*
* Version: 1.00
*
* Description: This file contains functions that handle custom BLE services
*
* Related Document: CE218134_BLE_CapSense.pdf
*
* Hardware Dependency: CY8CKIT-062-BLE PSoC 6 BLE Pioneer Kit
*
******************************************************************************
* Copyright (2017), Cypress Semiconductor Corporation.
******************************************************************************
* This software, including source code, documentation and related materials
* ("Software") is owned by Cypress Semiconductor Corporation (Cypress) and is
* protected by and subject to worldwide patent protection (United States and
* foreign), United States copyright laws and international treaty provisions.
* Cypress hereby grants to licensee a personal, non-exclusive, non-transferable
* license to copy, use, modify, create derivative works of, and compile the
* Cypress source code and derivative works for the sole purpose of creating
* custom software in support of licensee product, such licensee product to be
* used only in conjunction with Cypress's integrated circuit as specified in the
* applicable agreement. Any reproduction, modification, translation, compilation,
* or representation of this Software except as specified above is prohibited
* without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* Cypress reserves the right to make changes to the Software without notice.
* Cypress does not assume any liability arising out of the application or use
* of Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use as critical components in any products
* where a malfunction or failure may reasonably be expected to result in
* significant injury or death ("ACTIVE Risk Product"). By including Cypress's
* product in a ACTIVE Risk Product, the manufacturer of such system or application
* assumes all risk of such use and in doing so indemnifies Cypress against all
* liability. Use of this Software may be limited by and subject to the applicable
* Cypress software license agreement.
*****************************************************************************/
/*******************************************************************************
* This file contains functions that handle custom BLE services, which includes
* the CapSense Slider service, CapSense button service and the RGB LED service
*******************************************************************************/

/* Header file includes */
#include "BLE_applications.h"
#include "handle_RGB.h"
#include "BLE_CapSense_interface.h"

/* Client Characteristic Configuration descriptor (CCCD) data length */
#define CCCD_DATA_LEN   (uint8_t) (0x02u)

/* Indexes of a two-byte CCCD array */
#define CCCD_INDEX_0    (uint8_t) (0x00u)
#define CCCD_INDEX_1    (uint8_t) (0x01u)

/* Null byte used to initialize CCCD values */
#define CCCD_NULL       (uint8_t) (0x00u)

/* 'connectionHandle' stores the BLE connection parameters */
cy_stc_ble_conn_handle_t connectionHandle;

/* Array to store the present RGB LED control data. The four bytes of the array 
   represent {R,G,B,Intensity} respectively */
uint8_t             RGBData[RGB_DATA_LEN];

/* These flags are set when the Central device writes to CCCD (Client 
   Characteristic Configuration Descriptor) of the CapSense and RGB LED 
   Characteristic to enable notifications */
bool                sendCapSenseSliderNotifications = false;
bool                sendCapSenseButtonNotifications = false;
bool                RGBledNotifications = false;

/* This flag is used by application to know whether a Central device has been 
   connected. This value is continuously updated in BLE event callback
   function */
bool                deviceConnected = false;

/* This is used to restart advertisements in the main firmware loop */
bool                restartAdvertisement = true;

/* Status flag for the Stack Busy state. This flag is used to notify the 
   application if there is stack buffer free to push more data or not */
bool                busyStatus = false;

/*******************************************************************************
* Function Name: void startAdvertisement(void)
********************************************************************************
* Summary:
*  Check the "restartAdvertisement" flag and start the BLE advertisement if
*  the flag is "true"
*
* Parameters:
*  Void
*
* Return:
*  void
*
*******************************************************************************/
void startAdvertisement(void)
{
    /* Check if the restartAdvertisement flag is set by the event handler */
    if(restartAdvertisement)
	{
		/* Reset 'restartAdvertisement' flag */
		restartAdvertisement = false;
		
		/* Start Advertisement and enter discoverable mode */
		Cy_BLE_GAPP_StartAdvertisement(CY_BLE_ADVERTISING_FAST,
                       CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX);	
	}
}

/*******************************************************************************
* Function Name: void sendCapSenseSliderNotification(uint8_t CapSenseSliderData)
********************************************************************************
* Summary:
*  Send CapSense Slider data as BLE Notifications. This function updates
*  the notification handle with data and triggers the BLE component to send
*  notification
*
* Parameters:
*  CapSenseSliderData:	CapSense slider value
*
* Return:
*  void
*
*******************************************************************************/
void sendCapSenseSliderNotification(uint8_t CapSenseSliderData)
{
    /* 'CapSenseSlidernotificationHandle' stores CapSense notification data
       parameters */
    cy_stc_ble_gatts_handle_value_ntf_t  CapSenseSlidernotificationHandle;

    /* If stack is not busy, then send the notification */
    if (busyStatus == CY_BLE_STACK_STATE_FREE)
    {
        /* Update notification handle with CapSense slider data */
        CapSenseSlidernotificationHandle.connHandle = connectionHandle;
        CapSenseSlidernotificationHandle.handleValPair.attrHandle = 
                            CY_BLE_CAPSENSE_SLIDER_CAPSENSE_SLIDER_CHAR_HANDLE;
        CapSenseSlidernotificationHandle.handleValPair.value.val = 
                            &CapSenseSliderData;
        CapSenseSlidernotificationHandle.handleValPair.value.len =
                            CAPSENSE_SLIDER_DATA_LEN;

        /* Send the updated handle as part of attribute for notifications */
        Cy_BLE_GATTS_Notification(&CapSenseSlidernotificationHandle);
    }
}

/*******************************************************************************
* Function Name: void sendCapSenseButtonNotification(uint8_t *CapSenseButtonData)
********************************************************************************
* Summary:
*  Send CapSense Button data as BLE Notifications. This function updates
*  the notification handle with data and triggers the BLE component to send
*  notification
*
* Parameters:
*  CapSenseButtonData:	CapSense slider value
*
* Return:
*  void
*
*******************************************************************************/
void sendCapSenseButtonNotification(uint8_t *CapSenseButtonData)
{
    /* 'CapSenseButtonNotificationHandle' stores CapSense notification data 
       parameters */
    cy_stc_ble_gatts_handle_value_ntf_t  CapSenseButtonNotificationHandle;

    /* If stack is not busy, then send the notification */
    if (busyStatus == CY_BLE_STACK_STATE_FREE)
    {
        /* Update notification handle with CapSense slider data */
        CapSenseButtonNotificationHandle.connHandle = connectionHandle;
        CapSenseButtonNotificationHandle.handleValPair.attrHandle = 
                            CY_BLE_CAPSENSE_BUTTON_CAPSENSE_BUTTON_CHAR_HANDLE;
        CapSenseButtonNotificationHandle.handleValPair.value.val = 
                            CapSenseButtonData;
        CapSenseButtonNotificationHandle.handleValPair.value.len = 
                            CAPSENSE_BUTTON_DATA_LEN;

        /* Send the updated handle as part of attribute for notifications */
        Cy_BLE_GATTS_Notification(&CapSenseButtonNotificationHandle);
    }
}

/*******************************************************************************
* Function Name: void sendRGBNotification(uint8_t *RGBLedData)
********************************************************************************
* Summary:
*  Send RGB LED data as BLE Notifications. This function updates
*  the notification handle with data and triggers the BLE component to send
*  notification
*
* Parameters:
*  RGBLedData:	pointer to an array containing RGB color and Intensity values
*
* Return:
*  void
*
*******************************************************************************/
void sendRGBNotification(uint8_t *RGBLedData)
{
    /* 'rgbLednotificationHandle' stores RGB LED notification data parameters */
    cy_stc_ble_gatts_handle_value_ntf_t  rgbLednotificationHandle;

    /* If stack is not busy, then send the notification */
    if (busyStatus == CY_BLE_STACK_STATE_FREE)
    {
        /* Update notification handle will CapSense slider data */
        rgbLednotificationHandle.connHandle = connectionHandle;
        rgbLednotificationHandle.handleValPair.attrHandle = 
                                 CY_BLE_RGB_LED_RGB_LED_CONTROL_CHAR_HANDLE;
        rgbLednotificationHandle.handleValPair.value.val = RGBLedData;
        rgbLednotificationHandle.handleValPair.value.len = RGB_DATA_LEN;

        /* Send the updated handle as part of attribute for notifications */
        Cy_BLE_GATTS_Notification(&rgbLednotificationHandle);
    }
}
/*******************************************************************************
* Function Name: void updateRGB(void)
********************************************************************************
* Summary:
*  Receive the new RGB data and update the read characteristic handle so that the
*  next read from the BLE central device gives the current RGB color and intensity 
*  data. This function also calls setColorRGB() function to set the color of the
*  RGB LED per the current value
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void updateRGB(void)
{
    /* Local variable that stores RGB attribute value*/
    cy_stc_ble_gatts_db_attr_val_info_t  RGBattributeValue;

    /* 'RGBhandle' stores RGB control data parameters */
    cy_stc_ble_gatt_handle_value_pair_t  RGBhandle;

    /* Update RGB control handle with new values */
    RGBhandle.attrHandle = CY_BLE_RGB_LED_RGB_LED_CONTROL_CHAR_HANDLE;
    RGBhandle.value.val = RGBData;
    RGBhandle.value.len = RGB_DATA_LEN;
    RGBattributeValue.handleValuePair = RGBhandle;
    RGBattributeValue.offset = CCCD_NULL;
    RGBattributeValue.connHandle = connectionHandle;
    RGBattributeValue.flags = CCCD_NULL;

    /* Send updated RGB control handle as an attribute to the central device, 
       so that the central reads the new RGB color data */
    Cy_BLE_GATTS_WriteAttributeValueCCCD(&RGBattributeValue);
    
    /* Set the color of the RGB LED to match the current values */
    setColorRGB(RGBData);
}

/*******************************************************************************
* Function Name: void handleDisconnectEventforSlider(void)
********************************************************************************
* Summary:
*  This functions handles the 'disconnect' event for the Slider service
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void handleDisconnectEventforSlider(void)
{
    /* Local variable 'attributeHandle' stores attribute parameters*/
    cy_stc_ble_gatts_db_attr_val_info_t  attributeHandle;
    
    /* Handle value to update the CCCD */
    cy_stc_ble_gatt_handle_value_pair_t  CapSenseSliderNotificationCCCDhandle;

    /* Local variable to store the current CCCD value */
    uint8_t CapSenseSliderCCCDvalue[CCCD_DATA_LEN];

    /* Reset CapSense slider notification flag to prevent further notifications
        being sent to Central device after next connection. */
    sendCapSenseSliderNotifications = false;

    /* Reset the Slider CCCD value to disable notifications */
    /* Write the present CapSense notification status to the local variable */
    CapSenseSliderCCCDvalue[CCCD_INDEX_0] = sendCapSenseSliderNotifications;
    CapSenseSliderCCCDvalue[CCCD_INDEX_1] = CCCD_NULL;

    /* Update the Slider CCCD handle with notification status data*/
    CapSenseSliderNotificationCCCDhandle.attrHandle 
    = CY_BLE_CAPSENSE_SLIDER_CAPSENSE_SLIDER_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE;
    CapSenseSliderNotificationCCCDhandle.value.val = CapSenseSliderCCCDvalue;
    CapSenseSliderNotificationCCCDhandle.value.len = CCCD_DATA_LEN;

    /* Report the Slider data to BLE component for sending data when read by 
       the Central device */
    attributeHandle.handleValuePair = CapSenseSliderNotificationCCCDhandle;
    attributeHandle.offset = CCCD_NULL;
    attributeHandle.connHandle = connectionHandle;
    attributeHandle.flags = CY_BLE_GATT_DB_PEER_INITIATED;
    Cy_BLE_GATTS_WriteAttributeValueCCCD(&attributeHandle);
}

/*******************************************************************************
* Function Name: void handleDisconnectEventforButtons(void)
********************************************************************************
* Summary:
*  This functions handles the 'disconnect' event for the Button service
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void handleDisconnectEventforButtons(void)
{
    /* Local variable 'attributeHandle' stores attribute parameters*/
    cy_stc_ble_gatts_db_attr_val_info_t  attributeHandle;
    
    /* Handle value to update the CCCD */
    cy_stc_ble_gatt_handle_value_pair_t  CapSenseButtonNotificationCCCDhandle;

    /* Local variable to store the current CCCD value */
    uint8_t CapSenseButtonCCCDvalue[CCCD_DATA_LEN];

    /* Reset CapSense button notification flag to prevent further notifications
    being sent to Central device after next connection. */
    sendCapSenseButtonNotifications = false;

    /* Reset the Button CCCD value to disable notifications */
    /* Write the present CapSense notification status to the local variable */
    CapSenseButtonCCCDvalue[CCCD_INDEX_0] = sendCapSenseButtonNotifications;
    CapSenseButtonCCCDvalue[CCCD_INDEX_1] = CCCD_NULL;

    /* Update the Button CCCD handle with notification status data*/
    CapSenseButtonNotificationCCCDhandle.attrHandle
    = CY_BLE_CAPSENSE_BUTTON_CAPSENSE_BUTTON_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE;
    CapSenseButtonNotificationCCCDhandle.value.val = CapSenseButtonCCCDvalue;
    CapSenseButtonNotificationCCCDhandle.value.len = CCCD_DATA_LEN;

    /* Report the button data to BLE component for sending data when read by 
       the Central device */
    attributeHandle.handleValuePair = CapSenseButtonNotificationCCCDhandle;
    attributeHandle.offset = CCCD_NULL;
    attributeHandle.connHandle = connectionHandle;
    attributeHandle.flags = CY_BLE_GATT_DB_PEER_INITIATED;
    Cy_BLE_GATTS_WriteAttributeValueCCCD(&attributeHandle);
}

/*******************************************************************************
* Function Name: void handleDisconnectEventforRGB(void)
********************************************************************************
* Summary:
*  This functions handles the 'disconnect' event for the RGB LED service
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void handleDisconnectEventforRGB(void)
{
    /* Local variable 'attributeHandle' stores attribute parameters*/
    cy_stc_ble_gatts_db_attr_val_info_t  attributeHandle;

    /* Handle value to update the CCCD */
    cy_stc_ble_gatt_handle_value_pair_t  RGBNotificationCCCDhandle;

    /* Local variable to store the current CCCD value */
    uint8_t                         RGBCCCDvalue[CCCD_DATA_LEN];

    /* Reset RGB notification flag to prevent further notifications
       being sent to Central device after next connection. */
    RGBledNotifications = false;

    /* Write the present RGB notification status to the local variable */
    RGBCCCDvalue[CCCD_INDEX_0] = RGBledNotifications;
    RGBCCCDvalue[CCCD_INDEX_1] = CCCD_NULL;

    /* Update CCCD handle with notification status data */
    RGBNotificationCCCDhandle.attrHandle 
    = CY_BLE_RGB_LED_RGB_LED_CONTROL_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE;
    RGBNotificationCCCDhandle.value.val = RGBCCCDvalue;
    RGBNotificationCCCDhandle.value.len = CCCD_DATA_LEN;

    /* Report data to BLE component for sending data when read by Central device */
    attributeHandle.handleValuePair = RGBNotificationCCCDhandle;
    attributeHandle.offset = CCCD_NULL;
    attributeHandle.connHandle = connectionHandle;
    attributeHandle.flags = CY_BLE_GATT_DB_PEER_INITIATED;
    Cy_BLE_GATTS_WriteAttributeValueCCCD(&attributeHandle);

    /* Reset the color coordinates */
    RGBData[RED_INDEX] = RGB_LED_OFF;
    RGBData[GREEN_INDEX] = RGB_LED_OFF;
    RGBData[BLUE_INDEX] = RGB_LED_OFF;
    RGBData[INTENSITY_INDEX] = RGB_LED_OFF;
    
    /* Set the color of the RGB LED to match the current values */
    updateRGB();
}

/*******************************************************************************
* Function Name: void handleWriteRequestforSlider
*                     (cy_stc_ble_gatts_write_cmd_req_param_t *writeRequest)
********************************************************************************
* Summary:
*  This functions handles the 'write request' event for the Slider service
*
* Parameters:
*  writeRequest : pointer to the write request parameters from the central        
*
* Return:
*  void
*
*******************************************************************************/
void handleWriteRequestforSlider(cy_stc_ble_gatts_write_cmd_req_param_t *writeRequest)
{
    /* Local variable 'attributeHandle' stores attribute parameters*/
    cy_stc_ble_gatts_db_attr_val_info_t  attributeHandle;

    /* Handle value to update the CCCD */
    cy_stc_ble_gatt_handle_value_pair_t  CapSenseSliderNotificationCCCDhandle;
    
    /* Local variable to store the current CCCD value */
    uint8_t CapSenseSliderCCCDvalue[CCCD_DATA_LEN];

    /* Extract the Write value sent by the Client for CapSense Slider CCCD */
    if (writeRequest->handleValPair.value.val
        [CY_BLE_CAPSENSE_SLIDER_CAPSENSE_SLIDER_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX]
        == true)
    {
        sendCapSenseSliderNotifications = true;
    }
    else if (writeRequest->handleValPair.value.val
            [CY_BLE_CAPSENSE_SLIDER_CAPSENSE_SLIDER_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX]
            == false)
    {
        sendCapSenseSliderNotifications = false;
    }

    /* Write the present CapSense notification status to the local variable */
    CapSenseSliderCCCDvalue[CCCD_INDEX_0] = sendCapSenseSliderNotifications;
    CapSenseSliderCCCDvalue[CCCD_INDEX_1] = CCCD_NULL;

    /* Update CCCD handle with notification status data */
    CapSenseSliderNotificationCCCDhandle.attrHandle 
    = CY_BLE_CAPSENSE_SLIDER_CAPSENSE_SLIDER_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE;
    CapSenseSliderNotificationCCCDhandle.value.val = CapSenseSliderCCCDvalue;
    CapSenseSliderNotificationCCCDhandle.value.len = CCCD_DATA_LEN;

    /* Report data to BLE component for sending data when read by 
       the central device */
    attributeHandle.handleValuePair = CapSenseSliderNotificationCCCDhandle;
    attributeHandle.offset = CCCD_NULL;
    attributeHandle.connHandle = connectionHandle;
    attributeHandle.flags = CY_BLE_GATT_DB_PEER_INITIATED;
    Cy_BLE_GATTS_WriteAttributeValueCCCD(&attributeHandle);
}

/*******************************************************************************
* Function Name: void handleWriteRequestforButtons
*                     (cy_stc_ble_gatts_write_cmd_req_param_t *writeRequest)
********************************************************************************
* Summary:
*  This functions handles the 'write request' event for the Button service
*
* Parameters:
*  writeRequest : pointer to the write request parameters from the central       
*
* Return:
*  void
*
*******************************************************************************/
void handleWriteRequestforButtons(cy_stc_ble_gatts_write_cmd_req_param_t *writeRequest)
{
    /* Local variable 'attributeHandle' stores attribute parameters*/
    cy_stc_ble_gatts_db_attr_val_info_t  attributeHandle;
    
    /* Handle value to update the CCCD */
    cy_stc_ble_gatt_handle_value_pair_t  CapSenseButtonNotificationCCCDhandle;

    /* Local variable to store the current CCCD value */
    uint8_t                         CapSenseButtonCCCDvalue[CCCD_DATA_LEN];

    /* Extract the Write value sent by the Client for CapSense Button CCCD */
    if (writeRequest->handleValPair.value.val
        [CY_BLE_CAPSENSE_BUTTON_CAPSENSE_BUTTON_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX]
         == true)
    {
        sendCapSenseButtonNotifications = true;
    }
    else if (writeRequest->handleValPair.value.val
             [CY_BLE_CAPSENSE_BUTTON_CAPSENSE_BUTTON_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX]
             == false)
    {
        sendCapSenseButtonNotifications = false;
    }

    /* Write the present CapSense notification status to the local variable */
    CapSenseButtonCCCDvalue[CCCD_INDEX_0] = sendCapSenseButtonNotifications;
    CapSenseButtonCCCDvalue[CCCD_INDEX_1] = CCCD_NULL;

    /* Update CCCD handle with notification status data */
    CapSenseButtonNotificationCCCDhandle.attrHandle 
    = CY_BLE_CAPSENSE_BUTTON_CAPSENSE_BUTTON_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE;
    CapSenseButtonNotificationCCCDhandle.value.val = CapSenseButtonCCCDvalue;
    CapSenseButtonNotificationCCCDhandle.value.len = CCCD_DATA_LEN;

    /* Report data to BLE component for sending data when read by the central
       device */
    attributeHandle.handleValuePair = CapSenseButtonNotificationCCCDhandle;
    attributeHandle.offset = CCCD_NULL;
    attributeHandle.connHandle = connectionHandle;
    attributeHandle.flags = CY_BLE_GATT_DB_PEER_INITIATED;
    Cy_BLE_GATTS_WriteAttributeValueCCCD(&attributeHandle);
}

/*******************************************************************************
* Function Name: void handleWriteRequestforRGB
*                     (cy_stc_ble_gatts_write_cmd_req_param_t *writeRequest)
********************************************************************************
* Summary:
*  This functions handles the 'write request' event for the RGB LED service
*
* Parameters:
*  writeRequest : pointer to the write request parameters from the central        
*
* Return:
*  void
*
*******************************************************************************/
void handleWriteRequestforRGB(cy_stc_ble_gatts_write_cmd_req_param_t *writeRequest)
{
    /* Local variable 'attributeHandle' stores attribute parameters*/
    cy_stc_ble_gatts_db_attr_val_info_t  attributeHandle;
    cy_stc_ble_gatt_handle_value_pair_t  RGBNotificationCCCDhandle;

    /* Local variable to store the current CCCD value */
    uint8_t                         RGBCCCDvalue[CCCD_DATA_LEN];

    /* Extract the Write value sent by the Client for RGB LED CCCD */
    if (writeRequest->handleValPair.value.val
        [CY_BLE_RGB_LED_RGB_LED_CONTROL_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX]
        == true)
    {
        RGBledNotifications = true;
    }
    else if (writeRequest->handleValPair.value.val
             [CY_BLE_RGB_LED_RGB_LED_CONTROL_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX]
             == false)
    {
        RGBledNotifications = false;
    }

    /* Write the present RGB notification status to the local variable */
    RGBCCCDvalue[CCCD_INDEX_0] = RGBledNotifications;
    RGBCCCDvalue[CCCD_INDEX_1] = CCCD_NULL;

    /* Update CCCD handle with notification status data*/
    RGBNotificationCCCDhandle.attrHandle 
    = CY_BLE_RGB_LED_RGB_LED_CONTROL_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE;
    RGBNotificationCCCDhandle.value.val = RGBCCCDvalue;
    RGBNotificationCCCDhandle.value.len = CCCD_DATA_LEN;

    /* Report data to BLE component for sending data when read by the central
       device */
    attributeHandle.handleValuePair = RGBNotificationCCCDhandle;
    attributeHandle.offset = CCCD_NULL;
    attributeHandle.connHandle = connectionHandle;
    attributeHandle.flags = CY_BLE_GATT_DB_PEER_INITIATED;
    Cy_BLE_GATTS_WriteAttributeValueCCCD(&attributeHandle);

    /* Update the RGB LED Notification attribute with new color coordinates */
    sendRGBNotification(RGBData);
}

/*******************************************************************************
* Function Name: void customEventHandler(uint32_t event, void *eventParameter)
********************************************************************************
* Summary:
*  Call back event function to handle various events from the BLE stack
*
* Parameters:
*  event            :	event returned
*  eventParameter   :	link to value of the events returned
*
* Return:
*  void
*
*******************************************************************************/
void customEventHandler(uint32_t event, void *eventParameter)
{
    /* Local variable to store the data received as part of the write request
       events */
    cy_stc_ble_gatts_write_cmd_req_param_t   *writeReqParameter;
    
    /* Take an action based on the current event */
    switch (event)
    {
        /* This event is received when the BLE stack is Started */
        case CY_BLE_EVT_STACK_ON:
            
            /* Set restartAdvertisement flag to allow calling Advertisement
               API from the main function */
            restartAdvertisement = true;
            break;

        /* ~~~~~~~~~~~~~~~~~~~~~~GAP EVENTS ~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
            
        /* If the current BLE state is Disconnected, then the Advertisement
           Start-Stop event implies that advertisement has stopped */
        case CY_BLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
            
            /* Check if the advertisement has stopped */
            if (Cy_BLE_GetState() == CY_BLE_STATE_STOPPED)
            {
                /* Set restartAdvertisement flag to allow calling Advertisement
                   API from the main function */
                restartAdvertisement = true;
            }
            break;
        
        /* This event is received when device is disconnected */
        case CY_BLE_EVT_GAP_DEVICE_DISCONNECTED:
            
            /* Set restartAdvertisement flag to allow calling Advertisement
             API from the main function */
            restartAdvertisement = true;
            break;

        /* ~~~~~~~~~~~~~~~~~~~~~~GATT EVENTS ~~~~~~~~~~~~~~~~~~~~~~~~~~*/
        
        /* This event is received when device is connected over GATT level */    
        case CY_BLE_EVT_GATT_CONNECT_IND:
            
            /* Update attribute handle on GATT Connection*/
            connectionHandle = *(cy_stc_ble_conn_handle_t *) eventParameter;

            /* This flag is used by the application to check the connection
               status */
            deviceConnected = true;
            break;
        
        /* This event is received when device is disconnected */
        case CY_BLE_EVT_GATT_DISCONNECT_IND:
           
            /* Update deviceConnected flag*/
            deviceConnected = false;
            
            /* Call the functions that handle the disconnect events for all 
               three custom services */
            handleDisconnectEventforSlider();
            handleDisconnectEventforButtons();
            handleDisconnectEventforRGB();
            break;
        
        /* This event is received when Central device sends a Write command
           on an Attribute */
        case CY_BLE_EVT_GATTS_WRITE_REQ:
            
            /* Read the write request parameter */
            writeReqParameter = (cy_stc_ble_gatts_write_cmd_req_param_t *) 
                                eventParameter;

            /* When this event is triggered, the peripheral has received a 
               write command on the custom  characteristic. Check if command
               fits any of the custom attributes and update the flag for
               sending notifications by the respective service */
            if (CY_BLE_CAPSENSE_SLIDER_CAPSENSE_SLIDER_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE
                == writeReqParameter->handleValPair.attrHandle)
            {
                handleWriteRequestforSlider(writeReqParameter);
            }
            if (CY_BLE_CAPSENSE_BUTTON_CAPSENSE_BUTTON_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE
               == writeReqParameter->handleValPair.attrHandle)
            {
                handleWriteRequestforButtons(writeReqParameter);
            }
            if (CY_BLE_RGB_LED_RGB_LED_CONTROL_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE
                == writeReqParameter->handleValPair.attrHandle)
            {
                handleWriteRequestforRGB(writeReqParameter);
            }
            /* Check if the returned handle is matching to RGB LED Control Write
               Attribute and extract the RGB data*/
            if (CY_BLE_RGB_LED_RGB_LED_CONTROL_CHAR_HANDLE 
                == writeReqParameter->handleValPair.attrHandle)
            {
                /* Extract the Write value sent by the Client for RGB LED Color 
                   characteristic */
                RGBData[RED_INDEX] = 
                        writeReqParameter->handleValPair.value.val[RED_INDEX];
                RGBData[GREEN_INDEX] = 
                        writeReqParameter->handleValPair.value.val[GREEN_INDEX];
                RGBData[BLUE_INDEX] = 
                        writeReqParameter->handleValPair.value.val[BLUE_INDEX];
                RGBData[INTENSITY_INDEX] = 
                        writeReqParameter->handleValPair.value.val[INTENSITY_INDEX];

                /* Update the the attribute for RGB LED read characteristics and
                   set the color of the LED per the received value */
                updateRGB();
            }
            /* Send the response to the write request received. */
            Cy_BLE_GATTS_WriteRsp(connectionHandle);
            break;
        
        /* This event is generated when the internal stack buffer is full and no
           more data can be accepted or the stack has buffer available and can 
           accept data. This event is used by application to prevent pushing lot
           of data to the BLE stack. */
        case CY_BLE_EVT_STACK_BUSY_STATUS:
            
            /* Extract the present stack status */
            busyStatus = *(uint8_t *) eventParameter;
            break;
        
        /* Do nothing for all other events */
        default:
            break;
    }
}

/* [] END OF FILE */
