;/**************************************************************************//**
; * @file     startup_psoc6ble_cm0plus.s
; * @brief    CMSIS Core Device Startup File for
; *           ARMCM0plus Device Series
; * @version  V5.00
; * @date     02. March 2016
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

;/*
;//-------- <<< Use Configuration Wizard in Context Menu >>> ------------------
;*/

; Address of the NMI handler
CY_NMI_HANLDER_ADDR EQU 0x0000000D

; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>
                IF :DEF:__STACK_SIZE
Stack_Size      EQU     __STACK_SIZE
                ELSE
Stack_Size      EQU     0x00001000
                ENDIF
                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp

; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>
                IF :DEF:__HEAP_SIZE
Heap_Size       EQU     __HEAP_SIZE
                ELSE
Heap_Size       EQU     0x00000400
                ENDIF
                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit


                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset

                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

__Vectors       DCD     __initial_sp              ; Top of Stack
                DCD     Reset_Handler             ; Reset Handler

                DCD     CY_NMI_HANLDER_ADDR       ; NMI Handler located at ROM code
                DCD     HardFault_Handler         ; Hard Fault Handler
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     SVC_Handler               ; SVCall Handler
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     PendSV_Handler            ; PendSV Handler
                DCD     SysTick_Handler           ; SysTick Handler


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
                DCD     scb_8_interrupt_IRQHandler             ; DeepSleep   Serial Communication Block #8 (DeepSleep capable)
                DCD     srss_interrupt_mcwdt_0_IRQHandler      ; DeepSleep   Multi Counter Watchdog Timer interrupt
                DCD     srss_interrupt_mcwdt_1_IRQHandler      ; DeepSleep   Multi Counter Watchdog Timer interrupt
                DCD     srss_interrupt_backup_IRQHandler       ; DeepSleep   Backup domain interrupt
                DCD     srss_interrupt_IRQHandler              ; DeepSleep   Other combined Interrupts for SRSS (LVD, WDT, CLKCAL)
                DCD     pass_interrupt_ctbs_IRQHandler         ; DeepSleep   CTBm Interrupt (all CTBms)
                DCD     bless_interrupt_IRQHandler             ; DeepSleep   Bluetooth Radio interrupt
                DCD     cpuss_interrupts_ipc_0_IRQHandler      ; DeepSleep   CPUSS Inter Process Communication Interrupt #0
                DCD     cpuss_interrupts_ipc_1_IRQHandler      ; DeepSleep   CPUSS Inter Process Communication Interrupt #1
                DCD     cpuss_interrupts_ipc_2_IRQHandler      ; DeepSleep   CPUSS Inter Process Communication Interrupt #2
                DCD     cpuss_interrupts_ipc_3_IRQHandler      ; DeepSleep   CPUSS Inter Process Communication Interrupt #3
                DCD     cpuss_interrupts_ipc_4_IRQHandler      ; DeepSleep   CPUSS Inter Process Communication Interrupt #4
                DCD     cpuss_interrupts_ipc_5_IRQHandler      ; DeepSleep   CPUSS Inter Process Communication Interrupt #5
                DCD     cpuss_interrupts_ipc_6_IRQHandler      ; DeepSleep   CPUSS Inter Process Communication Interrupt #6

__Vectors_End

__Vectors_Size  EQU     __Vectors_End - __Vectors
                EXPORT __ramVectors
                AREA    RESET_RAM, READWRITE, NOINIT
__ramVectors    SPACE   __Vectors_Size


                AREA    |.text|, CODE, READONLY


; Saves and disables the interrupts
Cy_SaveIRQ       PROC
                EXPORT Cy_SaveIRQ
                MRS r0, PRIMASK
                CPSID I
                BX LR
                ENDP


; Restores the interrupts
Cy_RestoreIRQ    PROC
                EXPORT Cy_RestoreIRQ
                MSR PRIMASK, r0
                BX LR
                ENDP


