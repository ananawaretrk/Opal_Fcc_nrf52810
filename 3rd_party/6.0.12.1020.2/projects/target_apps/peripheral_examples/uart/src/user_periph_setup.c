/**
 ****************************************************************************************
 *
 * @file user_periph_setup.c
 *
 * @brief Peripherals initialization functions
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

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "gpio.h"
#include "uart.h"
#include "user_periph_setup.h"


static void set_pad_functions(void)
{
/*
    i.e. to set P0_1 as Generic purpose Output:
    GPIO_ConfigurePin(GPIO_PORT_0, GPIO_PIN_1, OUTPUT, PID_GPIO, false);
*/

#if defined (__DA14586__)
    // Disallow spontaneous DA14586 SPI Flash wake-up
    GPIO_ConfigurePin(GPIO_PORT_2, GPIO_PIN_3, OUTPUT, PID_GPIO, true);
#endif
}

// Configuration struct for UART
static const uart_cfg_t uart_cfg = {
    // Set Baud Rate
    .baud_rate = UART_BAUDRATE_115200,
    // Set data bits
    .data_bits = UART_DATABITS_8,
    // Set parity
    .parity = UART_PARITY_NONE,
    // Set stop bits
    .stop_bits = UART_STOPBITS_1,
    // Set flow control
    .auto_flow_control = UART_AFCE_DIS,
    // Set FIFO enable
    .use_fifo = UART_FIFO_EN,
    // Set Tx FIFO trigger level
    .tx_fifo_tr_lvl = UART_TX_FIFO_LEVEL_0,
    // Set Rx FIFO trigger level
    .rx_fifo_tr_lvl = UART_RX_FIFO_LEVEL_0,
    // Set interrupt priority
    .intr_priority = 2,
#if defined (CFG_UART_DMA_SUPPORT)
    // Set UART DMA Channel Pair Configuration
    .uart_dma_channel = UART_DMA_CHANNEL_01,
    // Set UART DMA Priority
    .uart_dma_priority = DMA_PRIO_0,
#endif
};

void uart_periph_init(uart_t *uart)
{
    // Initialize UART
    uart_initialize(uart, &uart_cfg);

    if (uart == UART1)
    {
        GPIO_ConfigurePin(UART1_TX_PORT, UART1_TX_PIN, OUTPUT, PID_UART1_TX, false);
        GPIO_ConfigurePin(UART1_RX_PORT, UART1_RX_PIN, INPUT, PID_UART1_RX, false);
    }
    else
    {
        GPIO_ConfigurePin(UART2_TX_PORT, UART2_TX_PIN, OUTPUT, PID_UART2_TX, false);
        GPIO_ConfigurePin(UART2_RX_PORT, UART2_RX_PIN, INPUT, PID_UART2_RX, false);
    }

    // Enable the pads
    GPIO_set_pad_latch_en(true);
}

void periph_init(void)
{
#if !defined (__DA14531__)
    // Power up peripherals' power domain
    SetBits16(PMU_CTRL_REG, PERIPH_SLEEP, 0);
    while (!(GetWord16(SYS_STAT_REG) & PER_IS_UP));
    SetBits16(CLK_16M_REG, XTAL16_BIAS_SH_ENABLE, 1);
#endif

    set_pad_functions();
}
