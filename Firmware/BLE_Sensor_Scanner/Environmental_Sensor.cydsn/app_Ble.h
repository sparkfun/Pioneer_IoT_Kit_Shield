/****************************************************************************
app_ble.h
BLE event handler header file
Mike Hord @ SparkFun Electronics
6 Dec 2017

This is the BLE event handler definition file. 

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

#ifndef _APP_BLE_H_
#define _APP_BLE_H_    
	
#include "main.h"

extern CYBLE_CONN_HANDLE_T ConnHandle;    
    
extern void AppCallBack(uint32 , void *);  
    
#endif /* _APP_BLE_H_ */


/* [] END OF FILE */
