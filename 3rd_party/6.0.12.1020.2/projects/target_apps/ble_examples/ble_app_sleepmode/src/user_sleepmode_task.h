/**
 ****************************************************************************************
 *
 * @file user_sleepmode_task.h
 *
 * @brief Sleep mode project header file.
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

#ifndef _USER_SLEEPMODE_TASK_H_
#define _USER_SLEEPMODE_TASK_H_

/**
 ****************************************************************************************
 * @addtogroup APP
 * @ingroup RICOW
 *
 * @brief
 *
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
 
#include "custs1_task.h"
#include "ke_msg.h"
#include <stdint.h>

/*
 * DEFINES
 ****************************************************************************************
 */

enum
{
    CUSTS1_BTN_STATE_RELEASED = 0,
    CUSTS1_BTN_STATE_PRESSED,
};

enum
{
    CUSTS1_CP_CMD_PWM_DISABLE = 0,
    CUSTS1_CP_CMD_PWM_ENABLE,
    CUSTS1_CP_CMD_ADC_VAL_2_DISABLE,
    CUSTS1_CP_CMD_ADC_VAL_2_ENABLE,
};

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

struct app_proj_env_tag
{
    uint8_t custs1_pwm_enabled;
    uint8_t custs1_adcval2_enabled;
    uint8_t custs1_btn_state;
};

/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Control point write indication handler.
 * @param[in] msgid    Id of the message received.
 * @param[in] param    Pointer to the parameters of the message.
 * @param[in] dest_id  ID of the receiving task instance.
 * @param[in] src_id   ID of the sending task instance.
 * @return void
 ****************************************************************************************
 */
void user_svc1_ctrl_wr_ind_handler(ke_msg_id_t const msgid,
                                   struct custs1_val_write_ind const *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id);

/**
 ****************************************************************************************
 * @brief Enable peripherals used by application.
 * @return void
 ****************************************************************************************
 */
void user_app_enable_periphs(void);


/**
 ****************************************************************************************
 * @brief Disable peripherals used by application.
 * @return void
 ****************************************************************************************
 */
void user_app_disable_periphs(void);

/// @} APP

#endif // _USER_SLEEPMODE_TASK_H_

