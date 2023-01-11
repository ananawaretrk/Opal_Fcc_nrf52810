/**
 ****************************************************************************************
 *
 * @file main.c
 *
 * @brief Battery level example
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
#include "gpio.h"
#include "uart.h"
#include "uart_utils.h"
#include "battery.h"
#if defined (__DA14531__)
#include "datasheet.h"
#include "hw_otpc.h"
#include "syscntl.h"
#include "otp_hdr.h"
#include "otp_cs.h"
#include "arch.h"
#endif

/**
 ****************************************************************************************
 * @brief Battery Level Indication example function
 * @return void
 ****************************************************************************************
 */
static void batt_test(void);

/**
 ****************************************************************************************
 * @brief Main routine of the battery level example
 * @return void
 ****************************************************************************************
 */
int main (void)
{
    system_init();
    batt_test();
    while(1);
}

void batt_test(void)
{
    printf_string(UART, "\n\r\n\r");
    printf_string(UART, "*******************\n\r");
    printf_string(UART, "* 3V BATTERY TEST *\n\r");
    printf_string(UART, "*******************\n\r");

    printf_string(UART, "\n\rBattery type: CR2032");
    printf_string(UART, "\n\rCurrent battery level (%): ");
    printf_byte_dec(UART, battery_get_lvl(BATT_CR2032));
    printf_string(UART, "% left");
    printf_string(UART, "\n\rEnd of test\n\r");
}
