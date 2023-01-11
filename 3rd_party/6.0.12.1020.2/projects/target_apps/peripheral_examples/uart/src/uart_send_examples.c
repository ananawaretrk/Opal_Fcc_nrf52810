/**
 ****************************************************************************************
 *
 * @file uart_send_examples.c
 *
 * @brief UART send examples.
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

#include "uart.h"
#include "uart_utils.h"
#include "uart_common.h"

static const char OUTPUT_STRING[] = "0123456789 0123456789 0123456789 0123456789 0123456789\r\n"
                                    "0123456789 0123456789 0123456789 0123456789 0123456789\r\n"
                                    "0123456789 0123456789 0123456789 0123456789 0123456789\r\n"
                                    "0123456789 0123456789 0123456789 0123456789 0123456789\r\n"
                                    "0123456789 0123456789 0123456789 0123456789 0123456789\r\n"
                                    ;

volatile bool uart_send_finished = false;
volatile uint16_t data_sent_cnt = 0;

static void uart_send_cb(uint16_t length)
{
    data_sent_cnt = length;
    uart_send_finished = true;
}

void uart_send_blocking_example(uart_t* uart)
{
    printf_string(uart, "\n\r\n\r****************************************\n\r");
    if (uart == UART1)
    {
        printf_string(uart, "* UART1 Send Blocking example *\n\r");
    }
    else
    {
        printf_string(uart, "* UART2 Send Blocking example *\n\r");
    }
    printf_string(uart, "****************************************\n\r\n\r");

    uart_send(uart, (uint8_t *)OUTPUT_STRING, sizeof(OUTPUT_STRING) - 1, UART_OP_BLOCKING);

    printf_string(uart, "****************************************\n\r\n\r");
}

void uart_send_interrupt_example(uart_t* uart)
{
    printf_string(uart, "\n\r\n\r****************************************\n\r");
    if (uart == UART1)
    {
        printf_string(uart, "* UART1 Send Interrupt example *\n\r");
    }
    else
    {
        printf_string(uart, "* UART2 Send Interrupt example *\n\r");
    }
    printf_string(uart, "****************************************\n\r\n\r");

    uart_send_finished = false;
    data_sent_cnt = 0;

    uart_register_tx_cb(uart, uart_send_cb);
    uart_send(uart, (uint8_t *)OUTPUT_STRING, sizeof(OUTPUT_STRING) - 1, UART_OP_INTR);
    while (!uart_send_finished);
    printf_string(uart, "\n\rData sent: 0x");
    print_hword(uart, data_sent_cnt);
    printf_string(uart, " Bytes\n\r\n\r");
    printf_string(uart, "****************************************\n\r\n\r");
}

#if defined (CFG_UART_DMA_SUPPORT)
void uart_send_dma_example(uart_t* uart)
{
    printf_string(uart, "\n\r\n\r****************************************\n\r");
    if (uart == UART1)
    {
        printf_string(uart, "* UART1 Send DMA example *\n\r");
    }
    else
    {
        printf_string(uart, "* UART2 Send DMA example *\n\r");
    }
    printf_string(uart, "****************************************\n\r\n\r");

    uart_send_finished = false;
    data_sent_cnt = 0;

    uart_register_tx_cb(uart, uart_send_cb);
    uart_send(uart, (uint8_t *)OUTPUT_STRING, sizeof(OUTPUT_STRING) - 1, UART_OP_DMA);
    while (!uart_send_finished);
    printf_string(uart, "\n\rData sent: 0x");
    print_hword(uart, data_sent_cnt);
    printf_string(uart, " Bytes\n\r\n\r");
    printf_string(uart, "****************************************\n\r\n\r");
}
#endif
