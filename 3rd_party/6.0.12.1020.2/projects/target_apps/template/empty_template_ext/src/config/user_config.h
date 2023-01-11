/**
 ****************************************************************************************
 *
 * @file user_config.h
 *
 * @brief User configuration file.
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

#ifndef _USER_CONFIG_H_
#define _USER_CONFIG_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "app_user_config.h"
#include "arch_api.h"
#include "app_default_handlers.h"

/*
 * DEFINES
 ****************************************************************************************
 */

#define USER_CONFIG                (0)

/*
 * LOCAL VARIABLES
 ****************************************************************************************
 */

/******************************************
 * Default sleep mode. Possible values are:
 *
 * - ARCH_SLEEP_OFF
 * - ARCH_EXT_SLEEP_ON
 * - ARCH_EXT_SLEEP_OTP_COPY_ON
 *
 ******************************************
 */
static const sleep_state_t app_default_sleep_mode = ARCH_SLEEP_OFF;

/// Advertising service data
/// Advertising AD type flags, shall not be set in advertising data
#define USER_ADVERTISE_DATA    ""

/// Advertising data length - maximum 28 bytes, 3 bytes are reserved to set
#define USER_ADVERTISE_DATA_LEN (sizeof(USER_ADVERTISE_DATA)-1)

/// Scan response data
#define USER_ADVERTISE_SCAN_RESPONSE_DATA ""

/// Scan response data length- maximum 31 bytes
#define USER_ADVERTISE_SCAN_RESPONSE_DATA_LEN (sizeof(USER_ADVERTISE_SCAN_RESPONSE_DATA)-1)

/// Device name to be placed in advertising or scan response data
#define USER_DEVICE_NAME    "DIALOG-TMPL"

/// Device name length
#define USER_DEVICE_NAME_LEN (sizeof(USER_DEVICE_NAME)-1)

#endif // _USER_CONFIG_H_
