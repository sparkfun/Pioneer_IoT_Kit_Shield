/*******************************************************************************
* File Name: SPI_SCB_IRQ.c  
* Version 1.0
*
*  Description:
*   SysInt component c-file
*
********************************************************************************
* Copyright 2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/


#include "SPI_SCB_IRQ.h"

/* Interrupt initialization structure(s) */

const cy_stc_sysint_t SPI_SCB_IRQ_cfg_cm4 = {
	.intrSrc = (IRQn_Type)SPI_SCB_IRQ_INTR_CORTEXM4_SRC,
	.intrCm0p = (IRQn_CM0P_Type)0u,
	.intrPriority = SPI_SCB_IRQ_INTR_CORTEXM4_PRIORITY
};

/* [] END OF FILE */
