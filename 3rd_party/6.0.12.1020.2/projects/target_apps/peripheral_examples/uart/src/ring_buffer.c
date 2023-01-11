/**
 ****************************************************************************************
 *
 * @file ring_buffer.c
 *
 * @brief Ring buffer implementation.
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

#include <stdint.h>
#include <stdbool.h>

static const unsigned int BUFFER_SIZE = 128;

static uint8_t buffer[BUFFER_SIZE];
static unsigned int buffer_count = 0;
static unsigned int buffer_head = 0;
static unsigned int buffer_tail = 0;

bool buffer_is_empty(void)
{
    return buffer_count == 0;
}

bool buffer_is_full(void)
{
    return buffer_count == BUFFER_SIZE;
}

void buffer_put_byte(uint8_t byte)
{
    if(!buffer_is_full())
    {
        buffer[buffer_head] = byte;
        buffer_head = (buffer_head + 1) % BUFFER_SIZE;
        buffer_count += 1;
    }
}

int buffer_get_byte(uint8_t *byte)
{
    int status = -1; // failure
    
    if(!buffer_is_empty())
    {
        *byte = buffer[buffer_tail];
        buffer_tail = (buffer_tail + 1) % BUFFER_SIZE;
        buffer_count -= 1;
        
        status = 0; // success
    }
    return status;
}
