#include "project.h"
#include "bme280.h"
#include "app_I2C.h"
#include <stdio.h>

uint16 dig_T1;
int16 dig_T2, dig_T3;

uint16 dig_P1;
int16 dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;

uint8 dig_H1, dig_H3;
int16 dig_H2, dig_H4, dig_H5;
int8 dig_H6;

void bme280Init()
{
  // Issue a reset to the BMP280 chip
  I2C_I2CMasterSendStart(0x77, I2C_I2C_WRITE_XFER_MODE, 2);
  I2C_I2CMasterWriteByte(0xe0, 1);
  I2C_I2CMasterWriteByte(0xb6, 1);
  I2C_I2CMasterSendStop(2);
  
  uint8 statusRegVal = 0;
  do
  {  
  I2C_I2CMasterSendStart(0x77, I2C_I2C_WRITE_XFER_MODE, 2);
  I2C_I2CMasterWriteByte(0xd0, 1);
  I2C_I2CMasterSendStop(2);
  
  I2C_I2CMasterSendStart(0x77, I2C_I2C_READ_XFER_MODE, 2);
  I2C_I2CMasterReadByte(0x03, &statusRegVal, 1);
  I2C_I2CMasterSendStop(2);
  } while (statusRegVal != 0x60);
  
  // Set up the ctrl_hum register for 1x oversampling
  I2C_I2CMasterSendStart(0x77, I2C_I2C_WRITE_XFER_MODE, 2);
  I2C_I2CMasterWriteByte(0xf2, 1);
  I2C_I2CMasterWriteByte(0b00000000, 1);
  I2C_I2CMasterSendStop(2);
  // Set up the ctrl_meas register for 1x oversampling & normal mode
  I2C_I2CMasterSendStart(0x77, I2C_I2C_WRITE_XFER_MODE, 2);
  I2C_I2CMasterWriteByte(0xf4, 1);
  I2C_I2CMasterWriteByte(0b10110111, 1);
  I2C_I2CMasterSendStop(2);
  
  // Now we need to fetch the calibration register values. These registers
  //  are not user writable and are stored in the memory on the BME280. Each
  //  BME280 read fetches raw data, then converts that raw data into temp,
  //  pressure, or humidity using these calibration values.
  
  // Start with temperature.
  uint8 tempBuff[2];
  readI2CRegister(0x77, 0x88, (uint8*)&dig_T1, 2);
  readI2CRegister(0x77, 0x8A, (uint8*)&dig_T2, 2);
  readI2CRegister(0x77, 0x8C, (uint8*)&dig_T3, 2);
}

void bme280TempRead(uint8 *buff)
{
  /*I2C_I2CMasterSendStart(0x77, I2C_I2C_WRITE_XFER_MODE, 2);
  I2C_I2CMasterWriteByte(0xfa, 1);
  I2C_I2CMasterSendStop(2);
  I2C_I2CMasterReadBuf(0x77, buff, 2, I2C_I2C_MODE_COMPLETE_XFER);
  while ((I2C_I2CMasterStatus() & I2C_I2C_MSTAT_RD_CMPLT) == 0);
  return;*/
  
  uint32 tempRaw, msb, lsb, xlsb;
  uint8 tempBuff[3];
  readI2CRegister(0x77, 0xFA, tempBuff, 3);
  msb = tempBuff[0]<<12;
  lsb = tempBuff[1]<<4;
  xlsb = tempBuff[2]>>4;
  tempRaw = msb | lsb | xlsb;
  char buffer[5];
  int32 temp = BME280_compensate_T_int32(tempRaw);
  memcpy(buff, &temp, 4);
  sprintf(buffer, "%ld", temp);
  UART_UartPutString(buffer);
  UART_UartPutCRLF(0x20);
}

