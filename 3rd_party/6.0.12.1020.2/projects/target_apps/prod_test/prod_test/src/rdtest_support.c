/**
 ****************************************************************************************
 *
 * @file rdtest_support.c
 *
 * @brief RD test support functions source code file.
 *
 * Copyright (c) 2017-2019 Dialog Semiconductor. All rights reserved.
 *
 * This software ("Software") is owned by Dialog Semiconductor.
 *
 * By using this Software you agree that Dialog Semiconductor retains all
 * intellectual property and proprietary rights in and to this Software and any
 * use, reproduction, disclosure or distribution of the Software without express
 * written permission or a license agreement from Dialog Semiconductor is
 * strictly prohibited. This Software is solely for use on or in conjunction
 * with Dialog Semiconductor products.
 *
 * EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, THE
 * SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE
 * PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, IN NO EVENT SHALL
 * DIALOG SEMICONDUCTOR BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THE SOFTWARE.
 *
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rdtest_support.h" 

void delay_systick_us(unsigned long int delay) //delay in units of 1 usec
{
    delay *= 16;            // see explanation above this function

    if (delay>0xFFFFFF)
    {
        delay = 0xFFFFFF;   // limit, max 24 bits
    }
    SysTick->CTRL = 0;      // Disable SysTick
    SysTick->LOAD = delay;  // Count down from delay eg. 999 to 0
    SysTick->VAL  = 0;      // Clear current value to 0
    SysTick->CTRL = 0x5;    // Enable SysTick, disable SysTick exception and use processor clock
    while ((SysTick->CTRL & 0x10000) == 0);
}
