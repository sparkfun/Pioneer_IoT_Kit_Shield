;/**************************************************************************//**
; * @file     startup_psoc6ble_cm0plus.s
; * @brief    CMSIS Core Device Startup File for
; *           ARMCM0plus Device Series
; * @version  V5.00
; * @date     08. March 2016
; ******************************************************************************/
;/*
; * Copyright (c) 2009-2016 ARM Limited. All rights reserved.
; *
; * SPDX-License-Identifier: Apache-2.0
; *
; * Licensed under the Apache License, Version 2.0 (the License); you may
; * not use this file except in compliance with the License.
; * You may obtain a copy of the License at
; *
; * www.apache.org/licenses/LICENSE-2.0
; *
; * Unless required by applicable law or agreed to in writing, software
; * distributed under the License is distributed on an AS IS BASIS, WITHOUT
; * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
; * See the License for the specific language governing permissions and
; * limitations under the License.
; */

;
; The modules in this file are included in the libraries, and may be replaced
; by any user-defined modules that define the PUBLIC symbol _program_start or
; a user defined start symbol.
; To override the cstartup defined in the library, simply add your modified
; version to the workbench project.
;
; The vector table is normally located at address 0.
; When debugging in RAM, it can be located in RAM, aligned to at least 2^6.
; The name "__vector_table" has special meaning for C-SPY:
; it is where the SP start value is found, and the NVIC vector
; table register (VTOR) is initialized to this address if != 0.
;
; Cortex-M version
;

        MODULE  ?cstartup

        ;; Forward declaration of sections.
        SECTION CSTACK:DATA:NOROOT(3)
        SECTION .intvec_ram:DATA:NOROOT(2)
        SECTION .intvec:CODE:NOROOT(2)

        EXTERN  __iar_program_start
        EXTERN  SystemInit
        EXTERN __iar_data_init3
        PUBLIC  __vector_table
        PUBLIC  __vector_table_0x1c
        PUBLIC  __Vectors
        PUBLIC  __Vectors_End
        PUBLIC  __Vectors_Size
        PUBLIC  __ramVectors

        DATA

__vector_table
        DCD     sfe(CSTACK)
        DCD     Reset_Handler

        DCD     0x0000000D      ; MI_Handler is defined in ROM code
        DCD     HardFault_Handler
        DCD     0
        DCD     0
        DCD     0
