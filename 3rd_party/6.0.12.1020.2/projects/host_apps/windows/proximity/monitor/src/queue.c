/**
 ****************************************************************************************
 *
 * @file queue.c
 *
 * @brief Software for queues and threads.
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

#include "queue.h"
#include "console.h"
#include "uart.h"

// Used to stop the tasks.
BOOL StopConsoleTask, StopRxTask;
HANDLE ConsoleQueueSem;    // mutex semaphore to protect console event queue
HANDLE UARTRxQueueSem;     // mutex semaphore to protect uart rx queue
HANDLE Rx232Id, ConsoleTaskId;  // Thread handles
QueueRecord ConsoleQueue, UARTRxQueue; //Queues UARTRx -> Main thread /  Console -> Main thread


void InitTasks(void)
{
    StopConsoleTask = FALSE;
    StopRxTask = FALSE;
    Rx232Id   = (HANDLE) _beginthread(UARTProc, 10000, NULL);
    ConsoleTaskId   = (HANDLE) _beginthread(ConsoleProc, 10000, NULL);
    // Set thread priorities
    SetThreadPriority(Rx232Id, THREAD_PRIORITY_TIME_CRITICAL);
    SetThreadPriority(ConsoleTaskId, THREAD_PRIORITY_TIME_CRITICAL);
    ConsoleQueueSem = CreateMutex( NULL, FALSE, NULL );
    UARTRxQueueSem = CreateMutex( NULL, FALSE, NULL );
}


void EnQueue(QueueRecord *rec,void *vdata)
{
    struct QueueStorage *tmp;

    tmp = (struct QueueStorage *) malloc(sizeof(struct QueueStorage));
    tmp->Next = NULL;
    tmp->Data = vdata;
    if(rec->First == NULL)
    {
        rec->First=tmp;
        rec->Last=tmp;
    }
    else
    {
        rec->Last->Next=tmp;
        rec->Last=tmp;
    }
}


void *DeQueue(QueueRecord *rec)
{
    void *tmp;
    struct QueueStorage *tmpqe;

    if(rec->First == NULL)
    {
        return NULL;
    }
    else
    {
        tmp = rec->First->Data;
        tmpqe = rec->First;
        rec->First = tmpqe->Next;
        free(tmpqe);


        if(rec->First == NULL)
        {
            rec->First = rec->Last = NULL;
        }

    }
    return tmp;
}
