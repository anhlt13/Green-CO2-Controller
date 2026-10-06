#ifndef I2C_BUS_H
#define I2C_BUS_H

#include <memory>
#include "PicoI2C.h"

extern std::shared_ptr<PicoI2C> i2c1Bus;

void i2cBusInit();

#endif