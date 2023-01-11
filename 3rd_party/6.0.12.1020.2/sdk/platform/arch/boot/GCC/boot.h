/**
 ****************************************************************************************
 *
 * @file boot.h
 *
 * @brief This file contains the declarations of the boot related variables.
 *
 * Copyright (c) 2018-2019 Dialog Semiconductor. All rights reserved.
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

#ifndef _BOOT_H_
#define _BOOT_H_

extern const uint32_t __ER_IROM3_BASE__[];
#define CODE_AREA_BASE      ((uint32_t)__ER_IROM3_BASE__)

extern const uint32_t __code_area_end__[];
#define CODE_AREA_END       ((uint32_t)__code_area_end__)
#define CODE_AREA_LENGTH    (CODE_AREA_END - CODE_AREA_BASE)

extern const uint32_t __retention_mem_area_uninit_start__[];
#define RET_MEM_BASE        ((uint32_t)__retention_mem_area_uninit_start__)

extern const uint32_t __heap_mem_area_not_ret_start__[];
#define NON_RET_HEAP_BASE   ((uint32_t)__heap_mem_area_not_ret_start__)

extern const uint32_t __heap_mem_area_not_ret_end__[];
#define NON_RET_HEAP_END   ((uint32_t)__heap_mem_area_not_ret_end__)
#define NON_RET_HEAP_LENGTH (NON_RET_HEAP_END - NON_RET_HEAP_BASE)

#endif // _BOOT_H_
