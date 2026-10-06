//
// Created by vuhav on 04/03/2026.
//

#ifndef RP2040_FREERTOS_IRQ_EEPROM_H
#define RP2040_FREERTOS_IRQ_EEPROM_H

#include <cstdint>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"

constexpr uint8_t  EEPROM_I2C_ADDR   = 0x50;
constexpr uint     I2C_SCL_PIN       = 17;
constexpr uint     I2C_SDA_PIN       = 16;

constexpr uint16_t LOG_START_ADDR    = 0;
constexpr uint     LOG_ENTRY_SIZE    = 64;
constexpr uint     LOG_MAX_ENTRIES   = 32;
constexpr uint     LOG_MAX_STR_LEN   = 61;   // 64 - 1 null - 2 crc
constexpr uint16_t CONFIG_ADDR = 2048;

constexpr size_t   EEPROM_SSID_MAX_LEN = 33;
constexpr size_t   EEPROM_PASS_MAX_LEN = 62;
constexpr uint16_t NETWORK_SSID_ADDR   = CONFIG_ADDR + 64;
constexpr uint16_t NETWORK_PASS_ADDR   = CONFIG_ADDR + 128;

bool eeprom_write_multi(uint16_t addr, const uint8_t* data, size_t len);
bool eeprom_read_multi(uint16_t addr, uint8_t* buffer, size_t len);
bool is_valid_log_entry(uint16_t entry_addr);
void eeprom_log_init();
void eeprom_log_write(const char* message);
void eeprom_log_read_and_print();
void eeprom_log_erase();
bool eeprom_save_setpoint(float setpoint);
bool eeprom_load_setpoint(float* setpoint);
bool eeprom_save_string(uint16_t addr, const char* str, size_t maxLen);
bool eeprom_load_string(uint16_t addr, char* outStr, size_t maxLen);

#endif //RP2040_FREERTOS_IRQ_EEPROM_H