/**
 ****************************************************************************************
 *
 * @file rdtest_api.h
 *
 * @brief RD test API header file.
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

#ifndef _RDTEST_API_H_
#define _RDTEST_API_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
 
#include <stdint.h>
#include "rdtest_lowlevel.h"
#include "rdtest_support.h"

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Initialize ports
 *
 * This function initialize ports and put CPLD in known (safe) state. Version can later
 * be used to control the lowlevel communication method, not yet implemented
 *
 * @param[in] version
 *
 * @return void
 *****************************************************************************************
 */
void rdtest_initialize(uint8_t version);

/**
 ****************************************************************************************
 * @brief Controls the Vpp
 *
 * This function control the Vpp OTP programming voltage
 *
 * @param[in] state
 *
 * @return void
 *****************************************************************************************
 */
void rdtest_vppcontrol(uint8_t state);

/**
 ****************************************************************************************
 * @brief Controls the pulsewidth
 *
 * This function controls the pulsewidth selection
 *
 * @param[in] length
 *
 * @return void
 *****************************************************************************************
 */
void rdtest_select_pulsewidth(uint8_t length);

/**
 ****************************************************************************************
 * @brief Connect or disconnects
 *
 * This function connect or disconnects an FTDI-uart to DUT_connector
 *
 * @param[in] connectmap16
 *
 * @return void
 *****************************************************************************************
 */
void rdtest_uart_connect(uint16_t connectmap16);

/**
 ****************************************************************************************
 * @brief Internal loopback
 *
 * This function control CPLD internal loopback, only port only!
 *
 * @param[in] port
 *
 * @return void
 *****************************************************************************************
 */
void rdtest_make_loopback(uint8_t port);

/**
 ****************************************************************************************
 * @brief Connect Vbat
 *
 * This function connect Vbat to DUT_connector
 *
 * @param[in] connectmap16
 *
 * @return void
 *****************************************************************************************
 */
void rdtest_vbatcontrol(uint16_t connectmap16);

/**
 ****************************************************************************************
 * @brief Reset pulse
 *
 * This function generate a reset pulse to all devices
 *
 * @param[in] delayms
 *
 * @return void
 *****************************************************************************************
 */
void rdtest_resetpulse(uint16_t delayms);

/**
 ****************************************************************************************
 * @brief Xtal-measurement to uart pin
 *
 * This function route xtal-measurement pulse to uart pin
 *
 * @param[in] connectmap16
 *
 * @return void
 *****************************************************************************************
 */
void rdtest_pulse_to_uart(uint16_t connectmap16);

/**
 ****************************************************************************************
 * @brief Xtal-measurement
 *
 * This function generate 1 xtal-measurement pulse
 *
 * @return void
 *****************************************************************************************
 */
void rdtest_generate_pulse(void);

#endif // _RDTEST_API_H_
