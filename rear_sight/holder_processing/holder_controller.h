//
// Created by biba_bo on 2021-10-20.
//

#ifndef RPI_SPI_TESTING_HOLDER_CONTROLLER_H
#define RPI_SPI_TESTING_HOLDER_CONTROLLER_H

#include <memory>
#include <thread>
#include <atomic>

#include "spi_library/spi.h"

namespace HolderMessageType {
    constexpr uint8_t SPEED_SET_MSG = 0x00;
    constexpr uint8_t SHOOT_COUNT_MSG = 0xAA;
    constexpr uint8_t SHOOT_PAUSE_MSG = 0xAB;
    constexpr uint8_t SHOOT_RESUME_MSG = 0xAC;
    constexpr uint8_t SHOOT_STOP_MSG = 0xAD;
}

namespace HolderMovementDirection {
    constexpr uint8_t DOWN_RIGHT = 0x00;
    constexpr uint8_t DOWN_LEFT = 0x01;
    constexpr uint8_t UP_LEFT = 0x02;
    constexpr uint8_t UP_RIGHT = 0x03;
}


namespace HolderMovementConst {
    constexpr uint8_t X_STOP_SPEED = 0x00;
    constexpr uint8_t Y_STOP_SPEED = 0x00;
    constexpr uint8_t X_MAX_SPEED = 0xFE;
    constexpr uint8_t Y_MAX_SPEED = 0xFE;
}

namespace HolderThreadConsts {
    constexpr int SPI_SPEED_MSG_TIMEOUT = 50000;
    constexpr int SPI_KEEP_ALIVE_MSG_TIMEOUT = 50000;
    constexpr int SPI_READ_MSG_TIMEOUT = 500;
    constexpr int CLIENT_MSG_WAIT_STEPS = 3;
}

class HolderController {
    const uint32_t _BUFFER_OUT_SIZE = 6;
    uint8_t _messageOutBuffer[6] = {0xFF, 0x00, 0x00,
                                    0x00, 0x00, 0x00};
    uint8_t _speedInfoBuffer[3] = {0x00, 0x00, 0x00};
    uint8_t _fireInfoBuffer[4] = {0x00, 0x00, 0x00, 0x00};
    std::shared_ptr<SPI> _spiDriver;


    std::thread _movingProcessingThread;
    std::thread _messageReceivingThread;
    std::atomic<int> _waitingForNextSpeedMessage;
    std::atomic<bool> _isProcessThread;
    std::atomic<bool> _isHasFireMessage = false;
    std::atomic<bool> _isMessageReceivingActive = false;

    void recalculateMessageSum();
    void processHolderControllerThread();

    void sendSpeedMessage();
    void sendFireMessage();

public:
    HolderController();
    HolderController(std::shared_ptr<SPI> spiDriver);
    ~HolderController();

    void setMoveSpeed(uint8_t xSpeed, uint8_t ySpeed, uint8_t direction);
    void stopMoving();
    void fireOn(uint8_t countFires);
};


#endif //RPI_SPI_TESTING_HOLDER_CONTROLLER_H
