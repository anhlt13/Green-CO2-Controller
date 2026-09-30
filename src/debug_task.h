//
// Created by baotr on 9/29/2026.
//

#ifndef RP2040_FREERTOS_IRQ_DEBUG_TASK_H
#define RP2040_FREERTOS_IRQ_DEBUG_TASK_H
#include <cstdint>
#include "queue.h"

void debugLog(const char *format, uint32_t arg1 = 0,uint32_t arg2 = 0, uint32_t arg3 = 0);

void debugTaskFunction(void *param);

#endif //RP2040_FREERTOS_IRQ_DEBUG_TASK_H