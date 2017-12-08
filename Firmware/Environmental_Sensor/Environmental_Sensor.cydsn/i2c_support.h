/****************************************************************************
i2c_support.h
I2C interface header file
Mike Hord @ SparkFun Electronics
6 Dec 2017

Defines and declarations for the I2C interface functions. 

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
  
#ifndef __I2C_SUPPORT_H__
#define __I2C_SUPPORT_H__

signed char readI2CRegister(unsigned char slaveAddress, unsigned char regAddress, unsigned char* buff, unsigned short cnt);
signed char writeI2CRegister(unsigned char slaveAddress, unsigned char regAddress, unsigned char* buff, unsigned short cnt);

#endif
