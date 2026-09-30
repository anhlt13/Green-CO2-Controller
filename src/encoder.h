//
// Created by baotr on 9/25/2026.
//

#ifndef RP2040_FREERTOS_IRQ_ENCODER_H
#define RP2040_FREERTOS_IRQ_ENCODER_H

#include "pico/stdlib.h"

constexpr  uint ROT_A_PIN = 10;
constexpr  uint ROT_B_PIN = 11;
constexpr  uint ROT_SW_PIN = 12;

void encoderInit(void);

#endif //RP2040_FREERTOS_IRQ_ENCODER_H