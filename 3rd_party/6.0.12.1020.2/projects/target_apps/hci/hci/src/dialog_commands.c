/**
 ****************************************************************************************
 *
 * @file dialog_commands.c
 *
 * @brief dialog vendor specific command handlers project source code.
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
 
#include "hci_int.h"
#include "hci.h"
#include "co_utils.h"
#include "dialog_commands.h"

// HCI dialog command descriptors (OGF Vendor Specific)
const struct hci_cmd_desc_tag hci_cmd_desc_tab_dialog_vs[] =
{
    CMD(VS1_DIALOG     , DBG, 0, PK_GEN_GEN, "B"       , "BB"       ),
};

const uint8_t dialog_commands_num = ARRAY_LEN (hci_cmd_desc_tab_dialog_vs);

/**
 ****************************************************************************************
 * @brief Handles the reception of the vs1 dialog hci command.
 *
 * @param[in] msgid Id of the message received (probably unused).
 * @param[in] param Pointer to the parameters of the message.
 * @param[in] dest_id ID of the receiving task instance (probably unused).
 * @param[in] src_id ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int dialog_commands_vs1_handler(ke_msg_id_t const msgid, 
                                      struct hci_vs1_dialog_cmd const *param,
                                      ke_task_id_t const dest_id, 
                                      ke_task_id_t const src_id)
{
    // structure type for the complete command event
    struct hci_vs1_dialog_cmd_cmp_evt *event = KE_MSG_ALLOC(HCI_CMD_CMP_EVENT , src_id, HCI_VS1_DIALOG_CMD_OPCODE, hci_vs1_dialog_cmd_cmp_evt);
    
    // process the received data if required

    // adding return values 
    event->returnData = 0xD1;
    event->status = CO_ERROR_NO_ERROR;
    hci_send_2_host(event);

    return (KE_MSG_CONSUMED);
}


// The message handlers for dialog HCI command complete events
const struct ke_msg_handler dialog_commands_handler_tab[] =
{
        {HCI_VS1_DIALOG_CMD_OPCODE,        (ke_msg_func_t)dialog_commands_vs1_handler},
};

const uint8_t dialog_commands_handler_num = ARRAY_LEN (dialog_commands_handler_tab);

