#include "app_I2C.h"

/*******************************************************************************
* Function Name: handleI2CTraffic
********************************************************************************
* Summary:
*    This function handles the I2C read or write processing
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/

void handleI2CTraffic(void)
{
  sendI2CNotification();
}

/*******************************************************************************
* Function Name: sendI2CNotification
********************************************************************************
* Summary:
*    This function notifies the I2C data written by I2C master to the Client
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/

void sendI2CNotification(void)
{
	/* stores  notification data parameters */
	CYBLE_GATTS_HANDLE_VALUE_NTF_T		I2CHandle;	
	
	if(sendNotifications_BME280_Temp)
	{
		/* Package the notification data as part of I2C_read Characteristic*/
		I2CHandle.attrHandle = CYBLE_BME280_TEMP_BME280_TEMP_CHAR_HANDLE;				
		
		I2CHandle.value.val = wrBuf;
		
		I2CHandle.value.len = byteCnt;

	    /* Send the I2C_read Characteristic to the client only when notification is enabled */
		do
		{
		    apiResult = CyBle_GattsNotification(cyBle_connHandle,&I2CHandle);
			
			CyBle_ProcessEvents();
		
		} while((CYBLE_ERROR_OK != apiResult)  && (CYBLE_STATE_CONNECTED == cyBle_state));
		
	}		
}
/* [] END OF FILE */
