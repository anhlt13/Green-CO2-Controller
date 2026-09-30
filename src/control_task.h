//
// Created by baotr on 9/24/2026.
//

#ifndef RP2040_FREERTOS_IRQ_CONTROL_TASK_H
#define RP2040_FREERTOS_IRQ_CONTROL_TASK_H

#include "FreeRTOS.h"
#include "task.h"
static void requestFanSpeed(uint16_t percent);
void controlTaskFunction(void *param);

#endif //RP2040_FREERTOS_IRQ_CONTROL_TASK_H