//
// Created by baotr on 9/29/2026.
//

#include "debug_task.h"
#include "task.h"
#include "queue.h"
#include <cstdio>

#include "portmacro.h"
#include "projdefs.h"

struct DebugMessage_t {
    const char *format;
    uint32_t    arg1;
    uint32_t    arg2;
    uint32_t    arg3;
    TickType_t  timestamp;
};

static QueueHandle_t debugQueue = nullptr;

void debugInit(void)
{
    debugQueue = xQueueCreate(20, sizeof(DebugMessage_t));
    vQueueAddToRegistry(debugQueue, "DebugQueue");
}

void debugLog(const char *format, uint32_t arg1, uint32_t arg2, uint32_t arg3)
{
    if (debugQueue == nullptr) {
        return;
    }
    DebugMessage_t msg = { format, arg1, arg2, arg3, xTaskGetTickCount() };
    xQueueSend(debugQueue, &msg, 0);
}

void debugTaskFunction(void *pvParameters)
{
    (void)pvParameters;

    DebugMessage_t msg;
    for (;;) {
        if (xQueueReceive(debugQueue, &msg, portMAX_DELAY) == pdTRUE) {
            printf("[%lu] ", (unsigned long)msg.timestamp);
            printf(msg.format, msg.arg1, msg.arg2, msg.arg3);
            printf("\n");
        }
    }
}