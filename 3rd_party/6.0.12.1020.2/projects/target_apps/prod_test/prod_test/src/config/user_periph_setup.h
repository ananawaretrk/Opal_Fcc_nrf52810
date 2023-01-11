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
#include "dialog_prod.h"

/// Divider for 1000000 bits/s
#define UART_BAUDRATE_1M            1
#define UART_FRAC_BAUDRATE_1M       0

/// Divider for 921600 bits/s
#define UART_BAUDRATE_921K6         1
#define UART_FRAC_BAUDRATE_921K6    1

/// Divider for 500000 bits/s
#define UART_BAUDRATE_500K          2
#define UART_FRAC_BAUDRATE_500K     0

/// Divider for 460800 bits/s
#define UART_BAUDRATE_460K8         2
#define UART_FRAC_BAUDRATE_460K8    3

/// Divider for 230400 bits/s
#define UART_BAUDRATE_230K4         4
#define UART_FRAC_BAUDRATE_230K4    5

/// Divider for 115200 bits/s
#define UART_BAUDRATE_115K2         8
#define UART_FRAC_BAUDRATE_115K2   11

/// Divider for 57600 bits/s
#define UART_BAUDRATE_57K6          17
#define UART_FRAC_BAUDRATE_57K6     6

/// Divider for 38400 bits/s
#define UART_BAUDRATE_38K4          26
#define UART_FRAC_BAUDRATE_38K4     1

/// Divider for 28800 bits/s
#define UART_BAUDRATE_28K8          34
#define UART_FRAC_BAUDRATE_28K8     12

/// Divider for 19200 bits/s
#define UART_BAUDRATE_19K2          52
#define UART_FRAC_BAUDRATE_19K2     1

/// Divider for 14400 bits/s
#define UART_BAUDRATE_14K4          69
#define UART_FRAC_BAUDRATE_14K4     7

/// Divider for 9600 bits/s
#define UART_BAUDRATE_9K6           104
#define UART_FRAC_BAUDRATE_9K6      3

/// Divider for 2400 bits/s
#define UART_BAUDRATE_2K4           416
#define UART_FRAC_BAUDRATE_2K4      11

/// Character format
enum
{
    /// char format 5
    UART_CHARFORMAT_5 = 0,
    /// char format 6
    UART_CHARFORMAT_6 = 1,
    /// char format 7
    UART_CHARFORMAT_7 = 2,
    /// char format 8
    UART_CHARFORMAT_8 = 3
};

/*
 * VARIABLE DEFINITIONS
 ****************************************************************************************
 */
extern void uart_init(uint16_t baudr, uint8_t dlf_value, uint8_t mode);
extern uint8_t port_sel;

typedef struct __uart_sel_pins_t
{
    uint8_t uart_port_tx;
    uint8_t uart_tx_pin;
    uint8_t uart_port_rx;
    uint8_t uart_rx_pin;
}_uart_sel_pins_t;

extern _uart_sel_pins_t     uart_sel_pins;
extern  uint8_t             baud_rate_sel;
extern  uint8_t             baud_rate_frac_sel;

/*
 * DEFINES
 ****************************************************************************
 */

/****************************************************************************/
/* UART pin configuration                                                   */
/****************************************************************************/

/****************************************************************************/
/* CONFIG_UART_GPIOS                                                        */
/*    -defined      Uart Port/Pins are defined by external tool             */
/*    -undefined    Uart Port/Pins are defined in the current project       */
/****************************************************************************/
#define CONFIG_UART_GPIOS

/****************************************************************************/
/* UART pin configuration                                                   */
/* Supported Port/Pin Combinations:                                         */
/* Tx: P00, Rx: P01                                                         */
/* Tx: P02, Rx: P03                                                         */
/* Tx/Rx: P03 (1-Wire UART)                                                 */
/* Tx: P04, Rx: P05                                                         */
/* Tx/Rx: P05 (1-Wire UART)                                                 */
/* Tx: P06, Rx: P07                                                         */
/****************************************************************************/

#if defined(__DA14531__)
    #define UART1_TX_GPIO_PORT  GPIO_PORT_0
    #define UART1_TX_GPIO_PIN   GPIO_PIN_0
    #define UART1_RX_GPIO_PORT  GPIO_PORT_0
    #define UART1_RX_GPIO_PIN   GPIO_PIN_1
#else
    #define UART1_TX_GPIO_PORT  GPIO_PORT_0
    #define UART1_TX_GPIO_PIN   GPIO_PIN_4
    #define UART1_RX_GPIO_PORT  GPIO_PORT_0
    #define UART1_RX_GPIO_PIN   GPIO_PIN_5
#endif

