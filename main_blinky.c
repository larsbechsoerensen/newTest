/*
 * FreeRTOS Kernel V10.3.1
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */


 /* Standard includes. */
#include <stdio.h>
#include <conio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "queue.h"

/* Priorities at which the tasks are created. */
#define taskA_PRIORITY		( tskIDLE_PRIORITY + 3 )
#define	taskB_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define	taskC_PRIORITY		( tskIDLE_PRIORITY + 1 )




/*-----------------------------------------------------------*/

TaskHandle_t* xHandle_A = NULL;
TaskHandle_t* xHandle_B = NULL;

QueueHandle_t xQueue1, xQueue2;

/*
 * The tasks as described in the comments at the top of this file.
 */
static void taskA(void* pvParameters);
static void taskB(void* pvParameters);
static void taskC(void* pvParameters);

static void idelWork(int t);

struct AMessage
{
    char ucMessageID;
    char ucData[10];
} xMessage;

void main_blinky(void)
{
    xMessage.ucMessageID = 0x41;
    memset(&(xMessage.ucData), 0x42, 10);
    printf("\nmemset: %s\n", xMessage.ucData);
    /* Create a queue capable of containing 10 unsigned long values. */
    xQueue1 = xQueueCreate(10, sizeof(int));

    xQueue2 = xQueueCreate(10, sizeof(struct AMessage));
 
    xTaskCreate(taskA,			/* The function that implements the task. */
        "Rx", 							/* The text name assigned to the task - for debug only as it is not used by the kernel. */
        configMINIMAL_STACK_SIZE, 		/* The size of the stack to allocate to the task. */
        NULL, 							/* The parameter passed to the task - not used in this simple case. */
        taskA_PRIORITY,/* The priority assigned to the task. */
        &xHandle_A);		/*Handle to task A*/

    xTaskCreate(taskB, "TX", configMINIMAL_STACK_SIZE, NULL, taskB_PRIORITY, &xHandle_B);/*Handle to task B*/

    xTaskCreate(taskC, "ZX", configMINIMAL_STACK_SIZE, NULL, taskC_PRIORITY, NULL);	/* The task handle is not required, so NULL is passed. */

    vTaskStartScheduler();

    for (;; );
}
/*-----------------------------------------------------------*/


static void taskA(void* pvParameters)
{
    /* Prevent the compiler warning about the unused parameter. */
    (void)pvParameters;
    int x = 42;

    

    for (;;)
    {
        printf("In task A\n\n");
        if (xQueueSendToBack(xQueue1, (void*)&x, (TickType_t)10) != pdPASS)
        {
            printf("Time out: Send int\n");
        }

        if (xQueueSendToBack(xQueue2, (void*)&xMessage, (TickType_t)0) != pdPASS) 
        {
            printf("Time out: Send struct\n");
        }


        vTaskDelay(2000);

    }
}

static void taskB(void* pvParameters)
{
    /* Prevent the compiler warning about the unused parameter. */
    (void)pvParameters;

    for (;; )
    {
        printf("\tIn task B\n");
        int i;
        int* pti = &i;
        if (xQueueReceive(xQueue1, &(i), (TickType_t)10) == pdPASS)
        {
            printf("\nReceived: %d\n", *pti);
        }
        else
        {
            printf("\nTime out: Received int\n");
        }

        struct AMessage xMessage, * ptxMessage;

        if (xQueueReceive(xQueue2, &(xMessage), (TickType_t)10) == pdPASS)
        {
            printf("\nReceived struct msgId: %c\n", xMessage.ucMessageID);
            char* data = xMessage.ucData;
            *(data + 10) = '\0';
            printf("\nReceived struct data: %s\n", data);
        }
        else {
            printf("\nTime out: Received struct\n");
        }
        vTaskDelay(1000);
        
    }
}



static void taskC(void* pvParameters)
{
    /* Prevent the compiler warning about the unused parameter. */
    (void)pvParameters;

    for (;; )
    {
        vTaskDelay(2000);
        printf("In task C\n");
        vTaskDelay(20000);

    }
}

static void idelWork(int t) {
    //do some idel work
    for (int i = 0; i < 20000; i++) {
        for (int j = 0; j < 2000; j++)
            for (int r = 0; r < t; r++)
                ;
    }
}

/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/


