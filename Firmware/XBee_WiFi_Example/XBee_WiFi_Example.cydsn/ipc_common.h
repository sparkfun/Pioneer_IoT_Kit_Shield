#ifndef __IPC_COMMON_H__
#define __IPC_COMMON_H__

#include "project.h"

#define D7_SEMAPHORE 16
#define D9_SEMAPHORE 17

/* function prototypes */
uint32_t ReadSharedVar(const uint8_t *sharedVar, uint8_t *copy, uint8_t semaID);
uint32_t WriteSharedVar(uint8_t *sharedVar, uint8_t value, uint8_t semaID);

#endif
