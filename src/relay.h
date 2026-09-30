//
// Created by baotr on 9/24/2026.
//

#ifndef RP2040_FREERTOS_IRQ_RELAY_H
#define RP2040_FREERTOS_IRQ_RELAY_H

#include "pico/stdlib.h"

constexpr uint CO2_VALVE_PIN = 27;

void relayInit(void);
void openCO2Valve(void);
void closeCO2Valve(void);

#endif //RP2040_FREERTOS_IRQ_RELAY_H