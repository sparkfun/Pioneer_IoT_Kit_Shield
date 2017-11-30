#include "app_I2C.h"
#include "bme280.h"

void readI2CRegister(uint32 slaveAddress, uint32 regAddress, uint8* buff, uint32 cnt)
{
  I2C_I2CMasterSendStart(slaveAddress, I2C_I2C_WRITE_XFER_MODE, 1);
  I2C_I2CMasterWriteByte(regAddress, 1);
  I2C_I2CMasterSendStop(1);
  I2C_I2CMasterReadBuf(slaveAddress, buff, cnt, I2C_I2C_MODE_COMPLETE_XFER);
  while ((I2C_I2CMasterStatus() & I2C_I2C_MSTAT_RD_CMPLT) == 0);
}

void sendI2CNotification(void)
{
	/* stores  notification data parameters */
	CYBLE_GATTS_HANDLE_VALUE_NTF_T		I2CHandle;	
	
	if(sendNotifications_BME280_Temp)
	{
    bme280TempRead(wrBuf);
		/* Package the notification data as part of I2C_read Characteristic*/
		I2CHandle.attrHandle = CYBLE_BME280_TEMP_BME280_TEMP_CHAR_HANDLE;				
		
		I2CHandle.value.val = wrBuf;
		
		I2CHandle.value.len = 2;

	  do
		{
		  apiResult = CyBle_GattsNotification(cyBle_connHandle, &I2CHandle);
			CyBle_ProcessEvents();
		} while((CYBLE_ERROR_OK != apiResult)  && (CYBLE_STATE_CONNECTED == cyBle_state));
		
	}
  if(sendNotifications_BME280_Pressure)
	{
    bme280PresRead(wrBuf);
		/* Package the notification data as part of I2C_read Characteristic*/
		I2CHandle.attrHandle = CYBLE_BME280_PRESSURE_BME280_PRESSURE_CHAR_HANDLE;				
		
		I2CHandle.value.val = wrBuf;
		
		I2CHandle.value.len = 2;

		do
		{
		  apiResult = CyBle_GattsNotification(cyBle_connHandle, &I2CHandle);
			CyBle_ProcessEvents();
		} while((CYBLE_ERROR_OK != apiResult)  && (CYBLE_STATE_CONNECTED == cyBle_state));
		
	}	
  if(sendNotifications_BME280_Humidity)
	{
    bme280HumRead(wrBuf);
		/* Package the notification data as part of I2C_read Characteristic*/
		I2CHandle.attrHandle = CYBLE_BME280_HUMIDITY_BME280_HUMIDITY_CHAR_HANDLE;				
		
		I2CHandle.value.val = wrBuf;
		
		I2CHandle.value.len = 2;

		do
		{
		  apiResult = CyBle_GattsNotification(cyBle_connHandle, &I2CHandle);
			CyBle_ProcessEvents();
		} while((CYBLE_ERROR_OK != apiResult)  && (CYBLE_STATE_CONNECTED == cyBle_state));
		
	}	
}
/* [] END OF FILE */
