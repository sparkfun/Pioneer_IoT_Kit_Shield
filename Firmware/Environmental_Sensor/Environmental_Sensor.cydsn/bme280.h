#ifndef __bme280_h__
#define __bme280_h__
  
#include "project.h"

void bme280Init(); 
void bme280TempRead(uint8 *buff);
void bme280PresRead(uint8 *buff);
void bme280HumRead(uint8 *buff);
  
#endif
  