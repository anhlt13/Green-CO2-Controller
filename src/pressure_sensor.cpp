//
// Created by baotr on 9/28/2026.
//

#include "pressure_sensor.h"

static constexpr float SDP610_125PA_SCALE_FACTOR =  240.0f;

bool readPressurePa(std::shared_ptr<PicoI2C> i2c, float *outPa)
{
    uint8_t triggerCmd = 0xF1;
    uint written = i2c->write(SDP610_I2C_ADDR, &triggerCmd, 1);
    if (written !=1)
    {
        return false;
    }

    uint8_t buf[2];
    uint readBytes = i2c->read(SDP610_I2C_ADDR, buf, 2);
    if (readBytes !=2)
    {
        return false;
    }

    int16_t raw = (int16_t)((buf[0]<<8)|buf[1]);

    *outPa = (float)raw/SDP610_125PA_SCALE_FACTOR;
    return true;
}
