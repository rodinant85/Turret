//
// Created by biba_bo on 2021-10-13.
//

#include "spi.h"
#include <mutex>

std::mutex lock_spi;


int SPI::init(std::string device, uint8_t mode, uint8_t bits, uint32_t speed, uint8_t cs) {
    if (_isInited)
        return 0;
    _isInited = true;

    int ret = 0;

    _device = device;
    _speed = speed;
    _cs = cs;
    _bits = bits;
    _mode = mode;

    LOG_INFO("SPI: ", _device, ", Speed: ", _speed, ", Mode: 0x", std::hex, (unsigned short int)_mode, std::dec);

    fd = open(device.c_str(), O_RDWR);
    if (fd < 0)
        LOG_ERROR("can't open device: ", device);

    /*
	 * spi mode
	 */
    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    if (ret == -1)
        LOG_ERROR("Can't set spi mode: 0x" , std::hex,  (unsigned short int)mode, std::dec);

    ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
    if (ret == -1)
        LOG_ERROR("Can't get spi mode: 0x" , std::hex,  (unsigned short int)mode, std::dec);

    /*
     * bits per word
     */
    ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    if (ret == -1)
        LOG_ERROR("Can't set bits per word: " , std::dec, (unsigned short int)bits);

    ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
    if (ret == -1)
        LOG_ERROR("Can't get bits per word: " , std::dec, (unsigned short int)bits);

    /*
     * max speed hz
     */
    ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1)
        LOG_ERROR("Can't set max speed hz: " , std::dec, speed);

    ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
    if (ret == -1)
        LOG_ERROR("Can't get max speed hz: " , std::dec, speed);

    return ret;
}


uint16_t  SPI::transfer16(uint16_t data) {

    uint16_t data_ret;
    int ret;

    struct spi_ioc_transfer mesg = {
            mesg.tx_buf = (unsigned long)&data,
            mesg.rx_buf = (unsigned long)&data_ret,
            mesg.len = sizeof(uint16_t),
            mesg.cs_change = _cs,
    };

    lock_spi.lock();
    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &mesg);
    lock_spi.unlock();

    if(ret == 1)
        LOG_ERROR("Can't SEND spi message");

    return data_ret;
}

uint8_t  SPI::transfer8(uint8_t data) {

    uint8_t data_ret;
    int ret;

    struct spi_ioc_transfer mesg = {
            mesg.tx_buf = (unsigned long)&data,
            mesg.rx_buf = (unsigned long)&data_ret,
            mesg.len = sizeof(uint8_t),
            mesg.cs_change = _cs,
    };

    lock_spi.lock();
    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &mesg);
    lock_spi.unlock();

    if(ret == 1)
        LOG_ERROR("Can't SEND spi message");

    return data_ret;
}

bool  SPI::sendArray(uint8_t *data, uint32_t len) {
    // print array
/*    std::cout << "MSG: ";
    for (uint32_t i = 0; i < len; i++)
        std::cout << (int) data[i] << " ";
    std::cout << "\n";*/

    int ret;

    struct spi_ioc_transfer mesg = {
            mesg.tx_buf = (unsigned long)data,
            mesg.rx_buf = (unsigned long)data,
            mesg.len = len,
            mesg.cs_change = _cs,
    };

    lock_spi.lock();
    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &mesg);
    lock_spi.unlock();

    if(ret == 1) {
        LOG_ERROR("Can't SEND spi message");

        return false;
    }

    return true;
}

bool SPI::receiveArray(uint8_t *data, uint32_t len) {
    int ret;
    struct spi_ioc_transfer mesg = {
            mesg.tx_buf = (unsigned long)data,
            mesg.rx_buf = (unsigned long)data,
            mesg.len = len,
            mesg.cs_change = _cs,
    };

    lock_spi.lock();
    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &mesg);
    lock_spi.unlock();

    if(ret == 1) {
        LOG_ERROR("Can't RECEIVE spi message");

        return false;
    }

    return true;
}
