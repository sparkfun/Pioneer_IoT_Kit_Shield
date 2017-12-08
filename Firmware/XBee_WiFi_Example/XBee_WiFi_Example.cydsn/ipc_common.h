#ifndef __IPC_COMMON_H__
#define __IPC_COMMON_H__

#include "project.h"

/* function prototypes */
uint32_t ReadSharedVar(const uint8_t *sharedVar, uint8_t *copy);
uint32_t WriteSharedVar(uint8_t *sharedVar, uint8_t value);

#endif