; Weak function for startup customization
Cy_OnResetUser PROC
                EXPORT  Cy_OnResetUser             [WEAK]
                BX      LR
                ENDP

; Reset Handler
Reset_Handler   PROC
                EXPORT  Reset_Handler               [WEAK]
                IMPORT  SystemInit
                IMPORT  __main

                ; Define strong function for startup customization
                BL      Cy_OnResetUser

                ; Disable global interrupts
                CPSID I

                ; Copy vectors from ROM to RAM
                LDR r1, =__Vectors
                LDR r0, =__ramVectors
                LDR r2, =__Vectors_Size
Vectors_Copy
                LDR r3, [r1]
                STR r3, [r0]
                ADDS r0, r0, #4
                ADDS r1, r1, #4
                SUBS r2, r2, #1
                CMP r2, #0
                BNE Vectors_Copy

                ; Update Vector Table Offset Register. */
                LDR r0, =__ramVectors
                LDR r1, =0xE000ED08
                STR r0, [r1]
                dsb 0xF

                LDR     R0, =__main
                BLX     R0

                ; Should never get here
                B       .

                ENDP

; $Sub$$main
|$Sub$$main|    PROC
                EXPORT  |$Sub$$main|
                IMPORT  |$Super$$main|

                LDR     R0, =SystemInit
                BLX     R0

                LDR     R0, =|$Super$$main|
                BX      R0
                ENDP

; Dummy Exception Handlers (infinite loops which can be modified)
NMI_Handler         PROC
                    EXPORT  NMI_Handler               [WEAK]
                    B       .
                    ENDP

Cy_SysLib_FaultHandler PROC
                    EXPORT  Cy_SysLib_FaultHandler    [WEAK]
                    B       .
                    ENDP

HardFault_Handler   PROC
                    EXPORT HardFault_Handler          [WEAK]
                    movs r0, #4
                    mov r1, LR
                    tst r0, r1
                    beq L_MSP
                    mrs r0, PSP
                    bl L_API_call
L_MSP
                    mrs r0, MSP
L_API_call
                    bl Cy_SysLib_FaultHandler
                    ENDP

SVC_Handler         PROC
                    EXPORT  SVC_Handler               [WEAK]
                    B       .
                    ENDP
PendSV_Handler      PROC
                    EXPORT  PendSV_Handler            [WEAK]
                    B       .
                    ENDP
SysTick_Handler     PROC
                    EXPORT  SysTick_Handler           [WEAK]
                    B       .
                    ENDP

