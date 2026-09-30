//
// Created by baotr on 9/24/2026.
//
#include "system_state.h"

SharedState_t systemState;

QueueHandle_t encoderQueue;
QueueHandle_t fanSpeedQueue;
QueueHandle_t cloudSetPointQueue;

TaskHandle_t controlTaskHandle;

SemaphoreHandle_t eepromMutex;
SemaphoreHandle_t sharedStateMutex;
SemaphoreHandle_t i2c1Mutex;

void systemStateInit(void)
{
    sharedStateMutex = xSemaphoreCreateMutex();
    eepromMutex = xSemaphoreCreateMutex();
    i2c1Mutex = xSemaphoreCreateMutex();

    encoderQueue = xQueueCreate(10,sizeof(EncoderEvent_t));
    fanSpeedQueue = xQueueCreate(10,sizeof(uint16_t));
    cloudSetPointQueue = xQueueCreate(10,sizeof(float));

    vQueueAddToRegistry(encoderQueue,"EncoderQueue");
    vQueueAddToRegistry(fanSpeedQueue,"FanSpeedQueue");
    vQueueAddToRegistry(cloudSetPointQueue,"CloudSetPointQueue");
}