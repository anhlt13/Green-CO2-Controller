#include "i2c_bus.h"

std::shared_ptr<PicoI2C> i2c1Bus = nullptr;

void i2cBusInit()
{
    i2c1Bus = std::make_shared<PicoI2C>(1, 400000);
}