Default_Handler PROC
                    EXPORT      Default_Handler                        [WEAK]
                    EXPORT      ioss_interrupts_gpio_0_IRQHandler      [WEAK]
                    EXPORT      ioss_interrupts_gpio_1_IRQHandler      [WEAK]
                    EXPORT      ioss_interrupts_gpio_2_IRQHandler      [WEAK]
                    EXPORT      ioss_interrupts_gpio_3_IRQHandler      [WEAK]
                    EXPORT      ioss_interrupts_gpio_4_IRQHandler      [WEAK]
                    EXPORT      ioss_interrupts_gpio_5_IRQHandler      [WEAK]
                    EXPORT      ioss_interrupts_gpio_6_IRQHandler      [WEAK]
                    EXPORT      ioss_interrupts_gpio_7_IRQHandler      [WEAK]
                    EXPORT      ioss_interrupts_gpio_8_IRQHandler      [WEAK]
                    EXPORT      ioss_interrupts_gpio_9_IRQHandler      [WEAK]
                    EXPORT      ioss_interrupts_gpio_10_IRQHandler     [WEAK]
                    EXPORT      ioss_interrupts_gpio_11_IRQHandler     [WEAK]
                    EXPORT      ioss_interrupts_gpio_12_IRQHandler     [WEAK]
                    EXPORT      ioss_interrupts_gpio_13_IRQHandler     [WEAK]
                    EXPORT      ioss_interrupts_gpio_14_IRQHandler     [WEAK]
                    EXPORT      ioss_interrupt_gpio_IRQHandler         [WEAK]
                    EXPORT      ioss_interrupt_vdd_IRQHandler          [WEAK]
                    EXPORT      lpcomp_interrupt_IRQHandler            [WEAK]
                    EXPORT      scb_8_interrupt_IRQHandler             [WEAK]
                    EXPORT      srss_interrupt_mcwdt_0_IRQHandler      [WEAK]
                    EXPORT      srss_interrupt_mcwdt_1_IRQHandler      [WEAK]
                    EXPORT      srss_interrupt_backup_IRQHandler       [WEAK]
                    EXPORT      srss_interrupt_IRQHandler              [WEAK]
                    EXPORT      pass_interrupt_ctbs_IRQHandler         [WEAK]
                    EXPORT      bless_interrupt_IRQHandler             [WEAK]
                    EXPORT      cpuss_interrupts_ipc_0_IRQHandler      [WEAK]
                    EXPORT      cpuss_interrupts_ipc_1_IRQHandler      [WEAK]
                    EXPORT      cpuss_interrupts_ipc_2_IRQHandler      [WEAK]
                    EXPORT      cpuss_interrupts_ipc_3_IRQHandler      [WEAK]
                    EXPORT      cpuss_interrupts_ipc_4_IRQHandler      [WEAK]
                    EXPORT      cpuss_interrupts_ipc_5_IRQHandler      [WEAK]
                    EXPORT      cpuss_interrupts_ipc_6_IRQHandler      [WEAK]

ioss_interrupts_gpio_0_IRQHandler
ioss_interrupts_gpio_1_IRQHandler
ioss_interrupts_gpio_2_IRQHandler
ioss_interrupts_gpio_3_IRQHandler
ioss_interrupts_gpio_4_IRQHandler
ioss_interrupts_gpio_5_IRQHandler
ioss_interrupts_gpio_6_IRQHandler
ioss_interrupts_gpio_7_IRQHandler
ioss_interrupts_gpio_8_IRQHandler
ioss_interrupts_gpio_9_IRQHandler
ioss_interrupts_gpio_10_IRQHandler
ioss_interrupts_gpio_11_IRQHandler
ioss_interrupts_gpio_12_IRQHandler
ioss_interrupts_gpio_13_IRQHandler
ioss_interrupts_gpio_14_IRQHandler
ioss_interrupt_gpio_IRQHandler
ioss_interrupt_vdd_IRQHandler
lpcomp_interrupt_IRQHandler
scb_8_interrupt_IRQHandler
srss_interrupt_mcwdt_0_IRQHandler
srss_interrupt_mcwdt_1_IRQHandler
srss_interrupt_backup_IRQHandler
srss_interrupt_IRQHandler
pass_interrupt_ctbs_IRQHandler
bless_interrupt_IRQHandler
cpuss_interrupts_ipc_0_IRQHandler
cpuss_interrupts_ipc_1_IRQHandler
cpuss_interrupts_ipc_2_IRQHandler
cpuss_interrupts_ipc_3_IRQHandler
cpuss_interrupts_ipc_4_IRQHandler
cpuss_interrupts_ipc_5_IRQHandler
cpuss_interrupts_ipc_6_IRQHandler
                B       .
                ENDP

                ALIGN


; User Initial Stack & Heap

                IF      :DEF:__MICROLIB

                EXPORT  __initial_sp
                EXPORT  __heap_base
                EXPORT  __heap_limit

                ELSE

                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap

__user_initial_stackheap PROC
                LDR     R0, =Heap_Mem
                LDR     R1, =(Stack_Mem + Stack_Size)
                LDR     R2, =(Heap_Mem +  Heap_Size)
                LDR     R3, =Stack_Mem
                BX      LR
                ENDP

                ALIGN

                ENDIF


                END
