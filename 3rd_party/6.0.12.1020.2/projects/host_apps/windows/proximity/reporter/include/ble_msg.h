/**
 ****************************************************************************************
 *
 * @file ble_msg.h
 *
 * @brief Header file for reception of ble messages sent from DA14585 embedded application over UART interface.
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

#ifndef BLE_MSG_H_
#define BLE_MSG_H_

#include "rwble_config.h"

typedef struct {
    unsigned short bType;
    unsigned short bDstid;
    unsigned short bSrcid;
    unsigned short bLength;
} ble_hdr;


typedef struct {
    unsigned short bType;
    unsigned short bDstid;
    unsigned short bSrcid;
    unsigned short bLength;
    unsigned char  bData[1];
} ble_msg;

 /*
 ****************************************************************************************
 * @brief Send message to UART iface.
 *
 * @param[in] msg   pointer to message.
 *
 * @return void.
 ****************************************************************************************
*/
void BleSendMsg(void *msg);
/*
 ****************************************************************************************
 * @brief Allocate memory for ble message.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 * @param[in] param_len Parameters length.
 *
 * @return void.
 ****************************************************************************************
*/
void *BleMsgAlloc(unsigned short id, unsigned short dest_id,
                  unsigned short src_id, unsigned short param_len);
/*
 ****************************************************************************************
 * @brief Allocate memory for ble message.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 * @param[in] param_len Parameters length.
 * @param[in] length    Length for the data
 *
 * @return void.
 ****************************************************************************************
*/
void *BleMsgDynAlloc(unsigned short id, unsigned short dest_id,
                     unsigned short src_id, unsigned short param_len, unsigned short length);


/*
 ****************************************************************************************
 * @brief Free ble msg.
 *
 * @param[in] msg   pointer to message.
 *
 * @return void.
 ****************************************************************************************
*/
void BleFreeMsg(void *msg);

/*
 ****************************************************************************************
 * @brief Free ble msg.
 *
 * @param[in] msg   pointer to message.
 *
 * @return void.
 ****************************************************************************************
*/
void BleFreeMsg(void *msg);

/*
 ****************************************************************************************
 * @brief Handles ble by calling corresponding procedure.
 *
 * @param[in] blemsg    Pointer to received message.
 *
 * @return void.
 ****************************************************************************************
*/
void HandleBleMsg(ble_msg *blemsg);
/*
 ****************************************************************************************
 * @brief Receives ble message from UART iface.
 *
 * @return void.
 ****************************************************************************************
*/
void BleReceiveMsg(void);



#endif //BLE_MSG_H_