enum
{
    P0_0_AND_P0_1_INITIALIZED_FROM_EXT_TOOL = 0x00,
    P0_2_AND_P0_3_INITIALIZED_FROM_EXT_TOOL = 0x02,
    P0_3_SINGLE_W_INITIALIZED_FROM_EXT_TOOL = 0x03,
    P0_4_AND_P0_5_INITIALIZED_FROM_EXT_TOOL = 0x04,
    P0_5_SINGLE_W_INITIALIZED_FROM_EXT_TOOL = 0x05,
    P0_6_AND_P0_7_INITIALIZED_FROM_EXT_TOOL = 0x06,
};

/****************************************************************************************/
/* UART2 configuration to use with arch_console print messages                          */
/****************************************************************************************/
// Define UART2 Tx Pad
#if defined (__DA14531__)
    #define UART2_TX_PORT           GPIO_PORT_0
    #define UART2_TX_PIN            GPIO_PIN_4
#else
    #define UART2_TX_PORT           GPIO_PORT_0
    #define UART2_TX_PIN            GPIO_PIN_4
#endif

// Define UART2 Settings
#define UART2_BAUDRATE              UART_BAUDRATE_115200
#define UART2_DATABITS              UART_DATABITS_8
#define UART2_PARITY                UART_PARITY_NONE
#define UART2_STOPBITS              UART_STOPBITS_1
#define UART2_AFCE                  UART_AFCE_DIS
#define UART2_FIFO                  UART_FIFO_EN
#define UART2_TX_FIFO_LEVEL         UART_TX_FIFO_LEVEL_0
#define UART2_RX_FIFO_LEVEL         UART_RX_FIFO_LEVEL_0

/****************************************************************************************/
/* SPI FLASH configuration                                                              */
/****************************************************************************************/
#ifndef __DA14586__
#define SPI_FLASH_DEV_SIZE  (256 * 1024)    // SPI Flash memory size in bytes
#else // DA14586
    #define SPI_EN_PORT    GPIO_PORT_2
    #define SPI_EN_PIN     GPIO_PIN_3

    #define SPI_CLK_PORT   GPIO_PORT_2
    #define SPI_CLK_PIN    GPIO_PIN_0

    #define SPI_DO_PORT    GPIO_PORT_2
    #define SPI_DO_PIN     GPIO_PIN_9

    #define SPI_DI_PORT    GPIO_PORT_2
    #define SPI_DI_PIN     GPIO_PIN_4
#endif

/***************************************************************************************/
/* Production debug output configuration                                               */
/***************************************************************************************/
#if PRODUCTION_DEBUG_OUTPUT
#if defined (__DA14531__)
    #define PRODUCTION_DEBUG_PORT   GPIO_PORT_0
    #define PRODUCTION_DEBUG_PIN    GPIO_PIN_11
#else
    #define PRODUCTION_DEBUG_PORT   GPIO_PORT_2
    #define PRODUCTION_DEBUG_PIN    GPIO_PIN_5
#endif
#endif

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Enable pad and peripheral clocks assuming that peripheral power domain
 *        is down. The UART and SPI clocks are set.
 * @return void
 ****************************************************************************************
 */
void periph_init(void);

/**
 ****************************************************************************************
 * @brief Map port pins. The UART and SPI port pins and GPIO ports are mapped.
 * @return void
 ****************************************************************************************
 */
void set_pad_functions(void);

/**
 ****************************************************************************************
 * @brief Each application reserves its own GPIOs here.
 * @return void
 ****************************************************************************************
 */
void GPIO_reservations(void);

/**
 ****************************************************************************************
 * @brief Map port pins. The UART pins are mapped.
 * @return void
 ****************************************************************************************
 */
void set_pad_uart(void);

/**
 ****************************************************************************************
 * @brief Unset UART port pins. UART pins are set to input and returned to normal GPIOs
 * @return void
 ****************************************************************************************
 */
void unset_pad_uart(void);

#if defined (__DA14531__)
/**
 ****************************************************************************************
 * @brief Enables the hw_reset functionality when P0_0 GPIO is not used for any other operation.
 * @return void
 ****************************************************************************************
 */
void set_hw_reset(void);
#endif

/**
 ****************************************************************************************
 * @brief Update port pins. The UART pins are stored to retention memory.
 * @param[in] tx_port Port for UART TX
 * @param[in] tx_pin Pin for UART TX
 * @param[in] rx_port Port for UART RX
 * @param[in] rx_pin Pin for UART RX
 * @return void
 ****************************************************************************************
 */
void update_uart_pads(GPIO_PORT tx_port, GPIO_PIN tx_pin, GPIO_PORT rx_port, GPIO_PIN rx_pin);

#if !defined (__DA14531__)
/**
 ****************************************************************************************
 * @brief Initialize TXEN and RXEN.
 * @return void
 ****************************************************************************************
 */
void init_TXEN_RXEN_irqs(void);
#endif

#endif // _USER_PERIPH_SETUP_H_
