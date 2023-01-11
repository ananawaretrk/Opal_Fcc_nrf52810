;/**
; ****************************************************************************************
; *
; * @file pulse.s
; *
; * @brief
; *
; * Copyright (c) 2017-2019 Dialog Semiconductor. All rights reserved.
; *
; * This software ("Software") is owned by Dialog Semiconductor.
; *
; * By using this Software you agree that Dialog Semiconductor retains all
; * intellectual property and proprietary rights in and to this Software and any
; * use, reproduction, disclosure or distribution of the Software without express
; * written permission or a license agreement from Dialog Semiconductor is
; * strictly prohibited. This Software is solely for use on or in conjunction
; * with Dialog Semiconductor products.
; *
; * EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, THE
; * SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
; * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
; * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE
; * PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, IN NO EVENT SHALL
; * DIALOG SEMICONDUCTOR BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL,
; * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
; * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
; * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
; * OF THE SOFTWARE.
; *
; ****************************************************************************************
; */

GUARD_COUNTER_INIT  EQU 3000000 ; 3000000 iterations x 9 cycles/iteration * 62.5 ns/cycle = 1.6875 seconds

SYSTICK_CTRL_ADDR   EQU 0xE000E010 ; (R/W)  SysTick Control and Status Register
SYSTICK_LOAD_ADDR   EQU 0xE000E014 ; (R/W)  SysTick Reload Value Register
SYSTICK_VAL_ADDR    EQU 0xE000E018 ; (R/W)  SysTick Current Value Register
SYSTICK_CALIB_ADDR  EQU 0xE000E01C ; (R/ )  SysTick Calibration Register


                THUMB

                AREA    |.text|, CODE, READONLY


measure_pulse   PROC
                EXPORT  measure_pulse

                push {r1-r5, lr}

                LDR      r5, =GUARD_COUNTER_INIT ; guard counter initialization
wait_low
                SUBS     r5, r5, #1              ; decrement guard counter 
                BEQ      abort                   ; abort if guard counter has reached zero
                LDRH     r2,[r0]
                TST      r2, r1
                BNE      wait_low

                ; detect positive edge
wait_high
                SUBS     r5, r5, #1              ; decrement guard counter 
                BEQ      abort                   ; abort if guard counter has reached zero
                LDRH     r2, [r0]
                TST      r2, r1
                BEQ      wait_high

                ; start systick timer
                LDR      r2, =SYSTICK_CTRL_ADDR
                LDR      r3, [r2]
                MOVS     r4, #1
                ORRS     r3,r3,r4
                STR      r3, [r2]

                ; detect negative edge
while_high
                SUBS     r5, r5, #1              ; decrement guard counter 
                BEQ      abort                   ; abort if guard counter has reached zero
                LDRH     r2,[r0]
                TST      r2, r1
                BNE      while_high

                ; read and return current systick value
                LDR      r2, =SYSTICK_VAL_ADDR
                LDR      r0, [r2]
                POP      {r1-r5, pc}

abort
                MOVS    r0, #0
                POP     {r1-r5, pc}
                ALIGN

                ENDP

                END
