#ifndef __bme280_h__
#define __bme280_h__
  
#include "project.h"

typedef int32 BME280_S32_t;
typedef uint32 BME280_U32_t;
typedef int64 BME280_S64_t;
typedef uint64 BME280_U64_t;
  
void bme280Init(); 
void bme280TempRead(uint8 *buff);
void bme280PresRead(uint8 *buff);
void bme280HumRead(uint8 *buff);
BME280_S32_t BME280_compensate_T_int32(BME280_S32_t adc_T);
BME280_U32_t BME280_compensate_P_int64(BME280_S32_t adc_P);
BME280_U32_t bme280_compensate_H_int32(BME280_S32_t adc_H);
  
#endif
  