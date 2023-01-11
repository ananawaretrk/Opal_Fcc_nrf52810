/**
 ****************************************************************************************
 *
 * @file console.h
 *
 * @brief Header file for basic console user interface of the host application.
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

#ifndef CONSOLE_H_
#define CONSOLE_H_

#include "rwble_config.h"
#include "stdbool.h"

extern bool console_main_pass_state;
extern bool conn_num_flag;

typedef struct {
    unsigned char type;
    unsigned char val;
    unsigned char idx;
} console_msg;

enum
{
    CONSOLE_DEV_DISC_CMD,
    CONSOLE_CONNECT_CMD,
    CONSOLE_DISCONNECT_CMD,
    CONSOLE_RD_LLV_CMD,
    CONSOLE_RD_TXP_CMD,
    CONSOLE_WR_LLV_CMD,
    CONSOLE_WR_IAS_CMD,
    CONSOLE_EXIT_CMD,
};

/*
 ****************************************************************************************
 * @brief Sends Discover devices message to application 's main thread.
 *
 * @return void.
 ****************************************************************************************
 */
void ConsoleSendScan(void);

/*
 ****************************************************************************************
 * @brief Sends Connect to device message to application 's main thread.
 *
 *  @param[in] indx Device's index in discovered devices list.
 *
 * @return void.
 ****************************************************************************************
 */
void ConsoleSendConnnect(int indx);

/*
 ****************************************************************************************
 * @brief Sends Disconnect request message to application 's main thread.
 *
 *  @param[in] id Selected device id.
 *
 * @return void.
 ****************************************************************************************
 */
void ConsoleSendDisconnnect(int id);

/*
 ****************************************************************************************
 * @brief Sends Read request message to application 's main thread.
 *
 *  @param[in] type  Attribute type to read.
 *  @param[in] idx  Selected device id.
 *
 * @return void.
 ****************************************************************************************
 */
void ConsoleRead(unsigned char type,unsigned char idx);

/*
 ****************************************************************************************
 * @brief Sends write request message to application 's main thread.
 *
 *  @param[in] type  Attribute type to write.
 *  @param[in] val   Attribute value.
 *  @param[in] idx   Selected device id.
 *
 * @return void.
 ****************************************************************************************
 */
void ConsoleWrite(unsigned char type, unsigned char val, unsigned char idx);

/**
 ****************************************************************************************
 * @brief Sends a message to application 's main thread to exit.
 *
 * @return void.
 ****************************************************************************************
 */
void ConsoleSendExit(void);

/**
 ****************************************************************************************
 * @brief Handles keypress events and sends corresponding message to application's main thread.
 *
 * @return void.
 ****************************************************************************************
 */
void HandleKeyEvent(int Key);

/**
 ****************************************************************************************
 * @brief Demo application Console Menu header
 *
 * @return void.
 ****************************************************************************************
 */
void ConsoleTitle(void);

/**
 ****************************************************************************************
 * @brief Prints Console menu in Scan state.
 *
 * @return void.
 ****************************************************************************************
 */
void ConsoleScan(void);

/**
 ****************************************************************************************
 * @brief Prints Console menu in Idle state.
 *
 * @return void.
 ****************************************************************************************
 */
void ConsoleIdle(void);

/**
 ****************************************************************************************
 * @brief Prints List of discovered devices.
 *
 * @return void.
 ****************************************************************************************
 */
void ConsolePrintScanList(void);

/**
 ****************************************************************************************
 * @brief Prints Console Menu in connected state.
 *
 * @param[in] full   If true, prints peers attributes values.
 *
 * @return void.
 ****************************************************************************************
 */
void ConsoleConnected(int full);

/**
 ****************************************************************************************
 * @brief Console Thread main loop.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
VOID ConsoleProc(PVOID unused);

/**
 ****************************************************************************************
 * @brief Prints connected devices list
 * 
 * @return void.
 ****************************************************************************************
 */
void ConsoleQues(void);


/**
 ****************************************************************************************
 * @brief prints message "maximum connections reached"
 *
 * @return void.
 ****************************************************************************************
 */
void print_err_con_num(void);

/**
 ****************************************************************************************
 * @brief Prints Tabs.
 *
 *  @param[in] size  Size of string.
 *  @param[in] mode  Mode of print.
 *
 * @return void.
 ****************************************************************************************
 */
void printtabs(int size, int mode);

#endif //CONSOLE_H_