void bme280PresRead(uint8 *buff)
{  
  I2C_I2CMasterSendStart(0x77, I2C_I2C_WRITE_XFER_MODE, 2);
  I2C_I2CMasterWriteByte(0xf7, 1);
  I2C_I2CMasterSendStop(2);
  I2C_I2CMasterReadBuf(0x77, buff, 2, I2C_I2C_MODE_COMPLETE_XFER);
  while ((I2C_I2CMasterStatus() & I2C_I2C_MSTAT_RD_CMPLT) == 0);
}

void bme280HumRead(uint8 *buff)
{  
  I2C_I2CMasterSendStart(0x77, I2C_I2C_WRITE_XFER_MODE, 2);
  I2C_I2CMasterWriteByte(0xfd, 1);
  I2C_I2CMasterSendStop(2);
  I2C_I2CMasterReadBuf(0x77, buff, 2, I2C_I2C_MODE_COMPLETE_XFER);
  while ((I2C_I2CMasterStatus() & I2C_I2C_MSTAT_RD_CMPLT) == 0);
}

// The following three functions are taken directly from the BME280 datasheet, pp23-24.

// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
// t_fine carries fine temperature as global value
BME280_S32_t t_fine;
BME280_S32_t BME280_compensate_T_int32(BME280_S32_t adc_T)
{
  BME280_S32_t var1, var2, T;
  var1 = ((((adc_T>>3) - ((BME280_S32_t)dig_T1<<1))) * ((BME280_S32_t)dig_T2)) >> 11;
  var2 = (((((adc_T>>4) - ((BME280_S32_t)dig_T1)) * ((adc_T>>4) - ((BME280_S32_t)dig_T1))) >> 12) *
    ((BME280_S32_t)dig_T3)) >> 14;
  t_fine = var1 + var2;
  T = (t_fine * 5 + 128) >> 8;
  return T;
}
// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
// Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
BME280_U32_t BME280_compensate_P_int64(BME280_S32_t adc_P)
{
BME280_S64_t var1, var2, p;
var1 = ((BME280_S64_t)t_fine) - 128000;
var2 = var1 * var1 * (BME280_S64_t)dig_P6;
var2 = var2 + ((var1*(BME280_S64_t)dig_P5)<<17);
var2 = var2 + (((BME280_S64_t)dig_P4)<<35);
var1 = ((var1 * var1 * (BME280_S64_t)dig_P3)>>8) + ((var1 * (BME280_S64_t)dig_P2)<<12);
var1 = (((((BME280_S64_t)1)<<47)+var1))*((BME280_S64_t)dig_P1)>>33;
if (var1 == 0)
{
return 0; // avoid exception caused by division by zero
}
p = 1048576-adc_P;
p = (((p<<31)-var2)*3125)/var1;
var1 = (((BME280_S64_t)dig_P9) * (p>>13) * (p>>13)) >> 25;
var2 = (((BME280_S64_t)dig_P8) * p) >> 19;
p = ((p + var1 + var2) >> 8) + (((BME280_S64_t)dig_P7)<<4);
return (BME280_U32_t)p;
}
// Returns humidity in %RH as unsigned 32 bit integer in Q22.10 format (22 integer and 10 fractional bits).
// Output value of “47445” represents 47445/1024 = 46.333 %RH
BME280_U32_t bme280_compensate_H_int32(BME280_S32_t adc_H)
{
  BME280_S32_t v_x1_u32r;
  v_x1_u32r = (t_fine - ((BME280_S32_t)76800));
  v_x1_u32r = (((((adc_H << 14) - (((BME280_S32_t)dig_H4) << 20) - (((BME280_S32_t)dig_H5) * v_x1_u32r)) +
    ((BME280_S32_t)16384)) >> 15) * (((((((v_x1_u32r * ((BME280_S32_t)dig_H6)) >> 10) * (((v_x1_u32r * 
    ((BME280_S32_t)dig_H3)) >> 11) + ((BME280_S32_t)32768))) >> 10) + ((BME280_S32_t)2097152)) *
    ((BME280_S32_t)dig_H2) + 8192) >> 14));
  v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((BME280_S32_t)dig_H1)) >> 4));
  v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
  v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
  return (BME280_U32_t)(v_x1_u32r>>12);
}
