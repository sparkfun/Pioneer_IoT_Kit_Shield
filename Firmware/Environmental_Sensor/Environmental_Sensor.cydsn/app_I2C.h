#ifndef _APP_I2C_H_
#define _APP_I2C_H_	

#include "main.h"      

#define I2C_WRITE_BUFFER_SIZE	61 /* Max supported by BCP */
	
extern uint8 wrBuf[I2C_WRITE_BUFFER_SIZE]; /* I2C write buffer */ 

void sendI2CNotification(void);
void readI2CRegister(uint32 slaveAddress, uint32 regAddress, uint8* buff, uint32 cnt);

#endif /* _APP_I2C_H_ */

/* [] END OF FILE */
