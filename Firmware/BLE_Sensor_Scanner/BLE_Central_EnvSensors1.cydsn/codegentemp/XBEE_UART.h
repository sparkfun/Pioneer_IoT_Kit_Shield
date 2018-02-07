/***************************************************************************//**
* \file XBEE_UART.h
* \version 2.0
*
*  This file provides constants and parameter values for the UART component.
*
********************************************************************************
* \copyright
* Copyright 2016-2017, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(XBEE_UART_CY_SCB_UART_PDL_H)
#define XBEE_UART_CY_SCB_UART_PDL_H

#include "cyfitter.h"
#include "scb/cy_scb_uart.h"

#if defined(__cplusplus)
extern "C" {
#endif

/***************************************
*   Initial Parameter Constants
****************************************/

#define XBEE_UART_DIRECTION  (2U)
#define XBEE_UART_ENABLE_RTS (0U)
#define XBEE_UART_ENABLE_CTS (0U)

/* UART direction enum */
#define XBEE_UART_RX    (0x1U)
#define XBEE_UART_TX    (0x2U)

#define XBEE_UART_ENABLE_RX  (0UL != (XBEE_UART_DIRECTION & XBEE_UART_RX))
#define XBEE_UART_ENABLE_TX  (0UL != (XBEE_UART_DIRECTION & XBEE_UART_TX))


/***************************************
*        Function Prototypes
***************************************/
/**
* \addtogroup group_general
* @{
*/
/* Component specific functions. */
void XBEE_UART_Start(void);

/* Basic functions */
__STATIC_INLINE cy_en_scb_uart_status_t XBEE_UART_Init(cy_stc_scb_uart_config_t const *config);
__STATIC_INLINE void XBEE_UART_DeInit(void);
__STATIC_INLINE void XBEE_UART_Enable(void);
__STATIC_INLINE void XBEE_UART_Disable(void);

/* Register callback. */
__STATIC_INLINE void XBEE_UART_RegisterCallback(cy_cb_scb_uart_handle_events_t callback);

/* Configuration change. */
#if (XBEE_UART_ENABLE_CTS)
__STATIC_INLINE void XBEE_UART_EnableCts(void);
__STATIC_INLINE void XBEE_UART_DisableCts(void);
#endif /* (XBEE_UART_ENABLE_CTS) */

#if (XBEE_UART_ENABLE_RTS)
__STATIC_INLINE void     XBEE_UART_SetRtsFifoLevel(uint32_t level);
__STATIC_INLINE uint32_t XBEE_UART_GetRtsFifoLevel(void);
#endif /* (XBEE_UART_ENABLE_RTS) */

__STATIC_INLINE void XBEE_UART_EnableSkipStart(void);
__STATIC_INLINE void XBEE_UART_DisableSkipStart(void);

#if (XBEE_UART_ENABLE_RX)
/* Low level: Receive direction. */
__STATIC_INLINE uint32_t XBEE_UART_Get(void);
__STATIC_INLINE uint32_t XBEE_UART_GetArray(void *buffer, uint32_t size);
__STATIC_INLINE void     XBEE_UART_GetArrayBlocking(void *buffer, uint32_t size);
__STATIC_INLINE uint32_t XBEE_UART_GetRxFifoStatus(void);
__STATIC_INLINE void     XBEE_UART_ClearRxFifoStatus(uint32_t clearMask);
__STATIC_INLINE uint32_t XBEE_UART_GetNumInRxFifo(void);
__STATIC_INLINE void     XBEE_UART_ClearRxFifo(void);
#endif /* (XBEE_UART_ENABLE_RX) */

#if (XBEE_UART_ENABLE_TX)
/* Low level: Transmit direction. */
__STATIC_INLINE uint32_t XBEE_UART_Put(uint32_t data);
__STATIC_INLINE uint32_t XBEE_UART_PutArray(void *buffer, uint32_t size);
__STATIC_INLINE void     XBEE_UART_PutArrayBlocking(void *buffer, uint32_t size);
__STATIC_INLINE void     XBEE_UART_PutString(char_t const string[]);
__STATIC_INLINE void     XBEE_UART_SendBreakBlocking(uint32_t breakWidth);
__STATIC_INLINE uint32_t XBEE_UART_GetTxFifoStatus(void);
__STATIC_INLINE void     XBEE_UART_ClearTxFifoStatus(uint32_t clearMask);
__STATIC_INLINE uint32_t XBEE_UART_GetNumInTxFifo(void);
__STATIC_INLINE bool     XBEE_UART_IsTxComplete(void);
__STATIC_INLINE void     XBEE_UART_ClearTxFifo(void);
#endif /* (XBEE_UART_ENABLE_TX) */

