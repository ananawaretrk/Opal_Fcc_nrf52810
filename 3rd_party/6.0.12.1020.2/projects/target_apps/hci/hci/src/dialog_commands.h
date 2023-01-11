/**
 ****************************************************************************************
 *
 * @file dialog_commands.h
 *
 * @brief Dialog_commands header file.
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

#ifndef DIALOG_COMMANDS_H_
#define DIALOG_COMMANDS_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <stdint.h>

/*
 * DEFINES
 ****************************************************************************************
 */

#define HCI_VS_FIRST_DIALOG_CMD_OPCODE    0xFE00

//Dialog HCI commands range from 0xFE00 to 0XFEFF
enum
{
    HCI_VS1_DIALOG_CMD_OPCODE = HCI_VS_FIRST_DIALOG_CMD_OPCODE,
    HCI_VS_LAST_DIALOG_CMD_OPCODE           // DO NOT MOVE. Must always be last and opcodes linear (00,01,02 ...)
};

//HCI Debug dialog vs1 command parameters - vendor specific
struct hci_vs1_dialog_cmd
{
    //VS specific data
    uint8_t    inputData;
};

//HCI Debug dialog vs1 complete event parameters - vendor specific
struct hci_vs1_dialog_cmd_cmp_evt
{
    //Status of the command reception
    uint8_t status;
    //type
    uint8_t returnData;
};

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */

// Number of Dialog handlers
extern const uint8_t dialog_commands_handler_num;
// Number of Dialog commands
extern const uint8_t dialog_commands_num;
// Dialog HCI commands
extern const struct hci_cmd_desc_tag hci_cmd_desc_tab_dialog_vs[];
// Dialog HCI commands handlers
extern const struct ke_msg_handler dialog_commands_handler_tab[];

#endif // DIALOG_COMMANDS_H_
