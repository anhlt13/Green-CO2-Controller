//
// Created by Tran Lan Anh on 24.9.2026.
//

#ifndef RP2040_FREERTOS_IRQ_SENSOR_TASK_H
#define RP2040_FREERTOS_IRQ_SENSOR_TASK_H

#include "ModbusClient.h"

const uint8_t GMP252_SLAVE_ADDRESS = 240;
const uint16_t GMP252_CO2_REGISTER = 256;

const uint8_t HMP60_SLAVE_ADDRESS = 241;
const uint16_t HMP60_HUMID_REGISTER = 256;
const uint16_t HMP60_TEMP_REGISTER = 257;

const uint8_t FAN_SLAVE_ADDRESS = 1;
const uint8_t FAN_COUNTER_REGISTER = 4;
const uint8_t FAN_SPEED_REGISTER = 0;

void sensor_task(void *parameters);

inline uint16_t co2= 0;
inline uint16_t FanCounter = 0;
inline uint16_t requestedFanSpeedPercent = 0;
inline uint16_t fanSpeedRegisterValue = 0;
inline uint8_t zeroCount = 0;
inline bool fan_running = false;

inline float humidity;
inline float temperature;

#if 0
#define UART_NR 0
#define UART_TX_PIN 0
#define UART_RX_PIN 1
#else
#define UART_NR 1
#define UART_TX_PIN 4
#define UART_RX_PIN 5
#endif

#define BAUD_RATE 9600
#define STOP_BITS 2 // for real system (pico simualtor also requires 2 stop bits)



#endif //RP2040_FREERTOS_IRQ_SENSOR_TASK_H