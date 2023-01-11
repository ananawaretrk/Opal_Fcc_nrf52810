/**
 ****************************************************************************************
 *
 * @file Xtal_TRIM.h
 *
 * @brief Automated XTAL trim calculation core API.
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

#ifndef XTAL_TRIM_H_
#define XTAL_TRIM_H_

#include "rwip.h"

// Status codes
#define XTAL_PLTFRM_NO_ERROR            (0)                         // XTAL trim internal proc successfull.
#define PULSE_OUT_OF_RANGE_ERROR        (-1)                        // Pulse found in the pulse pin assigned GPIO was out of acceptable range.
#define NO_PULSE_ERROR                  (-2)                        // No pulse found, or pulse > 740 ms (measure_pulse aborts).
#define WRITING_VAL_TO_OTP_ERROR        (-3)                        // Failed to write value in OTP.
#define INVALID_GPIO_ERROR              (-4)                        // Wrong GPIO configuration.
#define WRONG_XTAL_SOURCE_ERROR         (-5)                        // Incorrect pulse detected.

#define AUTO_XTAL_TEST_DBG_EN           (0)                         // Enable/Disable debug parameters.

int auto_trim(uint8_t XTAL_select, uint8_t port_number);
#if AUTO_XTAL_TEST_DBG_EN
void TRIM_test (int S1, int S2);                                    // testing
#endif

#endif /* XTAL_TRIM_H_ */
