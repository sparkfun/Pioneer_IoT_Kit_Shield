/*******************************************************************************
* File Name: cyfitter_gpio.h
* 
* PSoC Creator  4.2
*
* Description:
* 
* This file is automatically generated by PSoC Creator.
*
********************************************************************************
* Copyright (c) 2007-2017 Cypress Semiconductor.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
********************************************************************************/

#ifndef INCLUDED_CYFITTER_GPIO_H
#define INCLUDED_CYFITTER_GPIO_H
#include "cy_device_headers.h"

/* SW2 */
#define SW2_0_DRIVEMODE CY_GPIO_DM_PULLUP
#define SW2_0_INBUF_ENABLED 1u
#define SW2_0_INIT_DRIVESTATE 1u
#define SW2_0_INIT_MUXSEL 0u
#define SW2_0_INPUT_SYNC 2u
#define SW2_0_INTERRUPT_MODE CY_GPIO_INTR_FALLING
#define SW2_0_NUM 4u
#define SW2_0_PORT GPIO_PRT0
#define SW2_0_SLEWRATE CY_GPIO_SLEW_FAST
#define SW2_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS
#define SW2_DRIVEMODE CY_GPIO_DM_PULLUP
#define SW2_INBUF_ENABLED 1u
#define SW2_INIT_DRIVESTATE 1u
#define SW2_INIT_MUXSEL 0u
#define SW2_INPUT_SYNC 2u
#define SW2_INTERRUPT_MODE CY_GPIO_INTR_FALLING
#define SW2_NUM 4u
#define SW2_PORT GPIO_PRT0
#define SW2_SLEWRATE CY_GPIO_SLEW_FAST
#define SW2_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* UART_DEB_rx */
#define UART_DEB_rx_0_DRIVEMODE CY_GPIO_DM_HIGHZ
#define UART_DEB_rx_0_INBUF_ENABLED 1u
#define UART_DEB_rx_0_INIT_DRIVESTATE 1u
#define UART_DEB_rx_0_INIT_MUXSEL 18u
#define UART_DEB_rx_0_INPUT_SYNC 2u
#define UART_DEB_rx_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define UART_DEB_rx_0_NUM 0u
#define UART_DEB_rx_0_PORT GPIO_PRT10
#define UART_DEB_rx_0_SLEWRATE CY_GPIO_SLEW_FAST
#define UART_DEB_rx_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS
#define UART_DEB_rx_DRIVEMODE CY_GPIO_DM_HIGHZ
#define UART_DEB_rx_INBUF_ENABLED 1u
#define UART_DEB_rx_INIT_DRIVESTATE 1u
#define UART_DEB_rx_INIT_MUXSEL 18u
#define UART_DEB_rx_INPUT_SYNC 2u
#define UART_DEB_rx_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define UART_DEB_rx_NUM 0u
#define UART_DEB_rx_PORT GPIO_PRT10
#define UART_DEB_rx_SLEWRATE CY_GPIO_SLEW_FAST
#define UART_DEB_rx_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* UART_DEB_tx */
#define UART_DEB_tx_0_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define UART_DEB_tx_0_INBUF_ENABLED 0u
#define UART_DEB_tx_0_INIT_DRIVESTATE 1u
#define UART_DEB_tx_0_INIT_MUXSEL 18u
#define UART_DEB_tx_0_INPUT_SYNC 2u
#define UART_DEB_tx_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define UART_DEB_tx_0_NUM 1u
#define UART_DEB_tx_0_PORT GPIO_PRT10
#define UART_DEB_tx_0_SLEWRATE CY_GPIO_SLEW_FAST
#define UART_DEB_tx_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS
#define UART_DEB_tx_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define UART_DEB_tx_INBUF_ENABLED 0u
#define UART_DEB_tx_INIT_DRIVESTATE 1u
#define UART_DEB_tx_INIT_MUXSEL 18u
#define UART_DEB_tx_INPUT_SYNC 2u
#define UART_DEB_tx_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define UART_DEB_tx_NUM 1u
#define UART_DEB_tx_PORT GPIO_PRT10
#define UART_DEB_tx_SLEWRATE CY_GPIO_SLEW_FAST
#define UART_DEB_tx_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* XBEE_UART_tx */
#define XBEE_UART_tx_0_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define XBEE_UART_tx_0_INBUF_ENABLED 0u
#define XBEE_UART_tx_0_INIT_DRIVESTATE 1u
#define XBEE_UART_tx_0_INIT_MUXSEL 18u
#define XBEE_UART_tx_0_INPUT_SYNC 2u
#define XBEE_UART_tx_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define XBEE_UART_tx_0_NUM 1u
#define XBEE_UART_tx_0_PORT GPIO_PRT5
#define XBEE_UART_tx_0_SLEWRATE CY_GPIO_SLEW_FAST
#define XBEE_UART_tx_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS
#define XBEE_UART_tx_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define XBEE_UART_tx_INBUF_ENABLED 0u
#define XBEE_UART_tx_INIT_DRIVESTATE 1u
#define XBEE_UART_tx_INIT_MUXSEL 18u
#define XBEE_UART_tx_INPUT_SYNC 2u
#define XBEE_UART_tx_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define XBEE_UART_tx_NUM 1u
#define XBEE_UART_tx_PORT GPIO_PRT5
#define XBEE_UART_tx_SLEWRATE CY_GPIO_SLEW_FAST
#define XBEE_UART_tx_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* Connected_LED */
#define Connected_LED_0_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define Connected_LED_0_INBUF_ENABLED 0u
#define Connected_LED_0_INIT_DRIVESTATE 1u
#define Connected_LED_0_INIT_MUXSEL 0u
#define Connected_LED_0_INPUT_SYNC 2u
#define Connected_LED_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define Connected_LED_0_NUM 1u
#define Connected_LED_0_PORT GPIO_PRT11
#define Connected_LED_0_SLEWRATE CY_GPIO_SLEW_FAST
#define Connected_LED_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS
#define Connected_LED_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define Connected_LED_INBUF_ENABLED 0u
#define Connected_LED_INIT_DRIVESTATE 1u
#define Connected_LED_INIT_MUXSEL 0u
#define Connected_LED_INPUT_SYNC 2u
#define Connected_LED_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define Connected_LED_NUM 1u
#define Connected_LED_PORT GPIO_PRT11
#define Connected_LED_SLEWRATE CY_GPIO_SLEW_FAST
#define Connected_LED_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* Disconnect_LED */
#define Disconnect_LED_0_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define Disconnect_LED_0_INBUF_ENABLED 0u
#define Disconnect_LED_0_INIT_DRIVESTATE 1u
#define Disconnect_LED_0_INIT_MUXSEL 0u
#define Disconnect_LED_0_INPUT_SYNC 2u
#define Disconnect_LED_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define Disconnect_LED_0_NUM 3u
#define Disconnect_LED_0_PORT GPIO_PRT0
#define Disconnect_LED_0_SLEWRATE CY_GPIO_SLEW_FAST
#define Disconnect_LED_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS
#define Disconnect_LED_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define Disconnect_LED_INBUF_ENABLED 0u
#define Disconnect_LED_INIT_DRIVESTATE 1u
#define Disconnect_LED_INIT_MUXSEL 0u
#define Disconnect_LED_INPUT_SYNC 2u
#define Disconnect_LED_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define Disconnect_LED_NUM 3u
#define Disconnect_LED_PORT GPIO_PRT0
#define Disconnect_LED_SLEWRATE CY_GPIO_SLEW_FAST
#define Disconnect_LED_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* Advertising_LED */
#define Advertising_LED_0_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define Advertising_LED_0_INBUF_ENABLED 0u
#define Advertising_LED_0_INIT_DRIVESTATE 1u
#define Advertising_LED_0_INIT_MUXSEL 0u
#define Advertising_LED_0_INPUT_SYNC 2u
#define Advertising_LED_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define Advertising_LED_0_NUM 1u
#define Advertising_LED_0_PORT GPIO_PRT1
#define Advertising_LED_0_SLEWRATE CY_GPIO_SLEW_FAST
#define Advertising_LED_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS
#define Advertising_LED_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define Advertising_LED_INBUF_ENABLED 0u
#define Advertising_LED_INIT_DRIVESTATE 1u
#define Advertising_LED_INIT_MUXSEL 0u
#define Advertising_LED_INPUT_SYNC 2u
#define Advertising_LED_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define Advertising_LED_NUM 1u
#define Advertising_LED_PORT GPIO_PRT1
#define Advertising_LED_SLEWRATE CY_GPIO_SLEW_FAST
#define Advertising_LED_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

#endif /* INCLUDED_CYFITTER_GPIO_H */
