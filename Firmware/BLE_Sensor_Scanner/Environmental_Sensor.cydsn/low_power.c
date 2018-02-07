/****************************************************************************
i2c_support.c
I2C interface definition file

Function definitions for low power support

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

#include "low_power.h"

/*******************************************************************************
* Function Name: handleLowPowerMode
********************************************************************************
* Summary:
*        This functions puts the BLESS and the MCU core to the lowest possible
*		power state
*	     
* Parameters:
*	void
*
* Return:
*  	void
*
*******************************************************************************/
	
inline void handleLowPowerMode(void)
{
#ifdef LOW_POWER_MODE
	CYBLE_LP_MODE_T lpMode;
    CYBLE_BLESS_STATE_T blessState;
    uint8 interruptStatus;
	
	if(CyBle_GetState() != CYBLE_STATE_INITIALIZING)
    {
        /* Put BLE sub system in DeepSleep mode when it is idle */
        lpMode = CyBle_EnterLPM(CYBLE_BLESS_DEEPSLEEP);
        
       /* Disable global interrupts to avoid any other tasks from interrupting this section of code*/
        interruptStatus = CyEnterCriticalSection();
        
        /* Get current state of BLE sub system to check if it has successfully entered deep sleep state */
        blessState = CyBle_GetBleSsState();

        /* If BLE sub system has entered deep sleep, put chip into deep sleep for reducing power consumption */
        if(lpMode == CYBLE_BLESS_DEEPSLEEP)
        {   
            if(blessState == CYBLE_BLESS_STATE_ECO_ON || blessState == CYBLE_BLESS_STATE_DEEPSLEEP)
            {
               /* Put the chip into the deep sleep state as there are no pending tasks and BLE has also 
               ** successfully entered BLE DEEP SLEEP mode */
				#ifdef 	LED_INDICATION				
					ALL_LED_OFF();
				#endif	/* LED_INDICATION */			
				//I2C_Sleep();
               	
				CySysPmDeepSleep();
				
				//I2C_Wakeup();
				
#ifdef 	LED_INDICATION	
				if(CyBle_GetState() == CYBLE_STATE_CONNECTED)
				{
					CONNECT_LED_ON();
				}
				else
				{
					ADV_LED_ON();
				}
#endif	/* LED_INDICATION */
            }
        }
        /* BLE sub system has not entered deep sleep, wait for completion of radio operations */
        else if(blessState != CYBLE_BLESS_STATE_EVENT_CLOSE)
        {
            /* change HF clock source from IMO to ECO, as IMO can be stopped to save power */
            CySysClkWriteHfclkDirect(CY_SYS_CLK_HFCLK_ECO); 
            
			/* stop IMO for reducing power consumption */
            CySysClkImoStop(); 
            
			/* put the CPU to sleep */
            CySysPmSleep();
            
			/* starts execution after waking up, start IMO */
            CySysClkImoStart();
            
			/* change HF clock source back to IMO */
            CySysClkWriteHfclkDirect(CY_SYS_CLK_HFCLK_IMO);
        }
        /* Enable interrupts back */
        CyExitCriticalSection(interruptStatus);
    }
#endif /* LOW_POWER_MODE */
}

/* [] END OF FILE */
