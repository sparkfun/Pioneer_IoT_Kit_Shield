/****************************************************************************
ccs811.h
CCS811 interface header file
Mike Hord @ SparkFun Electronics
6 Dec 2017

Defines and declarations for the CCS811 sensor. 

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
  
#ifndef __ccs811_h__
#define __ccs811_h__

#include <stdint.h>
  
//Register addresses
#define CCS811_STATUS 0x00
#define CCS811_MEAS_MODE 0x01
#define CCS811_ALG_RESULT_DATA 0x02
#define CCS811_RAW_DATA 0x03
#define CCS811_ENV_DATA 0x05
#define CCS811_NTC 0x06
#define CCS811_THRESHOLDS 0x10
#define CCS811_BASELINE 0x11
#define CCS811_HW_ID 0x20
#define CCS811_HW_VERSION 0x21
#define CCS811_FW_BOOT_VERSION 0x23
#define CCS811_FW_APP_VERSION 0x24
#define CCS811_ERROR_ID 0xE0
#define CCS811_APP_START 0xF4
#define CCS811_SW_RESET 0xFF
  
typedef int8_t (*ccs811_com_fptr_t)(uint8_t dev_id, uint8_t reg_addr,
		uint8_t *data, uint16_t len);

typedef void (*ccs811_delay_fptr_t)(uint32_t period);

enum DRIVE_MODE {
  IDLE,
  MODE_1,
  MODE_2,
  MODE_3,
  MODE_4
};

struct __attribute__((__packed__)) ccs811_data {
  uint16_t ECO2;
  uint16_t TVOC;
  uint32_t data_reserved;
};

struct ccs811_settings {
  enum DRIVE_MODE driveMode;
};

struct ccs811_dev {
  uint8_t chip_id;
  uint8_t dev_addr;
	ccs811_com_fptr_t read;
	ccs811_com_fptr_t write;
	ccs811_delay_fptr_t delay_ms;
	struct ccs811_settings settings;
};
  
// Function prototypes
int8_t ccs811_init(struct ccs811_dev *dev);
int8_t ccs811_set_sensor_settings(const struct ccs811_dev *dev);
int8_t ccs811_get_sensor_settings(struct ccs811_dev *dev);
int8_t ccs811_get_sensor_data(struct ccs811_data *data, struct ccs811_dev *dev);

#endif
  