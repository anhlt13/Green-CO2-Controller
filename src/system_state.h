//
// Created by baotr on 9/24/2026.
//

#ifndef RP2040_FREERTOS_IRQ_SYSTEM_STATE_H
#define RP2040_FREERTOS_IRQ_SYSTEM_STATE_H

#include <stdint.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

//struct
typedef struct
{
    float measuredCo2;
    float relativeHumidity;
    float temperature;
    float pressure;
    uint16_t fanSpeedPercent;
    float co2Setpoint;
    bool valveIsOpen;
} SharedState_t;

extern SharedState_t systemState;

typedef struct
{
    int8_t direction;
} EncoderEvent_t;

//queues
extern QueueHandle_t encoderQueue;
extern QueueHandle_t fanSpeedQueue;
extern QueueHandle_t cloudSetPointQueue;

//task notification
extern TaskHandle_t controlTaskHandle;

//mutex
extern SemaphoreHandle_t eepromMutex;
extern SemaphoreHandle_t sharedStateMutex;
extern SemaphoreHandle_t i2c1Mutex;

void systemStateInit(void);
#endif //RP2040_FREERTOS_IRQ_SYSTEM_STATE_H