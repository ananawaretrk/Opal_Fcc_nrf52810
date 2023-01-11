/**
 ****************************************************************************************
 *
 * @file uart_common.h
 *
 * @brief UART examples common header file.
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

#ifndef _UART_COMMON_H
#define _UART_COMMON_H

#include "uart.h"

// Send examples functions

/**
 ****************************************************************************************
 * @brief UART send in blocking mode example.
 * @param[in] uart_id           Identifies which UART to use
 ****************************************************************************************
 */
void uart_send_blocking_example(uart_t* uart);

/**
 ****************************************************************************************
 * @brief UART send in interrupt mode example.
 * @param[in] uart_id           Identifies which UART to use
 ****************************************************************************************
 */
void uart_send_interrupt_example(uart_t* uart);

/**
 ****************************************************************************************
 * @brief UART send in DMA mode example.
 * @param[in] uart_id           Identifies which UART to use
 ****************************************************************************************
 */
void uart_send_dma_example(uart_t* uart);

// Receive examples functions
/**
 ****************************************************************************************
 * @brief UART receive in blocking mode example.
 * @param[in] uart_id           Identifies which UART to use
 ****************************************************************************************
 */
void uart_receive_blocking_example(uart_t* uart);

/**
 ****************************************************************************************
 * @brief UART receive in interrupt mode example.
 * @param[in] uart_id           Identifies which UART to use
 ****************************************************************************************
 */
void uart_receive_interrupt_example(uart_t* uart);

/**
 ****************************************************************************************
 * @brief UART receive in DMA mode example.
 * @param[in] uart_id           Identifies which UART to use
 ****************************************************************************************
 */
void uart_receive_dma_example(uart_t* uart);

// Loopback examples functions

/**
 ****************************************************************************************
 * @brief UART Interrupt loopback example
 * @param[in] uart_id           Identifies which UART to use
 ****************************************************************************************
 */
void uart_loopback_interrupt_example(uart_t *uart);


#endif // _UART2_PRINT_STRING_H
