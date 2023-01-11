/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief SysTick example
 *
 * Copyright (c) 2012-2019 Dialog Semiconductor. All rights reserved.
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
#include <stdio.h>
#include "arch_system.h"
#include "user_periph_setup.h"
#include "systick.h"
#include "gpio.h"

#define SYSTICK_PERIOD_US   1000000     // period for systick timer in us, so 1000000ticks = 1second
#define SYSTICK_EXCEPTION   1           // generate systick exceptions
int i = 0;

/**
 ****************************************************************************************
 * @brief SysTick Test routine
 * @return void
 ****************************************************************************************
 */
void systick_test(void);

/**
 ****************************************************************************************
 * @brief Main routine of the SysTick example
 * @return void
 ****************************************************************************************
 */
int main (void)
{
    system_init();
    periph_init();
    systick_test();
    while(1);
}

/**
 ****************************************************************************************
 * @brief SysTick ISR routine
 * @return void
 ****************************************************************************************
 */
static void systick_isr(void)
{
    if (i == 0)
    {
        GPIO_SetActive(LED_PORT, LED_PIN);
        i = 1;
    }
    else
    {
        GPIO_SetInactive(LED_PORT, LED_PIN);
        i = 0;
    }
}


void systick_test(void)
{
    systick_register_callback(systick_isr);
    // Systick will be initialized to use a reference clock frequency of 1 MHz
    systick_start(SYSTICK_PERIOD_US, SYSTICK_EXCEPTION);
}
