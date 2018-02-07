/****************************************************************************
main.h
Primary includes and defines file for the project
Mike Hord @ SparkFun Electronics
6 Dec 2017

This is the main() header file. It was derived from project #35
in the "100 projects in 100 days" series on Cypress.com. More info at
http://www.cypress.com/blog/100-projects-100-days?page=3

Development environment specifics:
Originally developed in PSoC Creator 4.1
Targets CY8CKIT-042-BLE development board with an IoT Shield and SparkFun
  Environmental Combo QWIIC Board

This code is beerware; if you see me (or any other SparkFun employee) at the
local, and you've found our code helpful, please buy us a round!
****************************************************************************/
  
#ifndef _MAIN_H_
#define _MAIN_H_

#include "project.h"
#include "config.h"
#include "app_Ble.h"
#include "app_sensors.h"
#include "LED.h"
#include "low_power.h"
	
extern	CYBLE_API_RESULT_T	apiResult;	
            
#endif	/* _MAIN_H_ */

/* [] END OF FILE */
