/****************************************************************************
sensors_config.c
Sensor configuration function definition file
Mike Hord @ SparkFun Electronics
6 Dec 2017

Function definitions for configuring the sensors for use.

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

#include "project.h"
#include "bme280.h"
#include "sensors_config.h"
#include "app_sensors.h"
#include "i2c_support.h"

void bme280_config(struct bme280_dev *bme280)
{
  bme280->chip_id = 0x60;  // This is the "WHO AM I" value of the chip
  bme280->dev_id = 0x77;   // This is the I2C address of the chip
  bme280->read = readI2CRegister;
  bme280->write = writeI2CRegister;
  bme280->delay_ms = CyDelay;
  bme280->intf = BME280_I2C_INTF;
  // Set 16x oversampling for all three values the BME280 measures
  bme280->settings.osr_h = BME280_OVERSAMPLING_16X;
  bme280->settings.osr_p = BME280_OVERSAMPLING_16X;
  bme280->settings.osr_t = BME280_OVERSAMPLING_16X;
  // Initialize the BME280
  bme280_init(bme280);
  //  Upload the various settings to the chip
  bme280_set_sensor_settings(BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL, bme280);
  // Set the sensor to automatically capture samples periodically
  bme280_set_sensor_mode(BME280_NORMAL_MODE, bme280);
}

void ccs811_config(struct ccs811_dev *ccs811)
{
  ccs811->dev_addr = 0x5B;
  ccs811->chip_id = 0x81;
  ccs811->read = readI2CRegister;
  ccs811->write = writeI2CRegister;
  ccs811->delay_ms = CyDelay;
  ccs811->settings.driveMode = MODE_1; // one sample per second
  ccs811_set_sensor_settings(ccs811);
  ccs811_init(ccs811);
}
