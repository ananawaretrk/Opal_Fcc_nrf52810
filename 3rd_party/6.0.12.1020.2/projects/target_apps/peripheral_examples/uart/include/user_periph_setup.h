/**
 ****************************************************************************************
 *
 * @file user_periph_setup.h
 *
 * @brief Peripherals setup header file.
 *
 * Copyright (c) 2015-2019 Dialog Semiconductor. All rights reserved.
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

#ifndef _USER_PERIPH_SETUP_H_
#define _USER_PERIPH_SETUP_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "gpio.h"
#include "uart.h"

/*
 * DEFINES
 ****************************************************************************************
 */
// Define UART1, UART2 Pads
#if defined (__DA14531__)
    #define UART1_TX_PORT           GPIO_PORT_0
    #define UART1_TX_PIN            GPIO_PIN_6
    #define UART1_RX_PORT           GPIO_PORT_0
    #define UART1_RX_PIN            GPIO_PIN_7

    #define UART2_TX_PORT           GPIO_PORT_0
    #define UART2_TX_PIN            GPIO_PIN_6
    #define UART2_RX_PORT           GPIO_PORT_0
    #define UART2_RX_PIN            GPIO_PIN_7

#else // DA14585, DA14586
    #define UART1_TX_PORT           GPIO_PORT_0
    #define UART1_TX_PIN            GPIO_PIN_4
    #define UART1_RX_PORT           GPIO_PORT_0
    #define UART1_RX_PIN            GPIO_PIN_5

    #define UART2_TX_PORT           GPIO_PORT_0
    #define UART2_TX_PIN            GPIO_PIN_4
    #define UART2_RX_PORT           GPIO_PORT_0
    #define UART2_RX_PIN            GPIO_PIN_5
#endif

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initializes application's peripherals and pins
 * @return void
 ****************************************************************************************
 */
void periph_init(void);

/**
 ****************************************************************************************
 * @brief Initializes UART Peripheral (Pads and Configuration)
 * @param[in] uart_id           Identifies which UART to use
 ****************************************************************************************
 */
void uart_periph_init(uart_t *uart);

#endif // _USER_PERIPH_SETUP_H_
