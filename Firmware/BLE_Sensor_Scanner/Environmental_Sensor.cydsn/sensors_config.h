/****************************************************************************
sensors_config.h
Sensor configuration header file
Mike Hord @ SparkFun Electronics
6 Dec 2017

Defines and declarations for the sensor configuration functions. 

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
  
#ifndef __SENSORS_CONFIG_H__
#define __SENSORS_CONFIG_H__

#include "bme280.h"
#include "ccs811.h"
  
void bme280_config(struct bme280_dev *bme280);
void ccs811_config(struct ccs811_dev *ccs811);
  
#endif