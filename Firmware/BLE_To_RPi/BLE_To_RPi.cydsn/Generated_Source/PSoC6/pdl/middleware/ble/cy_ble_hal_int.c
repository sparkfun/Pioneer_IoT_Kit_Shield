/***************************************************************************//**
* \file cy_ble_hal_int.c
* \version 2.0
*
* \brief
*  This file contains the source code for the Interrupt Service Routine for the
*  HAL section of the BLE Middleware.
*
********************************************************************************
* \copyright
* Copyright 2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/
#include "cy_ble_event_handler.h"
#include "cy_ble_hal_pvt.h"

/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if (CY_BLE_HOST_CONTR_CORE)

#if (CY_BLE_CONFIG_STACK_CONTR_CORE)

#if (CY_BLE_STACK_MODE == CY_BLE_STACK_RELEASE)
/* Private Function Prototypes  */
static void Cy_BLE_HAL_SendInterruptNotification(void);

/* Globals */    
volatile uint32_t cy_ble_interruptCallbackFeatureMask = 0u;  


/*******************************************************************************
* Function Name: Cy_BLE_HAL_SendInterruptNotification
****************************************************************************//**
*
*  This function sends notification (with interrupt type information) to user 
*  callback for the BLE interrupts subscribed by user. The list of supported 
*  interrupts described in the enumeration cy_en_ble_interrupt_callback_feature_t.
*  Cy_BLE_RegisterInterruptCallback() API used to register callback for receiving
*  the BLE interrupts.
*
*******************************************************************************/
static void Cy_BLE_HAL_SendInterruptNotification(void)
{
    if(cy_ble_interruptCallbackFeatureMask != 0u)
    {
        cy_en_ble_interrupt_callback_feature_t interruptType = CY_BLE_INTR_CALLBACK_NONE;
                
        /* Read BLE_BLELL->EVENT_INTR */      
        if((cy_ble_interruptCallbackFeatureMask & CY_BLE_INTR_CALLBACK_BLESS_STACK_ISR) != 0u) 
        {
            interruptType |= CY_BLE_INTR_CALLBACK_BLESS_STACK_ISR;    
        }
        else
        {
            if(((BLE_BLESS->INTR_STAT & BLE_BLESS_INTR_STAT_DSM_ENTERED_INTR_Msk) != 0u) &&
               ((cy_ble_interruptCallbackFeatureMask & CY_BLE_INTR_CALLBACK_BLESS_INTR_STAT_DSM_ENTERED)!= 0u))
            {
                interruptType |= CY_BLE_INTR_CALLBACK_BLESS_INTR_STAT_DSM_ENTERED;
            }
            
            if(((BLE_BLESS->INTR_STAT & BLE_BLESS_INTR_STAT_DSM_EXITED_INTR_Msk) != 0u) &&
               ((cy_ble_interruptCallbackFeatureMask & CY_BLE_INTR_CALLBACK_BLESS_INTR_STAT_DSM_EXITED) != 0u))
            {
                interruptType |= CY_BLE_INTR_CALLBACK_BLESS_INTR_STAT_DSM_EXITED;
            }
       
            if(((BLE_BLESS->INTR_STAT & BLE_BLESS_INTR_STAT_LL_INTR_Msk) != 0u))
            { 
                /**
                 *  Updates the firmware and hardware to exit sleep mode, when
                 *  called from the interrupt mode, after checking the state machine.
                 */
                Cy_BLE_LlIsrExitLowPowerMode();
           
                if(BLE_BLELL->EVENT_INTR != 0x0u)
                {            
                    if((BLE_BLELL->EVENT_INTR & BLE_BLELL_EVENT_INTR_CONN_INTR_Msk) != 0u)
                    {
                        if(((BLE_BLELL->CONN_INTR & BLE_BLELL_CONN_INTR_CLOSE_CE_Msk) != 0u) &&
                            ((cy_ble_interruptCallbackFeatureMask & CY_BLE_INTR_CALLBACK_BLELL_CONN_INTR_CLOSE_CE) != 0u))
                        {
                            /* Return interrupt type CY_BLE_INTR_CALLBACK_BLELL_CONN_INTR_CLOSE_CE */    
                            interruptType |= CY_BLE_INTR_CALLBACK_BLELL_CONN_INTR_CLOSE_CE;
                        }
                        
                        if(((BLE_BLELL->CONN_INTR & BLE_BLELL_CONN_INTR_CE_TX_ACK_Msk) != 0u) &&
                           ((cy_ble_interruptCallbackFeatureMask & CY_BLE_INTR_CALLBACK_BLELL_CONN_INTR_CE_TX_ACK) != 0u))
                        {
                            /* Return interrupt type CY_BLE_INTR_CALLBACK_BLELL_CONN_INTR_CE_TX_ACK */    
                            interruptType |= CY_BLE_INTR_CALLBACK_BLELL_CONN_INTR_CE_TX_ACK;
                        }
                        
                        if(((BLE_BLELL->CONN_EXT_INTR & BLE_BLELL_CONN_EXT_INTR_EARLY_INTR_Msk) != 0u) &&
                           ((cy_ble_interruptCallbackFeatureMask & CY_BLE_INTR_CALLBACK_BLELL_CONN_EXT_INTR_EARLY) != 0u))
                        {
                            /* Return interrupt type CY_BLE_INTR_CALLBACK_BLELL_CONN_EXT_INTR_EARLY */    
                            interruptType |= CY_BLE_INTR_CALLBACK_BLELL_CONN_EXT_INTR_EARLY;                     
                        }
                        
                        if(((BLE_BLELL->CONN_INTR & BLE_BLELL_CONN_INTR_CE_RX_Msk) != 0u) &&
                            ((cy_ble_interruptCallbackFeatureMask & CY_BLE_INTR_CALLBACK_BLELL_CONN_INTR_CE_RX) != 0u))
                        {
                            /* Return interrupt type CY_BLE_INTR_CALLBACK_BLELL_CONN_INTR_CE_RX */    
                            interruptType |= CY_BLE_INTR_CALLBACK_BLELL_CONN_INTR_CE_RX;
                        }
                        
                    }    
                    
                    if((BLE_BLELL->EVENT_INTR & BLE_BLELL_EVENT_INTR_SCAN_INTR_Msk) != 0u)
                    {
                        if(((BLE_BLELL->SCAN_INTR & BLE_BLELL_SCAN_INTR_ADV_RX_INTR_Msk) != 0u) &&
                           ((cy_ble_interruptCallbackFeatureMask & CY_BLE_INTR_CALLBACK_BLELL_SCAN_INTR_ADV_RX) != 0u))
                        {
                            /* Return interrupt type Return interrupt type CY_BLE_INTR_CALLBACK_BLELL_SCAN_INTR_ADV_RX */    
                            interruptType |= CY_BLE_INTR_CALLBACK_BLELL_SCAN_INTR_ADV_RX;
                        }
                        
                        if(((BLE_BLELL->SCAN_INTR & BLE_BLELL_SCAN_INTR_SCAN_RSP_RX_INTR_Msk) != 0u) &&
                           ((cy_ble_interruptCallbackFeatureMask & CY_BLE_INTR_CALLBACK_BLELL_SCAN_INTR_SCAN_RSP_RX) != 0u))
                        {
                            /* Return interrupt type CY_BLE_INTR_CALLBACK_BLELL_SCAN_INTR_SCAN_RSP_RX */    
                            interruptType |= CY_BLE_INTR_CALLBACK_BLELL_SCAN_INTR_SCAN_RSP_RX;
                        }  
                    }
                    
                    if((BLE_BLELL->EVENT_INTR & BLE_BLELL_EVENT_INTR_ADV_INTR_Msk) != 0u)
                    {
                        if(((BLE_BLELL->ADV_INTR & BLE_BLELL_ADV_INTR_CONN_REQ_RX_INTR_Msk) != 0u) &&
                           ((cy_ble_interruptCallbackFeatureMask & CY_BLE_INTR_CALLBACK_BLELL_ADV_INTR_CONN_REQ_RX) != 0u))
                        {
                            /* Return interrupt type CY_BLE_INTR_CALLBACK_BLELL_ADV_INTR_CONN_REQ_RX */    
                            interruptType |= CY_BLE_INTR_CALLBACK_BLELL_ADV_INTR_CONN_REQ_RX;
                        }   
                    }
                }
            }
        }
        
        /* Send interrupt type information to registered callback */
        if(interruptType != CY_BLE_INTR_CALLBACK_NONE)
        {       
            #if (CY_BLE_MODE_PROFILE)
                if(Cy_BLE_InterruptCallback != NULL)
                {
                    Cy_BLE_InterruptCallback((uint32_t)interruptType);   
                }
            #endif /* (CY_BLE_MODE_PROFILE) */
        }
    }
}
#endif /* (CY_BLE_STACK_MODE == CY_BLE_STACK_RELEASE) */
    

