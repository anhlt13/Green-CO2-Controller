//
// Created by baotr on 9/28/2026.
//

#ifndef RP2040_FREERTOS_IRQ_PRESSURE_SENSOR_H
#define RP2040_FREERTOS_IRQ_PRESSURE_SENSOR_H
#include <cstdint>
#include "hardware/i2c.h"

constexpr  uint8_t SDP610_I2C_ADDR = 0x40;
bool readPressurePa(float *outPa);
#endif //RP2040_FREERTOS_IRQ_PRESSURE_SENSOR_H