__vector_table_0x1c
        DCD     0
        DCD     0
        DCD     0
        DCD     0
        DCD     SVC_Handler
        DCD     0
        DCD     0
        DCD     PendSV_Handler
        DCD     SysTick_Handler

        ; External interrupts                         Power Mode  Description
        DCD     ioss_interrupts_gpio_0_IRQHandler      ; DeepSleep   GPIO Port Interrupt #0
        DCD     ioss_interrupts_gpio_1_IRQHandler      ; DeepSleep   GPIO Port Interrupt #1
        DCD     ioss_interrupts_gpio_2_IRQHandler      ; DeepSleep   GPIO Port Interrupt #2
        DCD     ioss_interrupts_gpio_3_IRQHandler      ; DeepSleep   GPIO Port Interrupt #3
        DCD     ioss_interrupts_gpio_4_IRQHandler      ; DeepSleep   GPIO Port Interrupt #4
        DCD     ioss_interrupts_gpio_5_IRQHandler      ; DeepSleep   GPIO Port Interrupt #5
        DCD     ioss_interrupts_gpio_6_IRQHandler      ; DeepSleep   GPIO Port Interrupt #6
        DCD     ioss_interrupts_gpio_7_IRQHandler      ; DeepSleep   GPIO Port Interrupt #7
        DCD     ioss_interrupts_gpio_8_IRQHandler      ; DeepSleep   GPIO Port Interrupt #8
        DCD     ioss_interrupts_gpio_9_IRQHandler      ; DeepSleep   GPIO Port Interrupt #9
        DCD     ioss_interrupts_gpio_10_IRQHandler     ; DeepSleep   GPIO Port Interrupt #10
        DCD     ioss_interrupts_gpio_11_IRQHandler     ; DeepSleep   GPIO Port Interrupt #11
        DCD     ioss_interrupts_gpio_12_IRQHandler     ; DeepSleep   GPIO Port Interrupt #12
        DCD     ioss_interrupts_gpio_13_IRQHandler     ; DeepSleep   GPIO Port Interrupt #13
        DCD     ioss_interrupts_gpio_14_IRQHandler     ; DeepSleep   GPIO Port Interrupt #14
        DCD     ioss_interrupt_gpio_IRQHandler         ; DeepSleep   GPIO All Ports
        DCD     ioss_interrupt_vdd_IRQHandler          ; DeepSleep   GPIO Supply Detect Interrupt
        DCD     lpcomp_interrupt_IRQHandler            ; DeepSleep   Low Power Comparator Interrupt
        DCD     scb_8_interrupt_IRQHandler             ; DeepSleep   Serial Communication Block #8 (DeepSleep cap)
        DCD     srss_interrupt_mcwdt_0_IRQHandler      ; DeepSleep   Multi Counter Watchdog Timer interrupt
        DCD     srss_interrupt_mcwdt_1_IRQHandler      ; DeepSleep   Multi Counter Watchdog Timer interrupt
        DCD     srss_interrupt_backup_IRQHandler       ; DeepSleep   Backup domain interrupt
        DCD     srss_interrupt_IRQHandler              ; DeepSleep   Other combined Interrupts for SRSS (LVD, WDT
        DCD     pass_interrupt_ctbs_IRQHandler         ; DeepSleep   CTBm Interrupt (all CTBms)
        DCD     bless_interrupt_IRQHandler             ; DeepSleep   Bluetooth Radio interrupt
        DCD     cpuss_interrupts_ipc_0_IRQHandler      ; DeepSleep   CPUSS Inter Process Communication Interrupt
        DCD     cpuss_interrupts_ipc_1_IRQHandler      ; DeepSleep   CPUSS Inter Process Communication Interrupt
        DCD     cpuss_interrupts_ipc_2_IRQHandler      ; DeepSleep   CPUSS Inter Process Communication Interrupt
        DCD     cpuss_interrupts_ipc_3_IRQHandler      ; DeepSleep   CPUSS Inter Process Communication Interrupt
        DCD     cpuss_interrupts_ipc_4_IRQHandler      ; DeepSleep   CPUSS Inter Process Communication Interrupt
        DCD     cpuss_interrupts_ipc_5_IRQHandler      ; DeepSleep   CPUSS Inter Process Communication Interrupt

__Vectors_End

__Vectors       EQU   __vector_table
__Vectors_Size  EQU   __Vectors_End - __Vectors

        SECTION .intvec_ram:DATA:REORDER:NOROOT(2)
__ramVectors
        DS32     __Vectors_Size


        THUMB

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Default handlers
;;
        PUBWEAK Default_Handler
        SECTION .text:CODE:REORDER:NOROOT(2)
Default_Handler
        B Default_Handler


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Saves and disables the interrupts
;;
        PUBLIC Cy_SaveIRQ
        SECTION .text:CODE:REORDER:NOROOT(2)
Cy_SaveIRQ
        MRS r0, PRIMASK
        CPSID I
        BX LR


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Restores the interrupts
;;
        PUBLIC Cy_RestoreIRQ
        SECTION .text:CODE:REORDER:NOROOT(2)
Cy_RestoreIRQ
        MSR PRIMASK, r0
        BX LR


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Weak function for startup customization
;;
        PUBWEAK Cy_OnResetUser
        SECTION .text:CODE:REORDER:NOROOT(2)
Cy_OnResetUser
        BX LR

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Define strong version to return zero for
;; __iar_program_start to skip data sections
;; initialization.
;;
        PUBLIC __low_level_init
        SECTION .text:CODE:REORDER:NOROOT(2)
__low_level_init
        MOVS R0, #0
        BX LR

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Default interrupt handlers.
;;
        THUMB
        PUBWEAK Reset_Handler
        SECTION .text:CODE:REORDER:NOROOT(2)
Reset_Handler

        ; Define strong function for startup customization
        LDR     R0, =Cy_OnResetUser
        BLX     R0

        ; Disable global interrupts
        CPSID I

        ; Copy vectors from ROM to RAM
        LDR r1, =__vector_table
        LDR r0, =__ramVectors
        LDR r2, =__Vectors_Size
intvec_copy
        LDR r3, [r1]
        STR r3, [r0]
        ADDS r0, r0, #4
        ADDS r1, r1, #4
        SUBS r2, r2, #1
        CMP r2, #0
        BNE intvec_copy

        ; Update Vector Table Offset Register
        LDR r0, =__ramVectors
        LDR r1, =0xE000ED08
        STR r0, [r1]
        dsb

        ; Initialize data sections
        LDR     R0, =__iar_data_init3
        BLX     R0

        LDR     R0, =SystemInit
        BLX     R0

        LDR     R0, =__iar_program_start
        BLX     R0

; Should never get here
Cy_Main_Exited
        B Cy_Main_Exited


        PUBWEAK NMI_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
NMI_Handler
        B NMI_Handler


        PUBWEAK Cy_SysLib_FaultHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
Cy_SysLib_FaultHandler
        B Cy_SysLib_FaultHandler

        PUBWEAK HardFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
HardFault_Handler
        IMPORT Cy_SysLib_FaultHandler
        movs r0, #4
        mov r1, LR
        tst r0, r1
        beq L_MSP
        mrs r0, PSP
        b L_API_call
L_MSP
        mrs r0, MSP
L_API_call
        ; Storing LR content for Creator call stack trace
        push {LR}
        bl Cy_SysLib_FaultHandler


        PUBWEAK SVC_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SVC_Handler
        B SVC_Handler

        PUBWEAK PendSV_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
PendSV_Handler
        B PendSV_Handler

        PUBWEAK SysTick_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SysTick_Handler
        B SysTick_Handler


        ; External interrupts
        PUBWEAK ioss_interrupts_gpio_0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
ioss_interrupts_gpio_0_IRQHandler
        B ioss_interrupts_gpio_0_IRQHandler

        PUBWEAK ioss_interrupts_gpio_1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
ioss_interrupts_gpio_1_IRQHandler
        B ioss_interrupts_gpio_1_IRQHandler

        PUBWEAK ioss_interrupts_gpio_2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
ioss_interrupts_gpio_2_IRQHandler
        B ioss_interrupts_gpio_2_IRQHandler

        PUBWEAK ioss_interrupts_gpio_3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
ioss_interrupts_gpio_3_IRQHandler
        B ioss_interrupts_gpio_3_IRQHandler

        PUBWEAK ioss_interrupts_gpio_4_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
ioss_interrupts_gpio_4_IRQHandler
        B ioss_interrupts_gpio_4_IRQHandler

        PUBWEAK ioss_interrupts_gpio_5_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
ioss_interrupts_gpio_5_IRQHandler
        B ioss_interrupts_gpio_5_IRQHandler

        PUBWEAK ioss_interrupts_gpio_6_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
ioss_interrupts_gpio_6_IRQHandler
        B ioss_interrupts_gpio_6_IRQHandler

        PUBWEAK ioss_interrupts_gpio_7_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
ioss_interrupts_gpio_7_IRQHandler
        B ioss_interrupts_gpio_7_IRQHandler

        PUBWEAK ioss_interrupts_gpio_8_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
ioss_interrupts_gpio_8_IRQHandler
        B ioss_interrupts_gpio_8_IRQHandler

        PUBWEAK ioss_interrupts_gpio_9_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
ioss_interrupts_gpio_9_IRQHandler
        B ioss_interrupts_gpio_9_IRQHandler

        PUBWEAK ioss_interrupts_gpio_10_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
ioss_interrupts_gpio_10_IRQHandler
        B ioss_interrupts_gpio_10_IRQHandler

        PUBWEAK ioss_interrupts_gpio_11_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
ioss_interrupts_gpio_11_IRQHandler
        B ioss_interrupts_gpio_11_IRQHandler

        PUBWEAK ioss_interrupts_gpio_12_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
ioss_interrupts_gpio_12_IRQHandler
        B ioss_interrupts_gpio_12_IRQHandler

        PUBWEAK ioss_interrupts_gpio_13_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
ioss_interrupts_gpio_13_IRQHandler
        B ioss_interrupts_gpio_13_IRQHandler

        PUBWEAK ioss_interrupts_gpio_14_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
ioss_interrupts_gpio_14_IRQHandler
        B ioss_interrupts_gpio_14_IRQHandler

        PUBWEAK ioss_interrupt_gpio_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
ioss_interrupt_gpio_IRQHandler
        B ioss_interrupt_gpio_IRQHandler

        PUBWEAK ioss_interrupt_vdd_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
ioss_interrupt_vdd_IRQHandler
        B ioss_interrupt_vdd_IRQHandler

        PUBWEAK lpcomp_interrupt_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
lpcomp_interrupt_IRQHandler
        B lpcomp_interrupt_IRQHandler

        PUBWEAK scb_8_interrupt_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
scb_8_interrupt_IRQHandler
        B scb_8_interrupt_IRQHandler

        PUBWEAK srss_interrupt_mcwdt_0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
srss_interrupt_mcwdt_0_IRQHandler
        B srss_interrupt_mcwdt_0_IRQHandler

        PUBWEAK srss_interrupt_mcwdt_1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
srss_interrupt_mcwdt_1_IRQHandler
        B srss_interrupt_mcwdt_1_IRQHandler

        PUBWEAK srss_interrupt_backup_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
srss_interrupt_backup_IRQHandler
        B srss_interrupt_backup_IRQHandler

        PUBWEAK srss_interrupt_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
srss_interrupt_IRQHandler
        B srss_interrupt_IRQHandler

        PUBWEAK pass_interrupt_ctbs_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
pass_interrupt_ctbs_IRQHandler
        B pass_interrupt_ctbs_IRQHandler

        PUBWEAK bless_interrupt_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
bless_interrupt_IRQHandler
        B bless_interrupt_IRQHandler

        PUBWEAK cpuss_interrupts_ipc_0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
cpuss_interrupts_ipc_0_IRQHandler
        B cpuss_interrupts_ipc_0_IRQHandler

        PUBWEAK cpuss_interrupts_ipc_1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
cpuss_interrupts_ipc_1_IRQHandler
        B cpuss_interrupts_ipc_1_IRQHandler

        PUBWEAK cpuss_interrupts_ipc_2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
cpuss_interrupts_ipc_2_IRQHandler
        B cpuss_interrupts_ipc_2_IRQHandler

        PUBWEAK cpuss_interrupts_ipc_3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
cpuss_interrupts_ipc_3_IRQHandler
        B cpuss_interrupts_ipc_3_IRQHandler

        PUBWEAK cpuss_interrupts_ipc_4_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
cpuss_interrupts_ipc_4_IRQHandler
        B cpuss_interrupts_ipc_4_IRQHandler

        PUBWEAK cpuss_interrupts_ipc_5_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
cpuss_interrupts_ipc_5_IRQHandler
        B cpuss_interrupts_ipc_5_IRQHandler


        END
