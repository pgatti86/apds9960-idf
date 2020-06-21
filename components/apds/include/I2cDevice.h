#ifndef I2cDevice_H
#define I2cDevice_H

#include <stdint.h>

class I2cDevice {
public:
    I2cDevice(void);
    ~I2cDevice(void);

    bool i2cInit(void);
    void delay(long millis);
    bool writeByte(uint8_t val);
    bool writeDataByte(uint8_t reg, uint8_t val);
    bool writeDataBlock(uint8_t reg, uint8_t *val, unsigned int len);
    bool readDataByte(uint8_t reg, uint8_t &val);
    int readDataBlock(uint8_t reg, uint8_t *val, unsigned int len);
};

#endif