/*******************************************************************************
* Function Name: Cy_BLE_BlessInterrupt
****************************************************************************//**
*
*  Handles the Interrupt Service Routine for the BLE sub-system.
*
* \return
*  None
*
*******************************************************************************/
void Cy_BLE_BlessInterrupt(void)
{
#if (CY_BLE_STACK_MODE == CY_BLE_STACK_RELEASE)
    /** 
     * Send notification (with interrupt type) to user callback for the 
     * BLE interrupts subscribed by user. Cy_BLE_RegisterInterruptCallback API 
     * used to register callback for receiving BLE interrupts.
     */
    Cy_BLE_HAL_SendInterruptNotification();
#endif /* (CY_BLE_STACK_MODE == CY_BLE_STACK_RELEASE) */
    
    /* Call BLE Stack manager bless function handler */
    Cy_BLE_HAL_BlessInterruptHandler();
    /* Host stack takes care of clearing interrupts */
}

#endif  /* (CY_BLE_CONFIG_STACK_CONTR_CORE) */

#if (CY_BLE_STACK_MODE_HOST_UART)


/*******************************************************************************
* Function Name: Cy_BLE_HAL_HOST_UART_Interrupt
****************************************************************************//**
*
*  Handles the Interrupt Service Routine for the UART.
*
* \return
*  None
*
*******************************************************************************/
void Cy_BLE_HAL_HOST_UART_Interrupt(void)
{
    uint8_t uartTxStatus = CY_BLE_INTR_TX_SUCCESS;
    uint32_t srcInterrupt = 0u;

    uint8_t lenRxFifo;
    uint8_t srcCount;
    uint8_t uartRxStatus = CY_BLE_INTR_RX_SUCCESS;
    uint8_t receivedData;

    /* Evaluate RX interrupt event */
    srcInterrupt = Cy_SCB_GetRxInterruptStatusMasked(cy_ble_configPtr->uartHostHw);

    if(0u != (srcInterrupt & CY_SCB_RX_INTR_NOT_EMPTY))
    {
        if(0u != (srcInterrupt & CY_SCB_RX_INTR_UART_PARITY_ERROR))
        {
            uartRxStatus |= CY_BLE_INTR_RX_PARITY_ERROR;
        }
        if(0u != (srcInterrupt & CY_SCB_RX_INTR_UART_FRAME_ERROR))
        {
            uartRxStatus |= CY_BLE_INTR_RX_FRAME_ERROR;
        }
        if(0u != (srcInterrupt & CY_SCB_RX_INTR_OVERFLOW))
        {
            uartRxStatus |= CY_BLE_INTR_RX_OVERFLOW;
        }
        if(uartRxStatus == CY_BLE_INTR_RX_SUCCESS)
        {
            lenRxFifo = (uint8_t)Cy_SCB_GetNumInRxFifo(cy_ble_configPtr->uartHostHw);
            for(srcCount = 0u; srcCount < lenRxFifo; srcCount++)
            {
                receivedData = (uint8_t)Cy_SCB_ReadRxFifo(cy_ble_configPtr->uartHostHw);
                Cy_BLE_HAL_HciTransportHandler((char8)receivedData, 1u);
            }
        }
        else
        {
            Cy_SCB_ClearRxFifo(cy_ble_configPtr->uartHostHw);
        }
        Cy_SCB_ClearRxInterrupt(cy_ble_configPtr->uartHostHw, srcInterrupt);
    }
    else
    {
        /* No RX interrupt. Do nothing. */
    }

    /* Evaluate TX interrupt event in sequence */
    srcInterrupt = Cy_SCB_GetTxInterruptStatusMasked(cy_ble_configPtr->uartHostHw);

    /* Stack manager TX UART complete */
    if(0u != (srcInterrupt & CY_SCB_TX_INTR_UART_DONE))
    {
        if(0u != (srcInterrupt & CY_SCB_TX_INTR_OVERFLOW))
        {
            /*Stack manager TX UART error */
            uartTxStatus |= CY_BLE_INTR_TX_OVERFLOW;
        }
        Cy_SCB_ClearTxInterrupt(cy_ble_configPtr->uartHostHw, srcInterrupt);
    }
    else
    {
        /* No TX interrupt. Do nothing. */
    }
}
#endif /* (CY_BLE_STACK_MODE_HOST_UART) */

#endif /* (CY_BLE_HOST_CONTR_CORE) */

#ifdef __cplusplus
}
#endif /* __cplusplus */

/* [] END OF FILE */
