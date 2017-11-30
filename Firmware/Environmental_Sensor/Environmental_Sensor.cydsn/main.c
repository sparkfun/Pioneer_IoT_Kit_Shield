/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include "main.h"
#include "bme280.h"

uint8 wrBuf[I2C_WRITE_BUFFER_SIZE];

uint8 sendNotifications_BME280_Temp;
uint8 sendNotifications_BME280_Pressure;
uint8 sendNotifications_BME280_Humidity;
CYBLE_API_RESULT_T	apiResult;  /*  variable to store BLE component API return */

/*******************************************************************************
* Function Name: main
********************************************************************************
* Summary:
*        System entrance point. This calls the initializing function and
* continuously process BLE and I2C events.
*
* Parameters:
*  void
*
* Return:
*  int
*

*******************************************************************************/
int main()
{	
	/* Enable the Global Interrupt */
	CyGlobalIntEnable;

	/* Turn off all LEDS */
	ALL_LED_OFF();
  
  // Initialize the BLE stack
  apiResult = CyBle_Start(AppCallBack);
	
	if(apiResult != CYBLE_ERROR_OK)
	{	
		// Failed to initialize stack, HACF
		DISCON_LED_ON();	
		while(1);
	}

	UART_Start();
  UART_UartPutString("Hello, world");
  I2C_Start();
  bme280Init();
  
	for(;;) /* Loop forever */
	{
    
    handleLowPowerMode();
    
		CyBle_ProcessEvents();
    
		sendI2CNotification();
		
	}
}


