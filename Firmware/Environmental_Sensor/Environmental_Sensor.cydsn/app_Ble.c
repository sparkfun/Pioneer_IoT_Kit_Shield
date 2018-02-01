/****************************************************************************
app_ble.c
BLE event handler
Mike Hord @ SparkFun Electronics
6 Dec 2017

This is the BLE event handler definition file. It was derived from project #35
in the "100 projects in 100 days" series on Cypress.com. More info at
http://www.cypress.com/blog/100-projects-100-days?page=3

Development environment specifics:
Originally developed in PSoC Creator 4.1
Targets CY8CKIT-042-BLE development board with an IoT Shield and SparkFun
  Environmental Combo QWIIC Board

This code is beerware; if you see me (or any other SparkFun employee) at the
local, and you've found our code helpful, please buy us a round!
****************************************************************************/

#include "app_Ble.h"

extern uint8 sendNotifications_BME280_Temp;
extern uint8 sendNotifications_BME280_Pressure;
extern uint8 sendNotifications_BME280_Humidity;
extern uint8 sendNotifications_CCS811_TVOC;
extern uint8 sendNotifications_CCS811_ECO2;

void AppCallBack(uint32 event, void *eventParam)
{ 
	CYBLE_GATTS_WRITE_REQ_PARAM_T *wrReqParam;
    
   	switch (event)
   	{
	    case CYBLE_EVT_STACK_ON:
			/* start advertising */
	      apiResult = CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
  			if(apiResult == CYBLE_ERROR_OK)
  			{
  				ADV_LED_ON(); 
  			}
	    break;

	    case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
        sendNotifications_BME280_Temp = 0;
  			sendNotifications_BME280_Pressure = 0;
        sendNotifications_BME280_Humidity = 0;
        sendNotifications_CCS811_TVOC = 0;
        sendNotifications_CCS811_ECO2 = 0;
  			
  			DISCON_LED_ON();
  			CyDelay(3000); 

  			/* start advertising */
  	    apiResult = CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
  			
  			if(apiResult == CYBLE_ERROR_OK)
  			{	
  				ADV_LED_ON(); 
  			}	
	    break;    
     
	    case CYBLE_EVT_GATT_CONNECT_IND:
  			CONNECT_LED_ON();
  	  break;
        
	    /* Client may do Write Value or Write Value without Response. Handle both */   
	    case CYBLE_EVT_GATTS_WRITE_REQ:
	    case CYBLE_EVT_GATTS_WRITE_CMD_REQ:
	      wrReqParam = (CYBLE_GATTS_WRITE_REQ_PARAM_T *) eventParam;
			
	        /* Handling Notification Enable */
			  if(wrReqParam->handleValPair.attrHandle == CYBLE_BME280_TEMP_BME280_TEMP_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE)
	        {
    				CYBLE_GATT_HANDLE_VALUE_PAIR_T    SensorNotificationCCDHandle;
    				uint8 SensorCCDValue[2];
			
            /* Extract CCD Notification enable flag */
            sendNotifications_BME280_Temp = wrReqParam->handleValPair.value.val[0];
				
    				/* Write the present sensor notification status to the local variable */
    				SensorCCDValue[0] = sendNotifications_BME280_Temp;
    				
    				SensorCCDValue[1] = 0x00;
    				
    				/* Update CCD handle with notification status data*/
    				SensorNotificationCCDHandle.attrHandle = CYBLE_BME280_TEMP_BME280_TEMP_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE;
    				
    				SensorNotificationCCDHandle.value.val = SensorCCDValue;
    				
    				SensorNotificationCCDHandle.value.len = 2;
    				
    				/* Report data to BLE component for sending data when read by Central device */
    				CyBle_GattsWriteAttributeValue(&SensorNotificationCCDHandle, 0, &cyBle_connHandle, CYBLE_GATT_DB_LOCALLY_INITIATED);			
	        }			  
          if(wrReqParam->handleValPair.attrHandle == CYBLE_BME280_PRESSURE_BME280_PRESSURE_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE)
	        {
    				CYBLE_GATT_HANDLE_VALUE_PAIR_T    SensorNotificationCCDHandle;
    				uint8 SensorCCDValue[2];
			
            /* Extract CCD Notification enable flag */
            sendNotifications_BME280_Pressure = wrReqParam->handleValPair.value.val[0];
				
    				/* Write the present sensor notification status to the local variable */
    				SensorCCDValue[0] = sendNotifications_BME280_Pressure;
    				
    				SensorCCDValue[1] = 0x00;
    				
    				/* Update CCD handle with notification status data*/
    				SensorNotificationCCDHandle.attrHandle = CYBLE_BME280_PRESSURE_BME280_PRESSURE_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE;
    				
    				SensorNotificationCCDHandle.value.val = SensorCCDValue;
    				
    				SensorNotificationCCDHandle.value.len = 2;
    				
    				/* Report data to BLE component for sending data when read by Central device */
    				CyBle_GattsWriteAttributeValue(&SensorNotificationCCDHandle, 0, &cyBle_connHandle, CYBLE_GATT_DB_LOCALLY_INITIATED);			
	        }
          if(wrReqParam->handleValPair.attrHandle == CYBLE_BME280_HUMIDITY_BME280_HUMIDITY_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE)
	        {
    				CYBLE_GATT_HANDLE_VALUE_PAIR_T    SensorNotificationCCDHandle;
    				uint8 SensorCCDValue[2];
			
            /* Extract CCD Notification enable flag */
            sendNotifications_BME280_Humidity = wrReqParam->handleValPair.value.val[0];
				
    				/* Write the present sensor notification status to the local variable */
    				SensorCCDValue[0] = sendNotifications_BME280_Humidity;
    				
    				SensorCCDValue[1] = 0x00;
    				
    				/* Update CCD handle with notification status data*/
    				SensorNotificationCCDHandle.attrHandle = CYBLE_BME280_HUMIDITY_BME280_HUMIDITY_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE;
    				
    				SensorNotificationCCDHandle.value.val = SensorCCDValue;
    				
    				SensorNotificationCCDHandle.value.len = 2;
    				
    				/* Report data to BLE component for sending data when read by Central device */
    				CyBle_GattsWriteAttributeValue(&SensorNotificationCCDHandle, 0, &cyBle_connHandle, CYBLE_GATT_DB_LOCALLY_INITIATED);			
	        }
          if(wrReqParam->handleValPair.attrHandle == CYBLE_CCS811_TVOC_CCS811_TVOC_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE)
	        {
    				CYBLE_GATT_HANDLE_VALUE_PAIR_T    SensorNotificationCCDHandle;
    				uint8 SensorCCDValue[2];
			
            /* Extract CCD Notification enable flag */
            sendNotifications_CCS811_TVOC = wrReqParam->handleValPair.value.val[0];
				
    				/* Write the present sensor notification status to the local variable */
    				SensorCCDValue[0] = sendNotifications_CCS811_TVOC;
    				
    				SensorCCDValue[1] = 0x00;
    				
    				/* Update CCD handle with notification status data*/
    				SensorNotificationCCDHandle.attrHandle = CYBLE_CCS811_TVOC_CCS811_TVOC_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE;
    				
    				SensorNotificationCCDHandle.value.val = SensorCCDValue;
    				
    				SensorNotificationCCDHandle.value.len = 2;
    				
    				/* Report data to BLE component for sending data when read by Central device */
    				CyBle_GattsWriteAttributeValue(&SensorNotificationCCDHandle, 0, &cyBle_connHandle, CYBLE_GATT_DB_LOCALLY_INITIATED);			
	        }	  
          if(wrReqParam->handleValPair.attrHandle == CYBLE_CCS811_ECO2_CCS811_ECO2_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE)
	        {
    				CYBLE_GATT_HANDLE_VALUE_PAIR_T    SensorNotificationCCDHandle;
    				uint8 SensorCCDValue[2];
			
            /* Extract CCD Notification enable flag */
            sendNotifications_CCS811_ECO2 = wrReqParam->handleValPair.value.val[0];
				
    				/* Write the present sensor notification status to the local variable */
    				SensorCCDValue[0] = sendNotifications_CCS811_ECO2;
    				
    				SensorCCDValue[1] = 0x00;
    				
    				/* Update CCD handle with notification status data*/
    				SensorNotificationCCDHandle.attrHandle = CYBLE_CCS811_ECO2_CCS811_ECO2_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE;
    				
    				SensorNotificationCCDHandle.value.val = SensorCCDValue;
    				
    				SensorNotificationCCDHandle.value.len = 2;
    				
    				/* Report data to BLE component for sending data when read by Central device */
    				CyBle_GattsWriteAttributeValue(&SensorNotificationCCDHandle, 0, &cyBle_connHandle, CYBLE_GATT_DB_LOCALLY_INITIATED);			
	        }	 
	        if (event == CYBLE_EVT_GATTS_WRITE_REQ)
			    {
	          CyBle_GattsWriteRsp(cyBle_connHandle);
			    }
	        break;
     
		default:
			break;
    }
}


/* [] END OF FILE */