#if (XBEE_UART_ENABLE_RX)
/* High level: Ring buffer functions. */
__STATIC_INLINE void     XBEE_UART_StartRingBuffer(void *buffer, uint32_t size);
__STATIC_INLINE void     XBEE_UART_StopRingBuffer(void);
__STATIC_INLINE void     XBEE_UART_ClearRingBuffer(void);
__STATIC_INLINE uint32_t XBEE_UART_GetNumInRingBuffer(void);

/* High level: Receive direction functions. */
__STATIC_INLINE cy_en_scb_uart_status_t XBEE_UART_Receive(void *buffer, uint32_t size);
__STATIC_INLINE void     XBEE_UART_AbortReceive(void);
__STATIC_INLINE uint32_t XBEE_UART_GetReceiveStatus(void);
__STATIC_INLINE uint32_t XBEE_UART_GetNumReceived(void);
#endif /* (XBEE_UART_ENABLE_RX) */

#if (XBEE_UART_ENABLE_TX)
/* High level: Transmit direction functions. */
__STATIC_INLINE cy_en_scb_uart_status_t XBEE_UART_Transmit(void *buffer, uint32_t size);
__STATIC_INLINE void     XBEE_UART_AbortTransmit(void);
__STATIC_INLINE uint32_t XBEE_UART_GetTransmitStatus(void);
__STATIC_INLINE uint32_t XBEE_UART_GetNumLeftToTransmit(void);
#endif /* (XBEE_UART_ENABLE_TX) */

/* Interrupt handler */
__STATIC_INLINE void XBEE_UART_Interrupt(void);
/** @} group_general */


/***************************************
*    Variables with External Linkage
***************************************/
/**
* \addtogroup group_globals
* @{
*/
extern uint8_t XBEE_UART_initVar;
extern cy_stc_scb_uart_config_t const XBEE_UART_config;
extern cy_stc_scb_uart_context_t XBEE_UART_context;
/** @} group_globals */


/***************************************
*         Preprocessor Macros
***************************************/
/**
* \addtogroup group_macros
* @{
*/
/** The pointer to the base address of the hardware */
#define XBEE_UART_HW     ((CySCB_Type *) XBEE_UART_SCB__HW)
/** @} group_macros */


/***************************************
*    In-line Function Implementation
***************************************/

/*******************************************************************************
* Function Name: XBEE_UART_Init
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_Init() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE cy_en_scb_uart_status_t XBEE_UART_Init(cy_stc_scb_uart_config_t const *config)
{
   return Cy_SCB_UART_Init(XBEE_UART_HW, config, &XBEE_UART_context);
}


/*******************************************************************************
* Function Name: XBEE_UART_DeInit
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_DeInit() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void XBEE_UART_DeInit(void)
{
    Cy_SCB_UART_DeInit(XBEE_UART_HW);
}


/*******************************************************************************
* Function Name: XBEE_UART_Enable
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_Enable() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void XBEE_UART_Enable(void)
{
    Cy_SCB_UART_Enable(XBEE_UART_HW);
}


/*******************************************************************************
* Function Name: XBEE_UART_Disable
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_Disable() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void XBEE_UART_Disable(void)
{
    Cy_SCB_UART_Disable(XBEE_UART_HW, &XBEE_UART_context);
}


/*******************************************************************************
* Function Name: XBEE_UART_RegisterCallback
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_RegisterCallback() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void XBEE_UART_RegisterCallback(cy_cb_scb_uart_handle_events_t callback)
{
    Cy_SCB_UART_RegisterCallback(XBEE_UART_HW, callback, &XBEE_UART_context);
}


#if (XBEE_UART_ENABLE_CTS)
/*******************************************************************************
* Function Name: XBEE_UART_EnableCts
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_EnableCts() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void XBEE_UART_EnableCts(void)
{
    Cy_SCB_UART_EnableCts(XBEE_UART_HW);
}


/*******************************************************************************
* Function Name: Cy_SCB_UART_DisableCts
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_DisableCts() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void XBEE_UART_DisableCts(void)
{
    Cy_SCB_UART_DisableCts(XBEE_UART_HW);
}
#endif /* (XBEE_UART_ENABLE_CTS) */


#if (XBEE_UART_ENABLE_RTS)
/*******************************************************************************
* Function Name: XBEE_UART_SetRtsFifoLevel
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_SetRtsFifoLevel() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void XBEE_UART_SetRtsFifoLevel(uint32_t level)
{
    Cy_SCB_UART_SetRtsFifoLevel(XBEE_UART_HW, level);
}


/*******************************************************************************
* Function Name: XBEE_UART_GetRtsFifoLevel
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetRtsFifoLevel() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t XBEE_UART_GetRtsFifoLevel(void)
{
    return Cy_SCB_UART_GetRtsFifoLevel(XBEE_UART_HW);
}
#endif /* (XBEE_UART_ENABLE_RTS) */


