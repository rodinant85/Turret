//
// Created by biba_bo on 2021-10-20.
//

#include "holder_controller.h"

HolderController::HolderController() {
    _spiDriver = std::make_shared<SPI>();
    _spiDriver->init("/dev/spidev0.0", SPI::SpiMode::MODE1, 8, 1100000, 0);

    processHolderControllerThread();
}

HolderController::HolderController(std::shared_ptr<SPI> spiDriver) : _spiDriver(spiDriver) {
    processHolderControllerThread();
}


HolderController::~HolderController() {
    _isProcessThread = false;
    if (_movingProcessingThread.joinable())
        _movingProcessingThread.join();

}

void HolderController::recalculateMessageSum() {
    uint16_t messageSum = 0;
    for (int i = 1; i < _BUFFER_OUT_SIZE - 1; i++)
        messageSum += (uint16_t) _messageOutBuffer[i];

    messageSum %= 256;
    _messageOutBuffer[_BUFFER_OUT_SIZE - 1] = (uint8_t) messageSum;
}

void HolderController::setMoveSpeed(uint8_t xSpeed, uint8_t ySpeed, uint8_t direction) {
    _waitingForNextSpeedMessage = HolderThreadConsts::CLIENT_MSG_WAIT_STEPS;

    _speedInfoBuffer[0] = xSpeed;
    _speedInfoBuffer[1] = ySpeed;
    _speedInfoBuffer[2] = direction;

    // also we need disable fire sending
    _isMessageReceivingActive = false;
    _isHasFireMessage = false;
}

void HolderController::stopMoving() {
    setMoveSpeed(0x00, 0x00, 0x00);
}

void HolderController::processHolderControllerThread() {
    _waitingForNextSpeedMessage = 0;
    _isProcessThread = true;

    this->_movingProcessingThread = std::thread([this]() {
        while (_isProcessThread) {
            if (_isHasFireMessage) {
                _isHasFireMessage = false;

                sendFireMessage();

                // sleep in keep alive mode
                std::this_thread::sleep_for(std::chrono::microseconds(HolderThreadConsts::SPI_KEEP_ALIVE_MSG_TIMEOUT));
            } else if (_waitingForNextSpeedMessage) {
                _waitingForNextSpeedMessage--;
                sendSpeedMessage();

                // sleep in sending speed mode
                std::this_thread::sleep_for(std::chrono::microseconds(HolderThreadConsts::SPI_SPEED_MSG_TIMEOUT));
            } else {
                stopMoving();
                _waitingForNextSpeedMessage = 1000;

                // sleep in keep alive mode
                std::this_thread::sleep_for(std::chrono::microseconds(HolderThreadConsts::SPI_KEEP_ALIVE_MSG_TIMEOUT));
            }
        }
    });
}

void HolderController::sendSpeedMessage() {
    _messageOutBuffer[0] = 0xFF;
    _messageOutBuffer[1] = HolderMessageType::SPEED_SET_MSG;
    _messageOutBuffer[2] = _speedInfoBuffer[0];
    _messageOutBuffer[3] = _speedInfoBuffer[1];
    _messageOutBuffer[4] = _speedInfoBuffer[2];

//    std::cout << "Msg: " << (int) _messageOutBuffer[0] << " "
//                         << (int) _messageOutBuffer[1] << " "
//                         << (int) _messageOutBuffer[2] << " "
//                         << (int) _messageOutBuffer[3] << " "
//                         << (int) _messageOutBuffer[4] << " "
//                         << (int) _messageOutBuffer[5] << "\n";

    recalculateMessageSum();
    _spiDriver->sendArray(_messageOutBuffer, _BUFFER_OUT_SIZE);
}

void HolderController::sendFireMessage() {
    _messageOutBuffer[0] = 0xFF;
    _messageOutBuffer[1] = _fireInfoBuffer[0];
    _messageOutBuffer[2] = _fireInfoBuffer[1];
    _messageOutBuffer[3] = _fireInfoBuffer[2];
    _messageOutBuffer[4] = _fireInfoBuffer[3];

    recalculateMessageSum();
    _spiDriver->sendArray(_messageOutBuffer, _BUFFER_OUT_SIZE);
}

void HolderController::fireOn(uint8_t count) {
    _fireInfoBuffer[0] = HolderMessageType::SHOOT_COUNT_MSG;
    _fireInfoBuffer[1] = count;
    _fireInfoBuffer[2] = 0x00;
    _fireInfoBuffer[3] = 0x00;

//    if (_isMessageReceivingActive)
//        return;
//
    _isHasFireMessage = true;
//    _isMessageReceivingActive = true;
//    if (_messageReceivingThread.joinable())
//        _messageReceivingThread.detach();
//    _messageReceivingThread = std::thread([this]() {
//        std::cout << "\n\n\n\n\n\n\n#### THREAD STARTED ######\n\n\n\n\n\n\n";
//        uint8_t buffer[12] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//        uint32_t bufferLength = 12;
//        while (_isMessageReceivingActive) {
//            _spiDriver->receiveArray(buffer, bufferLength);
//            uint32_t messageStart = 0xFF;
//            for (uint32_t i = 0; i < 7; i++) {
//                if (buffer[i] == 0xFF) {
//                    messageStart = i;
//                }
//            }
//
//            if (messageStart != 0xFF) {
//	            if (buffer[messageStart + 1] == HolderMessageType::SHOOT_COUNT_MSG) {
//        	        _isMessageReceivingActive = false;
//	                _isHasFireMessage = false;
//        	    }
//                    std::cout << "\nNice!!!\n";
//	    }
//
//            std::cout << "### RECV SPI: ";
//            for (uint32_t i = 0; i < bufferLength; i++) {
//                std::cout << (int) buffer[i] << ", ";
//                buffer[i] = 0x0A;
//            }
//            std::cout << "\n";
//
//            // TODO: DOES I NEED SLEEP TO DECREASE CPY USAGE
//             std::this_thread::sleep_for(std::chrono::microseconds(HolderThreadConsts::SPI_READ_MSG_TIMEOUT));
//        }
//    });
}
