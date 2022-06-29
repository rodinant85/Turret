//
// Created by biba_bo on 2021-10-13.
//

#ifndef RPI_SPI_TESTING_SPI_H
#define RPI_SPI_TESTING_SPI_H


#include <string>

#ifdef __linux__

#include <iostream>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#endif

#include "../../logging/LOG.h"


class ISPI {
public:

    enum SpiMode {
        MODE0 = 0|0,
        MODE1 = 0|SPI_CPHA,
        MODE2 = SPI_CPOL|0,
        MODE3 = SPI_CPOL|SPI_CPHA,

    };

    virtual int init(std::string device, uint8_t mode, uint8_t bits, uint32_t speed, uint8_t cs) = 0;
    virtual uint16_t transfer16(uint16_t data) = 0;
    virtual uint8_t  transfer8(uint8_t data) = 0;
    virtual bool sendArray(uint8_t *data, uint32_t len) = 0;
    virtual bool receiveArray(uint8_t *data, uint32_t len) = 0;

    int fd;

    uint8_t _mode;

    uint8_t _cs;
    uint8_t _bits;
    uint32_t _speed;
    std::string _device;
};


class SPI : public ISPI {
    bool _isInited = false;

public:

    int init(std::string device, uint8_t mode, uint8_t bits, uint32_t speed, uint8_t cs);
    uint16_t transfer16(uint16_t data);
    uint8_t  transfer8(uint8_t data);
    bool sendArray(uint8_t *data, uint32_t len);
    bool receiveArray(uint8_t *data, uint32_t len);
};

#endif //RPI_SPI_TESTING_SPI_H