/*******************************************************************************
* Function Name: XBEE_UART_EnableSkipStart
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_EnableSkipStart() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void XBEE_UART_EnableSkipStart(void)
{
    Cy_SCB_UART_EnableSkipStart(XBEE_UART_HW);
}


/*******************************************************************************
* Function Name: XBEE_UART_DisableSkipStart
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_DisableSkipStart() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void XBEE_UART_DisableSkipStart(void)
{
    Cy_SCB_UART_DisableSkipStart(XBEE_UART_HW);
}


#if (XBEE_UART_ENABLE_RX)
/*******************************************************************************
* Function Name: XBEE_UART_Get
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_Get() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t XBEE_UART_Get(void)
{
    return Cy_SCB_UART_Get(XBEE_UART_HW);
}


/*******************************************************************************
* Function Name: XBEE_UART_GetArray
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetArray() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t XBEE_UART_GetArray(void *buffer, uint32_t size)
{
    return Cy_SCB_UART_GetArray(XBEE_UART_HW, buffer, size);
}


/*******************************************************************************
* Function Name: XBEE_UART_GetArrayBlocking
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetArrayBlocking() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void XBEE_UART_GetArrayBlocking(void *buffer, uint32_t size)
{
    Cy_SCB_UART_GetArrayBlocking(XBEE_UART_HW, buffer, size);
}


/*******************************************************************************
* Function Name: XBEE_UART_GetRxFifoStatus
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetRxFifoStatus() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t XBEE_UART_GetRxFifoStatus(void)
{
    return Cy_SCB_UART_GetRxFifoStatus(XBEE_UART_HW);
}


/*******************************************************************************
* Function Name: XBEE_UART_ClearRxFifoStatus
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_ClearRxFifoStatus() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void XBEE_UART_ClearRxFifoStatus(uint32_t clearMask)
{
    Cy_SCB_UART_ClearRxFifoStatus(XBEE_UART_HW, clearMask);
}


/*******************************************************************************
* Function Name: XBEE_UART_GetNumInRxFifo
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetNumInRxFifo() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t XBEE_UART_GetNumInRxFifo(void)
{
    return Cy_SCB_UART_GetNumInRxFifo(XBEE_UART_HW);
}


/*******************************************************************************
* Function Name: XBEE_UART_ClearRxFifo
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_ClearRxFifo() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void XBEE_UART_ClearRxFifo(void)
{
    Cy_SCB_UART_ClearRxFifo(XBEE_UART_HW);
}
#endif /* (XBEE_UART_ENABLE_RX) */


#if (XBEE_UART_ENABLE_TX)
/*******************************************************************************
* Function Name: XBEE_UART_Put
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_Put() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t XBEE_UART_Put(uint32_t data)
{
    return Cy_SCB_UART_Put(XBEE_UART_HW,data);
}


/*******************************************************************************
* Function Name: XBEE_UART_PutArray
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_PutArray() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t XBEE_UART_PutArray(void *buffer, uint32_t size)
{
    return Cy_SCB_UART_PutArray(XBEE_UART_HW, buffer, size);
}


/*******************************************************************************
* Function Name: XBEE_UART_PutArrayBlocking
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_PutArrayBlocking() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void XBEE_UART_PutArrayBlocking(void *buffer, uint32_t size)
{
    Cy_SCB_UART_PutArrayBlocking(XBEE_UART_HW, buffer, size);
}


/*******************************************************************************
* Function Name: XBEE_UART_PutString
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_PutString() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void XBEE_UART_PutString(char_t const string[])
{
    Cy_SCB_UART_PutString(XBEE_UART_HW, string);
}


/*******************************************************************************
* Function Name: XBEE_UART_SendBreakBlocking
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_SendBreakBlocking() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void XBEE_UART_SendBreakBlocking(uint32_t breakWidth)
{
    Cy_SCB_UART_SendBreakBlocking(XBEE_UART_HW, breakWidth);
}


/*******************************************************************************
* Function Name: XBEE_UART_GetTxFifoStatus
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetTxFifoStatus() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t XBEE_UART_GetTxFifoStatus(void)
{
    return Cy_SCB_UART_GetTxFifoStatus(XBEE_UART_HW);
}


/*******************************************************************************
* Function Name: XBEE_UART_ClearTxFifoStatus
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_ClearTxFifoStatus() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void XBEE_UART_ClearTxFifoStatus(uint32_t clearMask)
{
    Cy_SCB_UART_ClearTxFifoStatus(XBEE_UART_HW, clearMask);
}


/*******************************************************************************
* Function Name: XBEE_UART_GetNumInTxFifo
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetNumInTxFifo() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t XBEE_UART_GetNumInTxFifo(void)
{
    return Cy_SCB_UART_GetNumInTxFifo(XBEE_UART_HW);
}


/*******************************************************************************
* Function Name: XBEE_UART_IsTxComplete
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_IsTxComplete() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE bool XBEE_UART_IsTxComplete(void)
{
    return Cy_SCB_UART_IsTxComplete(XBEE_UART_HW);
}


/*******************************************************************************
* Function Name: XBEE_UART_ClearTxFifo
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_ClearTxFifo() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void XBEE_UART_ClearTxFifo(void)
{
    Cy_SCB_UART_ClearTxFifo(XBEE_UART_HW);
}
#endif /* (XBEE_UART_ENABLE_TX) */


