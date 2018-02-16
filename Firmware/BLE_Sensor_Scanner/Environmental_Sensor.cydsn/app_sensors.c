/****************************************************************************
app_sensors.c
Sensor reading definition file
Mike Hord @ SparkFun Electronics
6 Dec 2017

This is the sensor reading definition file. Handles reading sensors and
sending notification (if necessary) to BLE Central Client device.

It was derived from project #35 in the "100 projects in 100 days" series on 
Cypress.com. More info at
http://www.cypress.com/blog/100-projects-100-days?page=3

Development environment specifics:
Originally developed in PSoC Creator 4.1
Targets CY8CKIT-042-BLE development board with an IoT Shield and SparkFun
  Environmental Combo QWIIC Board

This code is beerware; if you see me (or any other SparkFun employee) at the
local, and you've found our code helpful, please buy us a round!
****************************************************************************/

#include "app_sensors.h"
#include "bme280.h"
#include "ccs811.h"
#include <stdio.h>

extern struct bme280_dev bme280;
extern struct ccs811_dev ccs811;

static uint8_t resettedCCS811 = 0;

uint8 sendNotifications_BME280_Temp;
uint8 sendNotifications_BME280_Pressure;
uint8 sendNotifications_BME280_Humidity;
uint8 sendNotifications_CCS811_TVOC;
uint8 sendNotifications_CCS811_ECO2;

void sendSensorNotification(void)
{
	/* stores  notification data parameters */
	CYBLE_GATTS_HANDLE_VALUE_NTF_T		I2CHandle;
  struct bme280_data bme280_data;
  struct ccs811_data ccs811_data;
  bme280_get_sensor_data(BME280_ALL, &bme280_data, &bme280);
  ccs811_get_sensor_data(&ccs811_data, &ccs811);
  
  // Sometimes, the CCS811 goes out to lunch and starts returning
  //  65021 (0xFDFD) for one or both values. If this happens, we
  //  need to re-init the part.
  if (ccs811_data.ECO2 == 65021 || ccs811_data.TVOC == 65021)
  {
    ccs811_init(&ccs811);
    ccs811_get_sensor_data(&ccs811_data, &ccs811);
    UART_UartPutString("Reset CCS811\n");
    resettedCCS811 = 1;
  }
  
  if (resettedCCS811)
  {
    UART_UartPutString("Did the thing\n");
  }
  
  char buffer[20];
  
	if(sendNotifications_BME280_Temp)
	{
    sprintf(buffer, "temp: %u\n", bme280_data.temperature);
    UART_UartPutString(buffer);
    while (UART_SpiUartGetTxBufferSize() > 0);
		/* Package the notification data as part of I2C_read Characteristic*/
		I2CHandle.attrHandle = CYBLE_BME280_TEMP_BME280_TEMP_CHAR_HANDLE;				
		
		I2CHandle.value.val = (unsigned char*)&bme280_data.temperature;
		
		I2CHandle.value.len = 4;

	  do
		{
		  apiResult = CyBle_GattsNotification(cyBle_connHandle, &I2CHandle);
			CyBle_ProcessEvents();
		} while((CYBLE_ERROR_OK != apiResult)  && (CYBLE_STATE_CONNECTED == cyBle_state));
		
	}
  if(sendNotifications_BME280_Pressure)
	{
    sprintf(buffer, "pres %u\n", bme280_data.pressure);
    UART_UartPutString(buffer);
    while (UART_SpiUartGetTxBufferSize() > 0);
		/* Package the notification data as part of I2C_read Characteristic*/
		I2CHandle.attrHandle = CYBLE_BME280_PRESSURE_BME280_PRESSURE_CHAR_HANDLE;				
		
		I2CHandle.value.val = (unsigned char*)&bme280_data.pressure;
		
		I2CHandle.value.len = 4;

		do
		{
		  apiResult = CyBle_GattsNotification(cyBle_connHandle, &I2CHandle);
			CyBle_ProcessEvents();
		} while((CYBLE_ERROR_OK != apiResult)  && (CYBLE_STATE_CONNECTED == cyBle_state));
		
	}	
  if(sendNotifications_BME280_Humidity)
	{
    sprintf(buffer, "hum %u\n", bme280_data.humidity);
    UART_UartPutString(buffer);
    while (UART_SpiUartGetTxBufferSize() > 0);
		/* Package the notification data as part of I2C_read Characteristic*/
		I2CHandle.attrHandle = CYBLE_BME280_HUMIDITY_BME280_HUMIDITY_CHAR_HANDLE;				
		
		I2CHandle.value.val = (unsigned char*)&bme280_data.humidity;
		
		I2CHandle.value.len = 4;

		do
		{
		  apiResult = CyBle_GattsNotification(cyBle_connHandle, &I2CHandle);
			CyBle_ProcessEvents();
		} while((CYBLE_ERROR_OK != apiResult)  && (CYBLE_STATE_CONNECTED == cyBle_state));
  } 
  if(sendNotifications_CCS811_ECO2)
	{
    sprintf(buffer, "ECO2 %d\n", ccs811_data.ECO2);
    UART_UartPutString(buffer);
    while (UART_SpiUartGetTxBufferSize() > 0);
		/* Package the notification data as part of I2C_read Characteristic*/
		I2CHandle.attrHandle = CYBLE_CCS811_ECO2_CCS811_ECO2_CHAR_HANDLE;				
		
		I2CHandle.value.val = (unsigned char*)&ccs811_data.ECO2;
		
		I2CHandle.value.len = 2;

		do
		{
		  apiResult = CyBle_GattsNotification(cyBle_connHandle, &I2CHandle);
			CyBle_ProcessEvents();
		} while((CYBLE_ERROR_OK != apiResult)  && (CYBLE_STATE_CONNECTED == cyBle_state));
	}	
  if(sendNotifications_CCS811_TVOC)
	{
    sprintf(buffer, "tVOC %d\n", ccs811_data.TVOC);
    UART_UartPutString(buffer);
    while (UART_SpiUartGetTxBufferSize() > 0);
		/* Package the notification data as part of I2C_read Characteristic*/
		I2CHandle.attrHandle = CYBLE_CCS811_TVOC_CCS811_TVOC_CHAR_HANDLE;				
		
		I2CHandle.value.val = (unsigned char*)&ccs811_data.TVOC;
		
		I2CHandle.value.len = 2;

		do
		{
		  apiResult = CyBle_GattsNotification(cyBle_connHandle, &I2CHandle);
			CyBle_ProcessEvents();
		} while((CYBLE_ERROR_OK != apiResult)  && (CYBLE_STATE_CONNECTED == cyBle_state));
	}

}
/* [] END OF FILE */
