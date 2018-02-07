/****************************************************************************
main.c
Main function
Mike Hord @ SparkFun Electronics
6 Dec 2017

This is the main() function definition file. It was derived from project #35
in the "100 projects in 100 days" series on Cypress.com. More info at
http://www.cypress.com/blog/100-projects-100-days?page=3

Development environment specifics:
Originally developed in PSoC Creator 4.1
Targets CY8CKIT-042-BLE development board with an IoT Shield and SparkFun
  Environmental Combo QWIIC Board

This code is beerware; if you see me (or any other SparkFun employee) at the
local, and you've found our code helpful, please buy us a round!
****************************************************************************/
#include "main.h"
#include "bme280.h"
#include "sensors_config.h"

// Program-wide buffer for data to be written via BLE
uint8 wrBuf[WRITE_BUFFER_SIZE];

// Program-wide device objects for BME280 and CCS811
struct bme280_dev bme280;
struct ccs811_dev ccs811;

// Program-wide result storage for BLE API calls.
CYBLE_API_RESULT_T	apiResult;

int main()
{	
    uint32_t delayCnt = 0;
	// Enable the Global Interrupt
	CyGlobalIntEnable;

	// Turn off all LEDS
	ALL_LED_OFF();
  
  // Initialize the BLE stack
  apiResult = CyBle_Start(AppCallBack);
	
	if(apiResult != CYBLE_ERROR_OK)
	{	
		// Failed to initialize stack, HACF
		DISCON_LED_ON();	
		while(1);
	}

  // Initialize the hardware modules used in the system
	UART_Start();
  I2C_Start();
  
  UART_UartPutString("Env Sensor Example\n");
  
  CyDelay(500); // CCS811 wants a brief delay before it starts up
  
  // Configure and enable the sensors
  bme280_config(&bme280);
  ccs811_config(&ccs811);
  
  // Loop forever
	for(;;)
	{
    
    handleLowPowerMode();
    
		CyBle_ProcessEvents();
        CyDelay(10);
        if(delayCnt++ >= 100)
        {
		    sendSensorNotification();
            delayCnt = 0;
        }
		
	}
}