#if (XBEE_UART_ENABLE_RX)
/*******************************************************************************
* Function Name: XBEE_UART_StartRingBuffer
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_StartRingBuffer() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void XBEE_UART_StartRingBuffer(void *buffer, uint32_t size)
{
    Cy_SCB_UART_StartRingBuffer(XBEE_UART_HW, buffer, size, &XBEE_UART_context);
}


/*******************************************************************************
* Function Name: XBEE_UART_StopRingBuffer
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_StopRingBuffer() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void XBEE_UART_StopRingBuffer(void)
{
    Cy_SCB_UART_StopRingBuffer(XBEE_UART_HW, &XBEE_UART_context);
}


/*******************************************************************************
* Function Name: XBEE_UART_ClearRingBuffer
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_ClearRingBuffer() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void XBEE_UART_ClearRingBuffer(void)
{
    Cy_SCB_UART_ClearRingBuffer(XBEE_UART_HW, &XBEE_UART_context);
}


/*******************************************************************************
* Function Name: XBEE_UART_GetNumInRingBuffer
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetNumInRingBuffer() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t XBEE_UART_GetNumInRingBuffer(void)
{
    return Cy_SCB_UART_GetNumInRingBuffer(XBEE_UART_HW, &XBEE_UART_context);
}


/*******************************************************************************
* Function Name: XBEE_UART_Receive
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_Receive() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE cy_en_scb_uart_status_t XBEE_UART_Receive(void *buffer, uint32_t size)
{
    return Cy_SCB_UART_Receive(XBEE_UART_HW, buffer, size, &XBEE_UART_context);
}


/*******************************************************************************
* Function Name: XBEE_UART_GetReceiveStatus
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetReceiveStatus() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t XBEE_UART_GetReceiveStatus(void)
{
    return Cy_SCB_UART_GetReceiveStatus(XBEE_UART_HW, &XBEE_UART_context);
}


/*******************************************************************************
* Function Name: XBEE_UART_AbortReceive
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_AbortReceive() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void XBEE_UART_AbortReceive(void)
{
    Cy_SCB_UART_AbortReceive(XBEE_UART_HW, &XBEE_UART_context);
}


/*******************************************************************************
* Function Name: XBEE_UART_GetNumReceived
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetNumReceived() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t XBEE_UART_GetNumReceived(void)
{
    return Cy_SCB_UART_GetNumReceived(XBEE_UART_HW, &XBEE_UART_context);
}
#endif /* (XBEE_UART_ENABLE_RX) */


#if (XBEE_UART_ENABLE_TX)
/*******************************************************************************
* Function Name: XBEE_UART_Transmit
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_Transmit() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE cy_en_scb_uart_status_t XBEE_UART_Transmit(void *buffer, uint32_t size)
{
    return Cy_SCB_UART_Transmit(XBEE_UART_HW, buffer, size, &XBEE_UART_context);
}


/*******************************************************************************
* Function Name: XBEE_UART_GetTransmitStatus
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetTransmitStatus() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t XBEE_UART_GetTransmitStatus(void)
{
    return Cy_SCB_UART_GetTransmitStatus(XBEE_UART_HW, &XBEE_UART_context);
}


/*******************************************************************************
* Function Name: XBEE_UART_AbortTransmit
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_AbortTransmit() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void XBEE_UART_AbortTransmit(void)
{
    Cy_SCB_UART_AbortTransmit(XBEE_UART_HW, &XBEE_UART_context);
}


/*******************************************************************************
* Function Name: XBEE_UART_GetNumLeftToTransmit
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_GetNumLeftToTransmit() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t XBEE_UART_GetNumLeftToTransmit(void)
{
    return Cy_SCB_UART_GetNumLeftToTransmit(XBEE_UART_HW, &XBEE_UART_context);
}
#endif /* (XBEE_UART_ENABLE_TX) */


/*******************************************************************************
* Function Name: XBEE_UART_Interrupt
****************************************************************************//**
*
* Invokes the Cy_SCB_UART_Interrupt() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void XBEE_UART_Interrupt(void)
{
    Cy_SCB_UART_Interrupt(XBEE_UART_HW, &XBEE_UART_context);
}

#if defined(__cplusplus)
}
#endif

#endif /* XBEE_UART_CY_SCB_UART_PDL_H */


/* [] END OF FILE */
