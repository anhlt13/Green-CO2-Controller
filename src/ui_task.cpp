//
// Created by baotr on 9/26/2026.
//

#include "ui_task.h"
#include "system_state.h"
#include "encoder.h"
#include "ssd1306os.h"
#include "EEPROM.h"
#include "i2c_bus.h"
#include "pressure_sensor.h"

#include <cstdio>
#include <cstring>   \

enum UIScreen
{
    SCREEN_MAIN,
    SCREEN_MENU,
    SCREEN_SETPOINT,
    SCREEN_NETWORK
};

static const char *MENU_ITEMS[] =
{
    "View measured data",
    "Set CO2 target",
    "Network settings"
};

static constexpr int MENU_ITEM_COUNT = 3;
static char ssidBuffer[EEPROM_SSID_MAX_LEN]     = {0};
static char passwordBuffer[EEPROM_PASS_MAX_LEN] = {0};

static bool swPreviousReleased = true;


static bool swPressedEdge(void)
{
    bool pressedNow = (gpio_get(ROT_SW_PIN) == 0);
    if (pressedNow && swPreviousReleased)
    {
        swPreviousReleased = false;
        return true;
    }
    if (!pressedNow)
    {
        swPreviousReleased = true;
    }
    return false;
}

static void drawMainScreen(ssd1306os &oled, const SharedState_t &state)
{
    char line[32];
    oled.fill(0);

    snprintf(line, sizeof(line), "CO2: %.0f ppm", state.measuredCo2);
    oled.text(line, 0, 0);

    snprintf(line, sizeof(line), "RH:  %.0f %%", state.relativeHumidity);
    oled.text(line, 0, 8);

    snprintf(line, sizeof(line), "T:   %.1f C", state.temperature);
    oled.text(line, 0, 16);

    snprintf(line, sizeof(line), "Pres: %.2f Pa", state.pressure);
    oled.text(line, 0, 24);

    snprintf(line, sizeof(line), "Fan: %u %%", state.fanSpeedPercent);
    oled.text(line, 0, 32);

    snprintf(line, sizeof(line), "Valve: %s", state.valveIsOpen ? "OPEN" : "closed");
    oled.text(line, 0, 40);

    snprintf(line, sizeof(line), "Set: %.0f ppm", state.co2Setpoint);
    oled.text(line, 0, 48);

    oled.show();
}

static void drawMenuScreen(ssd1306os &oled, int selectedIndex)
{
    oled.fill(0);
    oled.text("------MENU------",0,0);
    for (int i = 0; i < MENU_ITEM_COUNT; i++)
    {
        char line[24];
        snprintf(line, sizeof(line), "%s %s", (i == selectedIndex) ? ">" : " ", MENU_ITEMS[i]);
        oled.text(line,0,12+i*10);
    }
    oled.show();
}

static void drawSetpointScreen(ssd1306os &oled, float pendingValue)
{
    char line[32];
    oled.fill(0);
    oled.text("Set CO2 target",0,0);
    snprintf(line, sizeof(line), "%.0f ppm", pendingValue);
    oled.text(line, 0, 20);
    oled.text("Turn: adjust", 0, 40);
    oled.text("Press: confirm", 0, 50);
    oled.show();
}

static void drawNetworkScreen(ssd1306os &oled, const char *ssid, const char *password)
{
    char line[48];
    oled.fill(0);
    oled.text("Network Info", 0, 0);

    snprintf(line, sizeof(line), "SSID: %s", (ssid[0] != 0) ? ssid : "(not set)");
    oled.text(line, 0, 20);

    snprintf(line, sizeof(line), "Pass: %s", (password[0] != 0) ? "****" : "(not set)");
    oled.text(line, 0, 35);

    oled.text("Press: back", 0, 50);
    oled.show();
}

