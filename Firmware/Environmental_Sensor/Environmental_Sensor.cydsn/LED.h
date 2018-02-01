/****************************************************************************
LED.h
LED macro header file
Mike Hord @ SparkFun Electronics
6 Dec 2017

Macros for turning LEDs on and off according to function rather than pin. 

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
  
#ifndef _LED_H_
#define _LED_H_
	
#include "project.h"

	
#define DISCON_LED_ON()		{ DISCON_LED_Write(0); CONNECT_LED_Write(1); ADV_LED_Write(1);}	
#define CONNECT_LED_ON()	{ DISCON_LED_Write(1); CONNECT_LED_Write(0); ADV_LED_Write(1);}	
#define ADV_LED_ON()		{ DISCON_LED_Write(1); CONNECT_LED_Write(1); ADV_LED_Write(0);}	
#define ALL_LED_OFF()   	{ DISCON_LED_Write(1); CONNECT_LED_Write(1); ADV_LED_Write(1);}	
	
#endif	/* _LED_H_ */


/* [] END OF FILE */
