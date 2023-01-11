/**
 ****************************************************************************************
 *
 * @file user_commands.c
 *
 * @brief user vendor specific command handlers project source code.
 *
 * Copyright (c) 2017-2019 Dialog Semiconductor. All rights reserved.
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

#include "hci.h"
#include "hci_int.h"
#include "ke_task.h"
#include "user_commands.h"
#include "user_periph_setup.h"  // peripheral configuration


// HCI user command descriptors (OGF Vendor Specific)
const struct hci_cmd_desc_tag hci_cmd_desc_tab_user_vs[] =
{
    CMD(VS1_USER     , DBG, 0, PK_GEN_GEN, "B"        , "BB"        ),
    CMD(VS2_USER     , DBG, 0, PK_GEN_GEN, "B"        , "3B"        ),
};

const uint8_t  user_commands_num = ARRAY_LEN (hci_cmd_desc_tab_user_vs);

/**
 ****************************************************************************************
 * @brief Handles the reception of the vs1 user hci command.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int user_commands_vs1_handler(ke_msg_id_t const msgid,
                                     struct hci_vs1_user_cmd const *param,
                                     ke_task_id_t const dest_id,
                                     ke_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_vs1_user_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT , src_id, HCI_VS1_USER_CMD_OPCODE, hci_vs1_user_cmd_cmp_evt);

    // process the received data if required

    // adding return values
    event->returnData = 0x90;
    event->status = CO_ERROR_NO_ERROR;
    hci_send_2_host(event);

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles the reception of the vs1 user hci command.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int user_commands_vs2_handler(ke_msg_id_t const msgid,
                                     struct hci_vs2_user_cmd const *param,
                                     ke_task_id_t const dest_id,
                                     ke_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_vs2_user_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT , src_id, HCI_VS2_USER_CMD_OPCODE, hci_vs2_user_cmd_cmp_evt);

    // process the received data if required

    // adding return values
    event->returnData = 0x91;
    event->mycode = 0xFF;
    event->status = CO_ERROR_NO_ERROR;
    hci_send_2_host(event);

    return (KE_MSG_CONSUMED);
}

// The message handlers for user HCI command complete events
const struct ke_msg_handler user_commands_handler_tab[] =
{
        {HCI_VS1_USER_CMD_OPCODE,        (ke_msg_func_t)user_commands_vs1_handler},
        {HCI_VS2_USER_CMD_OPCODE,        (ke_msg_func_t)user_commands_vs2_handler},
};

const uint8_t  user_commands_handler_num = ARRAY_LEN (user_commands_handler_tab);
