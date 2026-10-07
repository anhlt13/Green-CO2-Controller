//
// Created by baotr on 9/24/2026.
//

#include "control_task.h"
#include "system_state.h"
#include "relay.h"

#include "timers.h"
#include <cstdio>

static constexpr float CO2_SAFETY_LIMIT_PPM = 2000.0f;
static constexpr uint32_t VALVE_OPEN_MS = 1000;
static constexpr uint32_t VALVE_MIXING_MS = 30000;
static constexpr uint16_t FAN_SPEED_NORMAL = 0;
static constexpr uint16_t FAN_SPEED_EMERGENCY = 100;
static constexpr TickType_t CONTROL_LOOP_PERIOD = pdMS_TO_TICKS(1000);
static constexpr float CO2_MIN_VALID_PPM = 100.0f;


enum ValveState
{
    VALVE_IDLE,
    VALVE_OPEN,
    VALVE_MIXING
};

static ValveState valveState = VALVE_IDLE;
static TimerHandle_t valveTimer = nullptr;
static bool fanBooster = false;

static void valveTimerCallback (TimerHandle_t xTimer)
{
    if (valveState == VALVE_OPEN)
    {
        closeCO2Valve();
        valveState = VALVE_MIXING;
        xTimerChangePeriod(xTimer,pdMS_TO_TICKS(VALVE_MIXING_MS),0);
        xTimerStart(xTimer,0);
    }
    else if (valveState == VALVE_MIXING)
    {
        valveState = VALVE_IDLE;
    }
}

static void requestFanSpeed(uint16_t percent)
{
    xQueueSend(fanSpeedQueue, &percent, 0);
}

void controlTaskFunction(void* param)
{
    (void)param;
    valveTimer = xTimerCreate("ValveTimer", pdMS_TO_TICKS(VALVE_OPEN_MS),pdFALSE, nullptr,valveTimerCallback);

    while (true)
    {
        uint32_t notifiedValue = 0;
        BaseType_t gotNotification = xTaskNotifyWait(0, 0xFFFFFFFF, &notifiedValue, CONTROL_LOOP_PERIOD);

        float co2Now;
        float setpointNow;

        xSemaphoreTake(sharedStateMutex, portMAX_DELAY);

        if (gotNotification == pdTRUE)
        {
            systemState.co2Setpoint = (float)notifiedValue;
        }
        co2Now = systemState.measuredCo2;
        setpointNow = systemState.co2Setpoint;

        xSemaphoreGive(sharedStateMutex);

        if (co2Now > CO2_SAFETY_LIMIT_PPM)
        {
            if (!fanBooster)
            {
                requestFanSpeed(FAN_SPEED_EMERGENCY);
                fanBooster = true;
            }

            if (valveState == VALVE_OPEN)
            {
                closeCO2Valve();
                xTimerStop(valveTimer,0);
                valveState = VALVE_IDLE;
            }
        }
        else if (fanBooster &&  co2Now <= setpointNow)
        {
            requestFanSpeed(FAN_SPEED_NORMAL);
            fanBooster = false;
        }

        if (valveState == VALVE_IDLE && co2Now >= CO2_MIN_VALID_PPM && co2Now < setpointNow)
        {
            openCO2Valve();
            valveState = VALVE_OPEN;
            xTimerChangePeriod(valveTimer,pdMS_TO_TICKS(VALVE_OPEN_MS),0);
            xTimerStart(valveTimer,0);
        }

        xSemaphoreTake(sharedStateMutex,portMAX_DELAY);
        systemState.valveIsOpen = (valveState == VALVE_OPEN);
        xSemaphoreGive(sharedStateMutex);
    }
}

