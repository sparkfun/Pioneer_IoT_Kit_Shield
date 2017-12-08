/****************************************************************************
low_power.h
Low power mode header file

Defines and declarations in support of low power mode operation. 

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

#ifndef _LOW_POWER_H_
#define _LOW_POWER_H_
	
#include "config.h"
#include "LED.h"	
	
#ifdef	LOW_POWER_MODE
	
extern inline void handleLowPowerMode(void); 	

#endif	/* LOW_POWER_MODE */
	 
#endif	/* _LOW_POWER_H_ */

/* [] END OF FILE */
