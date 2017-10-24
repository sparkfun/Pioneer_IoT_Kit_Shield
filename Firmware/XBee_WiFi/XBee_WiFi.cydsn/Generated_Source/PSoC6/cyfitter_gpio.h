#ifndef INCLUDED_CYFITTER_GPIO_H
#define INCLUDED_CYFITTER_GPIO_H
#include "cy_device_headers.h"

/* Blue_LED */
#define Blue_LED_0_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define Blue_LED_0_INBUF_ENABLED 0u
#define Blue_LED_0_INIT_DRIVESTATE 1u
#define Blue_LED_0_INIT_MUXSEL 0u
#define Blue_LED_0_INPUT_SYNC 2u
#define Blue_LED_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define Blue_LED_0_PORT GPIO_PRT11
#define Blue_LED_0_SHIFT 1u
#define Blue_LED_0 Blue_LED_0_PORT, Blue_LED_0_SHIFT
#define Blue_LED_0_SLEWRATE CY_GPIO_SLEW_FAST
#define Blue_LED_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* Pin_CM0p_Reading */
#define Pin_CM0p_Reading_0_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define Pin_CM0p_Reading_0_INBUF_ENABLED 0u
#define Pin_CM0p_Reading_0_INIT_DRIVESTATE 0u
#define Pin_CM0p_Reading_0_INIT_MUXSEL 0u
#define Pin_CM0p_Reading_0_INPUT_SYNC 2u
#define Pin_CM0p_Reading_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define Pin_CM0p_Reading_0_PORT GPIO_PRT0
#define Pin_CM0p_Reading_0_SHIFT 1u
#define Pin_CM0p_Reading_0 Pin_CM0p_Reading_0_PORT, Pin_CM0p_Reading_0_SHIFT
#define Pin_CM0p_Reading_0_SLEWRATE CY_GPIO_SLEW_FAST
#define Pin_CM0p_Reading_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* Pin_CM0p_Writing */
#define Pin_CM0p_Writing_0_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define Pin_CM0p_Writing_0_INBUF_ENABLED 0u
#define Pin_CM0p_Writing_0_INIT_DRIVESTATE 0u
#define Pin_CM0p_Writing_0_INIT_MUXSEL 0u
#define Pin_CM0p_Writing_0_INPUT_SYNC 2u
#define Pin_CM0p_Writing_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define Pin_CM0p_Writing_0_PORT GPIO_PRT0
#define Pin_CM0p_Writing_0_SHIFT 0u
#define Pin_CM0p_Writing_0 Pin_CM0p_Writing_0_PORT, Pin_CM0p_Writing_0_SHIFT
#define Pin_CM0p_Writing_0_SLEWRATE CY_GPIO_SLEW_FAST
#define Pin_CM0p_Writing_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* Pin_CM4_Reading */
#define Pin_CM4_Reading_0_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define Pin_CM4_Reading_0_INBUF_ENABLED 0u
#define Pin_CM4_Reading_0_INIT_DRIVESTATE 0u
#define Pin_CM4_Reading_0_INIT_MUXSEL 0u
#define Pin_CM4_Reading_0_INPUT_SYNC 2u
#define Pin_CM4_Reading_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define Pin_CM4_Reading_0_PORT GPIO_PRT0
#define Pin_CM4_Reading_0_SHIFT 5u
#define Pin_CM4_Reading_0 Pin_CM4_Reading_0_PORT, Pin_CM4_Reading_0_SHIFT
#define Pin_CM4_Reading_0_SLEWRATE CY_GPIO_SLEW_FAST
#define Pin_CM4_Reading_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* Pin_CM4_Writing */
#define Pin_CM4_Writing_0_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define Pin_CM4_Writing_0_INBUF_ENABLED 0u
#define Pin_CM4_Writing_0_INIT_DRIVESTATE 0u
#define Pin_CM4_Writing_0_INIT_MUXSEL 0u
#define Pin_CM4_Writing_0_INPUT_SYNC 2u
#define Pin_CM4_Writing_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define Pin_CM4_Writing_0_PORT GPIO_PRT0
#define Pin_CM4_Writing_0_SHIFT 4u
#define Pin_CM4_Writing_0 Pin_CM4_Writing_0_PORT, Pin_CM4_Writing_0_SHIFT
#define Pin_CM4_Writing_0_SLEWRATE CY_GPIO_SLEW_FAST
#define Pin_CM4_Writing_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* Red_LED */
#define Red_LED_0_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define Red_LED_0_INBUF_ENABLED 0u
#define Red_LED_0_INIT_DRIVESTATE 1u
#define Red_LED_0_INIT_MUXSEL 0u
#define Red_LED_0_INPUT_SYNC 2u
#define Red_LED_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define Red_LED_0_PORT GPIO_PRT0
#define Red_LED_0_SHIFT 3u
#define Red_LED_0 Red_LED_0_PORT, Red_LED_0_SHIFT
#define Red_LED_0_SLEWRATE CY_GPIO_SLEW_FAST
#define Red_LED_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* SPI_miso_s */
#define SPI_miso_s_0_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define SPI_miso_s_0_INBUF_ENABLED 0u
#define SPI_miso_s_0_INIT_DRIVESTATE 1u
#define SPI_miso_s_0_INIT_MUXSEL 20u
#define SPI_miso_s_0_INPUT_SYNC 2u
#define SPI_miso_s_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define SPI_miso_s_0_PORT GPIO_PRT12
#define SPI_miso_s_0_SHIFT 1u
#define SPI_miso_s_0 SPI_miso_s_0_PORT, SPI_miso_s_0_SHIFT
#define SPI_miso_s_0_SLEWRATE CY_GPIO_SLEW_FAST
#define SPI_miso_s_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* SPI_mosi_s */
#define SPI_mosi_s_0_DRIVEMODE CY_GPIO_DM_ANALOG
#define SPI_mosi_s_0_INBUF_ENABLED 1u
#define SPI_mosi_s_0_INIT_DRIVESTATE 1u
#define SPI_mosi_s_0_INIT_MUXSEL 20u
#define SPI_mosi_s_0_INPUT_SYNC 2u
#define SPI_mosi_s_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define SPI_mosi_s_0_PORT GPIO_PRT12
#define SPI_mosi_s_0_SHIFT 0u
#define SPI_mosi_s_0 SPI_mosi_s_0_PORT, SPI_mosi_s_0_SHIFT
#define SPI_mosi_s_0_SLEWRATE CY_GPIO_SLEW_FAST
#define SPI_mosi_s_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* SPI_sclk_s */
#define SPI_sclk_s_0_DRIVEMODE CY_GPIO_DM_ANALOG
#define SPI_sclk_s_0_INBUF_ENABLED 1u
#define SPI_sclk_s_0_INIT_DRIVESTATE 1u
#define SPI_sclk_s_0_INIT_MUXSEL 20u
#define SPI_sclk_s_0_INPUT_SYNC 2u
#define SPI_sclk_s_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define SPI_sclk_s_0_PORT GPIO_PRT12
#define SPI_sclk_s_0_SHIFT 2u
#define SPI_sclk_s_0 SPI_sclk_s_0_PORT, SPI_sclk_s_0_SHIFT
#define SPI_sclk_s_0_SLEWRATE CY_GPIO_SLEW_FAST
#define SPI_sclk_s_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* SPI_ss_s */
#define SPI_ss_s_0_DRIVEMODE CY_GPIO_DM_ANALOG
#define SPI_ss_s_0_INBUF_ENABLED 1u
#define SPI_ss_s_0_INIT_DRIVESTATE 1u
#define SPI_ss_s_0_INIT_MUXSEL 20u
#define SPI_ss_s_0_INPUT_SYNC 2u
#define SPI_ss_s_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define SPI_ss_s_0_PORT GPIO_PRT12
#define SPI_ss_s_0_SHIFT 3u
#define SPI_ss_s_0 SPI_ss_s_0_PORT, SPI_ss_s_0_SHIFT
#define SPI_ss_s_0_SLEWRATE CY_GPIO_SLEW_FAST
#define SPI_ss_s_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* UART_rx */
#define UART_rx_0_DRIVEMODE CY_GPIO_DM_ANALOG
#define UART_rx_0_INBUF_ENABLED 1u
#define UART_rx_0_INIT_DRIVESTATE 1u
#define UART_rx_0_INIT_MUXSEL 18u
#define UART_rx_0_INPUT_SYNC 2u
#define UART_rx_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define UART_rx_0_PORT GPIO_PRT5
#define UART_rx_0_SHIFT 0u
#define UART_rx_0 UART_rx_0_PORT, UART_rx_0_SHIFT
#define UART_rx_0_SLEWRATE CY_GPIO_SLEW_FAST
#define UART_rx_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* UART_tx */
#define UART_tx_0_DRIVEMODE CY_GPIO_DM_STRONG_IN_OFF
#define UART_tx_0_INBUF_ENABLED 0u
#define UART_tx_0_INIT_DRIVESTATE 1u
#define UART_tx_0_INIT_MUXSEL 18u
#define UART_tx_0_INPUT_SYNC 2u
#define UART_tx_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define UART_tx_0_PORT GPIO_PRT5
#define UART_tx_0_SHIFT 1u
#define UART_tx_0 UART_tx_0_PORT, UART_tx_0_SHIFT
#define UART_tx_0_SLEWRATE CY_GPIO_SLEW_FAST
#define UART_tx_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* White_Btn */
#define White_Btn_0_DRIVEMODE CY_GPIO_DM_ANALOG
#define White_Btn_0_INBUF_ENABLED 1u
#define White_Btn_0_INIT_DRIVESTATE 0u
#define White_Btn_0_INIT_MUXSEL 0u
#define White_Btn_0_INPUT_SYNC 2u
#define White_Btn_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define White_Btn_0_PORT GPIO_PRT13
#define White_Btn_0_SHIFT 1u
#define White_Btn_0 White_Btn_0_PORT, White_Btn_0_SHIFT
#define White_Btn_0_SLEWRATE CY_GPIO_SLEW_FAST
#define White_Btn_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

/* Yellow_Btn */
#define Yellow_Btn_0_DRIVEMODE CY_GPIO_DM_ANALOG
#define Yellow_Btn_0_INBUF_ENABLED 1u
#define Yellow_Btn_0_INIT_DRIVESTATE 0u
#define Yellow_Btn_0_INIT_MUXSEL 0u
#define Yellow_Btn_0_INPUT_SYNC 2u
#define Yellow_Btn_0_INTERRUPT_MODE CY_GPIO_INTR_DISABLE
#define Yellow_Btn_0_PORT GPIO_PRT0
#define Yellow_Btn_0_SHIFT 2u
#define Yellow_Btn_0 Yellow_Btn_0_PORT, Yellow_Btn_0_SHIFT
#define Yellow_Btn_0_SLEWRATE CY_GPIO_SLEW_FAST
#define Yellow_Btn_0_THRESHOLD_LEVEL CY_GPIO_VTRIP_CMOS

#endif /* INCLUDED_CYFITTER_GPIO_H */
