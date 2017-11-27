#include "project.h"
#include "bme280.h"

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
  
  // Set up the ctrl_meas register for 1x oversampling & normal mode
  I2C_I2CMasterSendStart(0x77, I2C_I2C_WRITE_XFER_MODE, 2);
  I2C_I2CMasterWriteByte(0xf2, 1);
  I2C_I2CMasterWriteByte(0b00000001, 1);
  I2C_I2CMasterSendStop(2);
  // Set up the ctrl_meas register for 1x oversampling & normal mode
  I2C_I2CMasterSendStart(0x77, I2C_I2C_WRITE_XFER_MODE, 2);
  I2C_I2CMasterWriteByte(0xf4, 1);
  I2C_I2CMasterWriteByte(0b00100111, 1);
  I2C_I2CMasterSendStop(2);
}

void bme280TempRead(uint8 *buff)
{
  I2C_I2CMasterSendStart(0x77, I2C_I2C_WRITE_XFER_MODE, 2);
  I2C_I2CMasterWriteByte(0xfa, 1);
  I2C_I2CMasterSendStop(2);
  I2C_I2CMasterReadBuf(0x77, buff, 2, I2C_I2C_MODE_COMPLETE_XFER);
}

void bme280PresRead(uint8 *buff)
{  
  I2C_I2CMasterSendStart(0x77, I2C_I2C_WRITE_XFER_MODE, 2);
  I2C_I2CMasterWriteByte(0xf7, 1);
  I2C_I2CMasterSendStop(2);
  I2C_I2CMasterReadBuf(0x77, buff, 2, I2C_I2C_MODE_COMPLETE_XFER);
}

void bme280HumRead(uint8 *buff)
{  
  I2C_I2CMasterSendStart(0x77, I2C_I2C_WRITE_XFER_MODE, 2);
  I2C_I2CMasterWriteByte(0xfd, 1);
  I2C_I2CMasterSendStop(2);
  I2C_I2CMasterReadBuf(0x77, buff, 2, I2C_I2C_MODE_COMPLETE_XFER);
}
