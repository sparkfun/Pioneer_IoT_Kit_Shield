/****************************************************************************
ccs811.c
CCS811 interface definition file
Mike Hord @ SparkFun Electronics
6 Dec 2017

Implementation of interface for CCS811 sensors.

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
  
#include "ccs811.h"
#include "project.h"

// Starts the CCS811 sensor operation by checking to make sure it's present
//  and then writing to the APP_START register.
int8_t ccs811_init(const struct ccs811_dev *dev)
{
  uint8_t data = 0;
  dev->read(dev->dev_addr, CCS811_HW_ID, &data, 1);
  if (data != dev->chip_id) 
  {
    return -1;
  }
  dev->write(dev->dev_addr, CCS811_APP_START, &data, 0);
  return 0;
}

// Write the sensor settings present in the settings member of the device
//  struct to the sensor.
int8_t ccs811_set_sensor_settings(const struct ccs811_dev *dev)
{
  uint8_t driveMode = dev->settings.driveMode << 4;
  dev->write(dev->dev_addr, CCS811_MEAS_MODE, &driveMode, 1);
  return 0;
}

// Get the settings from the sensor and put them into the settings member of
//  the device struct.
int8_t ccs811_get_sensor_settings(struct ccs811_dev *dev)
{
  uint8_t data = 0;
  dev->read(dev->dev_addr, CCS811_MEAS_MODE, &data, 1);
  dev->settings.driveMode = data>>4 & 0xF8;
  return 0;
}

// Using the read, write, and delay functions set up in the config section,
//  fetch the sensor data from the sensor and put it in the data struct.
int8_t ccs811_get_sensor_data(struct ccs811_data *data, struct ccs811_dev *dev)
{
  uint8_t results[4] = {0};
  dev->read(dev->dev_addr, CCS811_ALG_RESULT_DATA, results, 4);
  data->ECO2 = (results[0]<<8) | (results[1]);
  data->TVOC = (results[2]<<8) | (results[3]);
  return 0;
}
