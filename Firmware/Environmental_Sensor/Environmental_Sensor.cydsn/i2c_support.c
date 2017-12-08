/****************************************************************************
i2c_support.c
I2C interface definition file
Mike Hord @ SparkFun Electronics
6 Dec 2017

Function definitions for I2C support

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
#include "i2c_support.h"

// Function to read arbitrary number of registers from an I2C device. This
//  is designed to plug directly into the CCS811 and BME280 drivers.
signed char readI2CRegister(unsigned char slaveAddress, unsigned char regAddress, unsigned char* buff, unsigned short cnt)
{
  I2C_I2CMasterSendStart(slaveAddress, I2C_I2C_WRITE_XFER_MODE, 1);
  I2C_I2CMasterWriteByte(regAddress, 1);
  I2C_I2CMasterSendStop(1);
  I2C_I2CMasterReadBuf(slaveAddress, buff, cnt, I2C_I2C_MODE_COMPLETE_XFER);
  while ((I2C_I2CMasterStatus() & I2C_I2C_MSTAT_RD_CMPLT) == 0);
  return BME280_OK;
}

// Function to write arbitrary number of registers to an I2C device. This
//  is designed to plug directly into the CCS811 and BME280 drivers.
signed char writeI2CRegister(unsigned char slaveAddress, unsigned char regAddress, unsigned char* buff, unsigned short cnt)
{
  unsigned short i = 0;
  I2C_I2CMasterSendStart(slaveAddress, I2C_I2C_WRITE_XFER_MODE, 1);
  I2C_I2CMasterWriteByte(regAddress, 1);
  for (i = 0; i < cnt; i++)
  {
    I2C_I2CMasterWriteByte(buff[i], 1);
  }
  I2C_I2CMasterSendStop(1);
  return BME280_OK;
}