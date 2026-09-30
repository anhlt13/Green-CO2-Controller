//
// Created by baotr on 9/24/2026.
//

#include "relay.h"

void relayInit()
{
    gpio_init(CO2_VALVE_PIN);
    gpio_set_dir(CO2_VALVE_PIN, GPIO_OUT);
    gpio_put(CO2_VALVE_PIN,0);
}

void openCO2Valve()
{
    gpio_put(CO2_VALVE_PIN,1);
}

void closeCO2Valve()
{
    gpio_put(CO2_VALVE_PIN,0);
}


