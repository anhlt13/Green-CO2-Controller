//
// Created by baotr on 10/6/2026.
//

#include "console_task.h"
#include "system_state.h"
#include "EEPROM.h"
#include "IPStack.h"

#include <cstdio>
#include <cstring>

#include "network_task.h"

static IPStack *ipStack = nullptr;

static void printHelp()
{
    printf("\n-- Console commands --\n");
    printf("set ssid <value>      - Save WiFi SSID to EEPROM\n");
    printf("set password <value>  - Save WiFi password to EEPROM\n");
    printf("connect               - Connect to Wifi with saved configuration\n");
    printf("disconnect            - Disconnect wifi\n");
    printf("show                  - display current configuration\n");
    printf("factory reset         - delete SSID/password, set setpoint to default\n");
    printf("help                  - command instruction\n");
}

static void handleCommand(char *line)
{
    // "set ssid <value>"
    if (strncmp(line, "set ssid ", 9) == 0) {
        const char *value = line + 9;
        xSemaphoreTake(eepromMutex, portMAX_DELAY);
        bool ok = eeprom_save_string(NETWORK_SSID_ADDR, value, EEPROM_SSID_MAX_LEN);
        xSemaphoreGive(eepromMutex);
        printf("------------------------------------------------------------");
        printf(ok ? "[CONSOLE] SSID saved.\n" : "[CONSOLE] Failed - value too long?\n");
        printf("------------------------------------------------------------");
    }
    // "set password <value>"
    else if (strncmp(line, "set password ", 13) == 0) {
        const char *value = line + 13;
        xSemaphoreTake(eepromMutex, portMAX_DELAY);
        bool ok = eeprom_save_string(NETWORK_PASS_ADDR, value, EEPROM_PASS_MAX_LEN);
        xSemaphoreGive(eepromMutex);
        printf("------------------------------------------------------------");
        printf(ok ? "[CONSOLE] Password saved.\n" : "[CONSOLE] Failed - value too long?\n");
        printf("------------------------------------------------------------");
    }
    else if (strcmp(line, "connect") == 0)
    {
        printf("------------------------------------------------------------");
        printf("[CONSOLE] Reconnecting with saved config...\n");
        printf("------------------------------------------------------------");
        xTaskNotify(networkTaskHandle,NETWORK_CONNECT,eSetValueWithOverwrite);
    }
    else if (strcmp(line, "disconnect") == 0)
    {
        xTaskNotify(networkTaskHandle,NETWORK_DISCONNECT,eSetValueWithOverwrite
    );

    }
    else if (strcmp(line, "show") == 0) {
        char ssid[EEPROM_SSID_MAX_LEN] = {0};
        char pass[EEPROM_PASS_MAX_LEN] = {0};
        float setpoint = 0.0f;

        xSemaphoreTake(eepromMutex, portMAX_DELAY);
        bool haveSsid = eeprom_load_string(NETWORK_SSID_ADDR, ssid, EEPROM_SSID_MAX_LEN);
        bool havePass = eeprom_load_string(NETWORK_PASS_ADDR, pass, EEPROM_PASS_MAX_LEN);
        bool haveSet  = eeprom_load_setpoint(&setpoint);
        xSemaphoreGive(eepromMutex);

        printf("SSID:     %s\n", haveSsid ? ssid : "(not set)");
        printf("Password: %s\n", havePass ? "****" : "(not set)");
        printf("Setpoint: %s\n", haveSet ? "" : "(not set)");
        if (haveSet) printf("  -> %.0f ppm\n", setpoint);
    }
    else if (strcmp(line, "factory reset") == 0) {
        xSemaphoreTake(eepromMutex, portMAX_DELAY);
        eeprom_save_string(NETWORK_SSID_ADDR, "", EEPROM_SSID_MAX_LEN);
        eeprom_save_string(NETWORK_PASS_ADDR, "", EEPROM_PASS_MAX_LEN);
        eeprom_save_setpoint(800.0f);
        xSemaphoreGive(eepromMutex);
        printf("------------------------------------------------------------");
        printf("[CONSOLE] Factory settings restored.\n");
        printf("------------------------------------------------------------");
    }
    else if (strcmp(line, "help") == 0 || strlen(line) == 0) {
        printHelp();
    }
    else {
        printf("------------------------------------------------------------");
        printf("[CONSOLE] Unknown command. Type 'help'.\n");
        printf("------------------------------------------------------------");
    }
}

void consoleTaskFunction(void *pvParameters)
{
    (void)pvParameters;
    printHelp();

    char lineBuf[96];
    size_t idx = 0;

    for (;;) {
        int c = getchar();

        if (c == '\r' || c == '\n') {
            putchar('\n');
            lineBuf[idx] = 0;
            handleCommand(lineBuf);
            idx = 0;
        } else if (c == 127 || c == 8) { // Backspace/Delete
            if (idx > 0) {
                idx--;
                printf("\b \b");
            }
        } else if (idx < sizeof(lineBuf) - 1) {
            lineBuf[idx++] = (char)c;
            //putchar(c);
        }
    }
}