/*******************************************************************************
* File Name: UART_SCB_IRQ.c  
* Version 1.0
*
*  Description:
*   SysInt component header file
*
********************************************************************************
* Copyright 2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_SYSINT_UART_SCB_IRQ_H)
#define CY_SYSINT_UART_SCB_IRQ_H

#include "cyfitter.h"
#include "sysint/cy_sysint.h"


/***************************************
*       Placement Definitions
***************************************/
#define UART_SCB_IRQ_INTR_CORTEXM4_SRC			(UART_SCB_IRQ__INTC_NUMBER)
#define UART_SCB_IRQ_INTR_CORTEXM4_PRIORITY		(UART_SCB_IRQ__INTC_CORTEXM4_PRIORITY)

extern const cy_stc_sysint_t UART_SCB_IRQ_cfg_cm4;

    
#endif /* CY_SYSINT_UART_SCB_IRQ_H */


/* [] END OF FILE */
