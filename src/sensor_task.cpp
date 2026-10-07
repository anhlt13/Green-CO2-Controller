//
// Created by Tran Lan Anh on 24.9.2026.
//
#include "sensor_task.h"
#include "system_state.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ModbusClient.h"
#include "PicoOsUart.h"
#include <cstdio>
#include <cstdint>

#include "ModbusRegister.h"


void sensor_task(void* parameters)
{
   auto uart = std::make_shared<PicoOsUart>(UART_NR, UART_TX_PIN, UART_RX_PIN, BAUD_RATE, STOP_BITS );
   auto rtu_client = std::make_shared<ModbusClient>(uart);

   ModbusRegister co2_data(rtu_client, GMP252_SLAVE_ADDRESS,GMP252_CO2_REGISTER, true);
   ModbusRegister humid_data(rtu_client,HMP60_SLAVE_ADDRESS, HMP60_HUMID_REGISTER, true);
   ModbusRegister temp_data(rtu_client, HMP60_SLAVE_ADDRESS, HMP60_TEMP_REGISTER, true);
   ModbusRegister fan_counter(rtu_client, FAN_SLAVE_ADDRESS,FAN_COUNTER_REGISTER, false);
   ModbusRegister fan_speed(rtu_client, FAN_SLAVE_ADDRESS, FAN_SPEED_REGISTER,true);
   while (true)
   {
      co2 = co2_data.read();
      //printf("co2 = %d ppm\n", co2);
      vTaskDelay(pdMS_TO_TICKS(10));

      humidity = static_cast<float>(humid_data.read()) / 10.0f;
      //printf("humidity = %2.f %% RH\n", humidity);
      vTaskDelay(pdMS_TO_TICKS(10));

      temperature = static_cast<float>(temp_data.read()) / 10.0f;
      //printf("temperature = %2.f C\n", temperature);
      vTaskDelay(pdMS_TO_TICKS(10));

      FanCounter = fan_counter.read();
       //printf("fan counter = %d\n", FanCounter);

       if (FanCounter > 0)
       {
           zeroCount = 0;
           fan_running = true;
       }else
       {
           zeroCount++;
           if (zeroCount >= 2)
           {
               fan_running = false;
           }
       }

       if (sharedStateMutex != nullptr)
       {
           xSemaphoreTake(sharedStateMutex, portMAX_DELAY);
           systemState.measuredCo2 = co2;
           systemState.relativeHumidity = humidity;
           systemState.temperature = temperature;
           systemState.fanPulseCount = FanCounter;
           systemState.fanRunning = fan_running;
           xSemaphoreGive(sharedStateMutex);
       }

       if (fanSpeedQueue != nullptr)
       {
           if (xQueueReceive(fanSpeedQueue, &requestedFanSpeedPercent, 0) == pdTRUE)
           {
               if (requestedFanSpeedPercent > 100)
               {
                   requestedFanSpeedPercent = 100;
               }
               fanSpeedRegisterValue = requestedFanSpeedPercent * 10;
               fan_speed.write(fanSpeedRegisterValue);
               printf("[FAN]fan speed request = %u%%, MIO value = %u\n", requestedFanSpeedPercent, fanSpeedRegisterValue );
               if (sharedStateMutex != nullptr)
               {
                   xSemaphoreTake(sharedStateMutex, portMAX_DELAY);
                   systemState.fanSpeedPercent = requestedFanSpeedPercent;
                   xSemaphoreGive(sharedStateMutex);
               }
           }
       }
      vTaskDelay(pdMS_TO_TICKS(1000));
   }

}