void uiTaskFunction(void* param)
{
    (void)param;

    ssd1306os oled(i2c1Bus);
    encoderInit();

    UIScreen screen = SCREEN_MAIN;
    int menuSelectedIndex = 0;
    float pendingSetpoint = 0.0f;

    xSemaphoreTake(eepromMutex, portMAX_DELAY);
    if (!eeprom_load_string(NETWORK_SSID_ADDR, ssidBuffer, EEPROM_SSID_MAX_LEN)) {
        ssidBuffer[0] = 0;
    }
    if (!eeprom_load_string(NETWORK_PASS_ADDR, passwordBuffer, EEPROM_PASS_MAX_LEN)) {
        passwordBuffer[0] = 0;
    }
    xSemaphoreGive(eepromMutex);

    xSemaphoreTake(eepromMutex, portMAX_DELAY);
    float loadedSetPoint;
    bool haveSaved = eeprom_load_setpoint(&loadedSetPoint);
    xSemaphoreGive(eepromMutex);

    xSemaphoreTake(sharedStateMutex, portMAX_DELAY);
    if (haveSaved)
    {
        systemState.co2Setpoint = loadedSetPoint;
    }
    else
    {
        systemState.co2Setpoint = 800.0f;
    }
    xSemaphoreGive(sharedStateMutex);

    TickType_t lastDrawTime = xTaskGetTickCount();

    while (true)
    {
        EncoderEvent_t event;
        BaseType_t hasEvent = xQueueReceive(encoderQueue, &event, pdMS_TO_TICKS(50));
        bool swPressed = swPressedEdge();

        switch (screen)
        {
        case SCREEN_MAIN:
            if (swPressed)
            {
                screen = SCREEN_MENU;
                menuSelectedIndex=0;
            }
            break;
        case SCREEN_MENU:
            if (hasEvent == pdTRUE)
            {
                menuSelectedIndex += event.direction;
                if (menuSelectedIndex < 0 )
                {
                    menuSelectedIndex = MENU_ITEM_COUNT-1;
                }
                if (menuSelectedIndex >= MENU_ITEM_COUNT)
                {
                    menuSelectedIndex = 0;
                }

            }
            if (swPressed)
            {
                if (menuSelectedIndex == 0)
                {
                    screen = SCREEN_MAIN;
                }
                else if (menuSelectedIndex == 1)
                {
                    xSemaphoreTake(sharedStateMutex, portMAX_DELAY);
                    pendingSetpoint = systemState.co2Setpoint;
                    xSemaphoreGive(sharedStateMutex);
                    screen = SCREEN_SETPOINT;
                }
                else if (menuSelectedIndex == 2)
                {
                    screen = SCREEN_NETWORK;
                }
            }
            break;
        case SCREEN_SETPOINT:
            if (hasEvent == pdTRUE)
            {
                pendingSetpoint += (float)event.direction * 50.0f;
                if (pendingSetpoint < 0.0f)
                {
                    pendingSetpoint = 0.0f;
                }
                if (pendingSetpoint > 1500.0f)
                {
                    pendingSetpoint = 1500.0f;
                }
            }
            if (swPressed)
            {
                xSemaphoreTake(sharedStateMutex, portMAX_DELAY);
                systemState.co2Setpoint = pendingSetpoint;
                xSemaphoreGive(sharedStateMutex);

                xSemaphoreTake(eepromMutex,portMAX_DELAY);
                eeprom_save_setpoint(pendingSetpoint);
                xSemaphoreGive(eepromMutex);

                xTaskNotify(controlTaskHandle, (uint32_t)pendingSetpoint, eSetValueWithOverwrite);

                screen = SCREEN_MAIN;
            }
            break;

        case SCREEN_NETWORK:
            if (swPressed)
            {
                screen = SCREEN_MENU;
            }
            break;
        }

        float pressureValue;
        if (readPressurePa(i2c1Bus, &pressureValue)) {
            xSemaphoreTake(sharedStateMutex, portMAX_DELAY);
            systemState.pressure = pressureValue;
            xSemaphoreGive(sharedStateMutex);
        }

        TickType_t now = xTaskGetTickCount();
        if (hasEvent == pdTRUE || swPressed || (now-lastDrawTime)>pdMS_TO_TICKS(500))
        {
            lastDrawTime = now;
            SharedState_t snapshot;
            xSemaphoreTake(sharedStateMutex, portMAX_DELAY);
            snapshot = systemState;
            xSemaphoreGive(sharedStateMutex);
            xSemaphoreTake(eepromMutex, portMAX_DELAY);
            if (!eeprom_load_string(NETWORK_SSID_ADDR, ssidBuffer, EEPROM_SSID_MAX_LEN)) {
                ssidBuffer[0] = 0;
            }
            if (!eeprom_load_string(NETWORK_PASS_ADDR, passwordBuffer, EEPROM_PASS_MAX_LEN)) {
                passwordBuffer[0] = 0;
            }
            xSemaphoreGive(eepromMutex);

            xSemaphoreTake(i2c1Mutex, portMAX_DELAY);
            switch (screen)
            {
            case SCREEN_MAIN:
                drawMainScreen(oled, snapshot);
                break;
            case SCREEN_MENU:
                drawMenuScreen(oled,menuSelectedIndex);
                break;
            case SCREEN_SETPOINT:
                drawSetpointScreen(oled,pendingSetpoint);
                break;
            case SCREEN_NETWORK:
                drawNetworkScreen(oled, ssidBuffer, passwordBuffer);
                break;
            }
            xSemaphoreGive(i2c1Mutex);
        }
    }
}
