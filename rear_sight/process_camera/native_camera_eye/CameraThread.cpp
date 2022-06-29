//
// Created by biba_bo on 2021-06-22.
//

#include "CameraThread.h"
#include "../../web_server/web_server_events/holder_commands.h"

Eye eye;
std::vector<const libcamera::Stream *> streams;
std::map<libcamera::FrameBuffer *, MappedBuffer> mappedBuffers;
libcamera::Rectangle sensorArea;
cv::Mat dst, dstYUV;
cv::Mat mainMatBGRA, roiMatBGRA;
//double zoom = 1;
std::map<int, uint64_t> lastBufferTime;
CameraThread *cameraThread;


void processInfo(int idStream, libcamera::FrameBuffer *buffer) {
    try {

        if (!lastBufferTime.count(idStream))
            lastBufferTime.insert({idStream, 0});

        const libcamera::FrameMetadata &metadata = buffer->metadata();

        double fps = metadata.timestamp - lastBufferTime[idStream];
        fps = lastBufferTime[idStream] && fps ? 1000000000.0 / fps : 0.0;
        lastBufferTime[idStream] = metadata.timestamp;

//        std::cout
//                << "idStream: " << idStream
//                << "\tseq: " << metadata.sequence
//                << "\tbytesused:" << metadata.planes[0].bytesused
//                << "\ttimestamp:" << metadata.timestamp
//                << "\tfps:" << fps << "\n";

    } catch (...) {
        std::cerr << "processInfo error\n";
    }
}

//int oldX = -1;

void processRequest(libcamera::Request *request) {
    if (request->status() == libcamera::Request::RequestCancelled)
        return;

    libcamera::Rectangle crop;

    const libcamera::Request::BufferMap &buffers = request->buffers();



    /* Process buffers. */
    if (request->buffers().count(streams.at(0))) {

        libcamera::FrameBuffer *buffer = request->buffers().at(streams.at(0));
        std::memcpy(dst.data, (uchar *) mappedBuffers[buffer].memory, mappedBuffers[buffer].size);
        cv::cvtColor(dst, mainMatBGRA, cv::COLOR_BGR2BGRA);

        if (!cameraThread)
            cameraThread = CameraThread::getCameraThreadInstance();
        if (cameraThread->isSwappedStreams()) {
            crop.x = 0;
            crop.y = 0;
            crop.width = WIDTH;
            crop.height = HEIGHT;
        } else {
            crop.x = cameraThread->getX();
            crop.y = cameraThread->getY();
            crop.width = cameraThread->getWidth();
            crop.height = cameraThread->getHeight();
        }

        processInfo(0, buffer);
    }


    if (streams.size() > 1)
        if (request->buffers().count(streams.at(1))) {
            libcamera::FrameBuffer *buffer = request->buffers().at(streams.at(1));

            std::memcpy(dstYUV.data, (uchar *) mappedBuffers[buffer].memory, mappedBuffers[buffer].size);
            cv::cvtColor(dstYUV, roiMatBGRA, cv::COLOR_YUV2BGRA_NV12);

            if (cameraThread->isSwappedStreams()) {
                crop.x = cameraThread->getX();
                crop.y = cameraThread->getY();
                crop.width = cameraThread->getWidth();
                crop.height = cameraThread->getHeight();
            } else {
                crop.x = 0;
                crop.y = 0;
                crop.width = WIDTH;
                crop.height = HEIGHT;
            }

            processInfo(1, buffer);
        }

    request->reuse(libcamera::Request::ReuseBuffers);

    crop.translateBy(sensorArea.topLeft());
    request->controls().set(libcamera::controls::ScalerCrop, crop);
//    request->controls().set(libcamera::controls::ExposureTime, 1000);
    request->controls().set(libcamera::controls::AeExposureMode, true);
    request->controls().set(libcamera::controls::AeConstraintMode, true);
    request->controls().set(libcamera::controls::AeMeteringMode, true);
//    request->controls().set(libcamera::controls::Brightness, 0.0);
    request->controls().set(libcamera::controls::AnalogueGain, 0.0);
    request->controls().set(libcamera::controls::Sharpness, 0);

    eye.AddRequest(request);

    if (roiMatBGRA.cols > 0 && roiMatBGRA.rows > 0) {
        cv::resize(roiMatBGRA, roiMatBGRA, cv::Size(RESIZE_WIDTH, RESIZE_HEIGHT));
        cv::Rect roi(cv::Point(RESIZE_X, RESIZE_Y), roiMatBGRA.size());
        roiMatBGRA.copyTo(mainMatBGRA(roi));
        cv::Mat clonedMat = mainMatBGRA.clone();
        cameraThread->drawOnMat(clonedMat);
        cameraThread->addMat(clonedMat);
    }
}


CameraThread::CameraThread() {
    _isSwappedStreams = false;
    _isActiveCameraThread = false;
    _isSetFuse = true;
    _isShowDebugMessages = false;
    _frameParameters = std::make_shared<FrameParameters>();
    _rearSightProcessor = std::make_shared<RearSightProcessor>(_frameParameters);
    _ballisticCalculator = std::make_shared<BallisticCalculator>();
    _delegate = DelegateWS::getInstance();

    // load from file points
    std::fstream myFile("src/roi_center.dat", std::ios_base::in);
    int roiCenterX = 0;
    int roiCenterY = 0;
    double roiPercent = 0.0;
    if (myFile >> roiCenterX &&
            myFile >> roiCenterY &&
            myFile >> roiPercent) {
        if (roiCenterX && roiCenterY) {
            _rearSightProcessor->setROICenter(roiCenterX, roiCenterY, roiPercent);
        }
    }
    _rearSightProcessor->enableFixedROICenter();
    myFile.close();

    _rearSightProcessor->set_new_frame_param();
    _savedFireCorrection = _ballisticCalculator->getCurrentCorrection();

    _tracker = std::make_shared<Tracker>();
    std::cout << "\n\n\n read coords: x=" << roiCenterX << " y=" << roiCenterY << " p=" << roiPercent << "\n\n\n";
}

CameraThread::~CameraThread() {

}

void CameraThread::processCameraThread() {
    _cameraStreamingThread = std::thread([this]() {
        std::string idSensor;

        std::basic_ofstream<char> myOfStream;
        libcamera::logSetLevel("*", "FATAL");
        libcamera::logSetStream(&myOfStream);

        CHECK(eye.Init());
        eye.ShowCamers();
        CHECK(eye.SetCameraId(0, idSensor));
        CHECK(eye.CameraConfiguration({libcamera::StreamRole::Viewfinder, libcamera::StreamRole::StillCapture}));

        CHECK(eye.SetConfiguration(0, {WIDTH_STREAM_FRAME, HEIGHT_STREAM_FRAME},
                                   libcamera::PixelFormat::fromString("RGB888")));
        CHECK(eye.SetConfiguration(1, {480, 320}, libcamera::PixelFormat::fromString("RGB888")));
        CHECK(eye.StreamConfiguration());
        CHECK(eye.BufferAlloc());
        CHECK(eye.CreateRequests());
        eye.SetCallBack([](libcamera::Request *request) {
            eye.RequestCompleted(processRequest, request);
        });


        sensorArea = eye.GetSensorArea();
        streams = eye.GetStreams();
        mappedBuffers = eye.GetMappedBuffers();
        libcamera::StreamConfiguration &streamConfig = eye.GetStreamConfiguration(0);

        dst = cv::Mat(
                eye.GetStreamConfiguration(0).size.height,
                eye.GetStreamConfiguration(0).size.width, CV_8UC3
        );
        if (streams.size() > 1)
            dstYUV = cv::Mat(
                    eye.GetStreamConfiguration(1).size.height * 3 / 2,
                    eye.GetStreamConfiguration(1).size.width, CV_8UC1 //CV_8UC1
            );

        libcamera::ControlList controlList;
        controlList.set(libcamera::controls::ScalerCrop,
                        libcamera::Rectangle(0, 0, sensorArea.width, sensorArea.height));

        CHECK(eye.Start(&controlList));
        CHECK(eye.Join());
    });

}

void CameraThread::startTracking(int x, int y) {
    std::cout << "start x: " << x << " start y: " << y << "\n";
    cv::Mat init_frame = getLastMat();
    //std::cout << "last math: " << init_frame.size[0] << " : "<< init_frame.size[1] << "\n";
    //float m_frame = std::max(init_frame.size[0], init_frame.size[1]);
    //int xp;
    //int yp;
    //xp = (init_frame.size[1] / 2) + (x*m_frame);
    //yp = (init_frame.size[0] / 2) + (y*m_frame);
    //std::cout << "xp: " << xp << " yp: " << yp << "\n";
    _bb = cv::Rect(x-50, y-50, 100, 100);
    _tracker->init(init_frame, _bb);
    cv::rectangle(init_frame, _bb, cv::Scalar(0, 255, 0), 1, cv::LINE_8, 0);
    cv::imwrite("initframe.png", init_frame);
    TRACKING_FLAG = true;
}

void CameraThread::startCameraStream() {
    if (_isActiveCameraThread)
        return;
    _isActiveCameraThread = true;
    processCameraThread();
}

void CameraThread::stopCameraStream() {
    if (!_isActiveCameraThread)
        return;
    _isActiveCameraThread = false;
}

void CameraThread::joinCameraThread() {
    if (!_isActiveCameraThread)
        return;
    if (_cameraStreamingThread.joinable())
        _cameraStreamingThread.join();
}

CameraThread *CameraThread::_cameraThreadInstance = 0;

CameraThread *CameraThread::getCameraThreadInstance() {
    if (!_cameraThreadInstance)
        _cameraThreadInstance = new CameraThread();
    return _cameraThreadInstance;
}

bool CameraThread::isHasFramesInQueue() {
    return _matsQueue.size() > 0;
}

void CameraThread::addMat(cv::Mat newFrame) {
    if (_matsQueue.size() + 1 > MAX_MATS_QUEUE_SIZE)
        _matsQueue.pop_front();
    if(TRACKING_FLAG) {
        auto correction = _tracker->update(newFrame, _bb);
        std::cout << correction.first << std::endl;
        if(!correction.first) TRACKING_FLAG = false;
        else {
             _delegate->doEvent(std::make_shared<EventWS>(EVENT_CAMERA_DELTA, correction.second.first, correction.second.second));
            std::cout << "deltaX: " << correction.second.first << ", deltaY: " << correction.second.second << std::endl;
            cv::rectangle(newFrame, _bb, cv::Scalar(0, 255, 0), 1, cv::LINE_8, 0);
        }
    }
    _matsQueue.push_back(newFrame);
}

cv::Mat CameraThread::getLastMat() {
    if (isHasFramesInQueue())
        return _matsQueue.front();
    return cv::Mat();
}

void CameraThread::drawOnMat(cv::Mat &matForDrawing) {
    if (_isSwappedStreams) {
        int roiX = RESIZE_X + (int) ((float) _frameParameters->CROPPED_X * (float) WIDTH_STREAM_FRAME / (float) WIDTH);
        int roiY = RESIZE_Y + (int) ((float) _frameParameters->CROPPED_Y * (float) HEIGHT_STREAM_FRAME / (float) HEIGHT);
        int roiWidth = (int) ((float) _frameParameters->CROPPED_WIDTH * (float) WIDTH_STREAM_FRAME / (float) WIDTH);
        int roiHeight = (int) ((float) _frameParameters->CROPPED_HEIGHT * (float) HEIGHT_STREAM_FRAME / (float) HEIGHT);

        cv::Rect myInterestRegion(roiX, roiY, roiWidth, roiHeight);
        cv::rectangle(matForDrawing, myInterestRegion, FocalDrawing::COLOR_RED, 2, 0, 0);

        cv::line(matForDrawing, cv::Point(RESIZE_WIDTH / 2, 37), cv::Point(RESIZE_WIDTH / 2, RESIZE_HEIGHT - 37),
                 _isSetFuse ? FocalDrawing::COLOR_ORANGE : FocalDrawing::COLOR_RED, 2, cv::LINE_8);
        cv::line(matForDrawing, cv::Point(125, RESIZE_HEIGHT / 2), cv::Point(RESIZE_WIDTH - 125, RESIZE_HEIGHT / 2),
                 _isSetFuse ? FocalDrawing::COLOR_ORANGE : FocalDrawing::COLOR_RED, 2, cv::LINE_8);

    } else {
        int roiX = RESIZE_X + (int) ((float) _frameParameters->CROPPED_X * (float) RESIZE_WIDTH / (float) WIDTH);
        int roiY = RESIZE_Y + (int) ((float) _frameParameters->CROPPED_Y * (float) RESIZE_HEIGHT / (float) HEIGHT);
        int roiWidth = (int) ((float) _frameParameters->CROPPED_WIDTH * (float) RESIZE_WIDTH / (float) WIDTH);
        int roiHeight = (int) ((float) _frameParameters->CROPPED_HEIGHT * (float) RESIZE_HEIGHT / (float) HEIGHT);

        cv::Rect myInterestRegion(roiX, roiY, roiWidth, roiHeight);
        cv::rectangle(matForDrawing, myInterestRegion, FocalDrawing::COLOR_RED, 2, 0, 0);

        cv::line(matForDrawing, cv::Point(DRAW_LINE_1B_X, DRAW_LINE_1B_Y), cv::Point(DRAW_LINE_1E_X, DRAW_LINE_1E_Y),
                 _isSetFuse ? FocalDrawing::COLOR_ORANGE : FocalDrawing::COLOR_RED, 1, cv::LINE_8);
        cv::line(matForDrawing, cv::Point(DRAW_LINE_2B_X, DRAW_LINE_2B_Y), cv::Point(DRAW_LINE_2E_X, DRAW_LINE_2E_Y),
                 _isSetFuse ? FocalDrawing::COLOR_ORANGE : FocalDrawing::COLOR_RED, 1, cv::LINE_8);
        cv::line(matForDrawing, cv::Point(DRAW_LINE_3B_X, DRAW_LINE_3B_Y), cv::Point(DRAW_LINE_3E_X, DRAW_LINE_3E_Y),
                 _isSetFuse ? FocalDrawing::COLOR_ORANGE : FocalDrawing::COLOR_RED, 1, cv::LINE_8);
        cv::line(matForDrawing, cv::Point(DRAW_LINE_4B_X, DRAW_LINE_4B_Y), cv::Point(DRAW_LINE_4E_X, DRAW_LINE_4E_Y),
                 _isSetFuse ? FocalDrawing::COLOR_ORANGE : FocalDrawing::COLOR_RED, 1, cv::LINE_8);

        // draw focal plane dots and lines
        recalculateFocalPoints(_rearSightProcessor->isFixedROICenter() ? _rearSightProcessor->getROIPercentCorrected() : _rearSightProcessor->getROIPercent());
        for (int i = 0; i < 4; i++) {
            if (_focalPointsVector[i] > 1.0)
                break;
            cv::circle(matForDrawing,
                       cv::Point(DRAW_CIRCLE_X + (int) (_focalPointsVector[i] * (double) DRAW_LINE_LENGTH),
                                 DRAW_CIRCLE_Y),
                       DRAW_FOCAL_CIRCLE_RADIUS, _isSetFuse ? FocalDrawing::COLOR_ORANGE : FocalDrawing::COLOR_RED,
                       2, cv::LINE_8, 0);
            cv::circle(matForDrawing,
                       cv::Point(DRAW_CIRCLE_X - (int) (_focalPointsVector[i] * (double) DRAW_LINE_LENGTH),
                                 DRAW_CIRCLE_Y),
                       DRAW_FOCAL_CIRCLE_RADIUS, _isSetFuse ? FocalDrawing::COLOR_ORANGE : FocalDrawing::COLOR_RED,
                       2, cv::LINE_8, 0);
            cv::circle(matForDrawing,
                       cv::Point(DRAW_CIRCLE_X,
                                 DRAW_CIRCLE_Y + (int) (_focalPointsVector[i] * (double) DRAW_LINE_LENGTH)),
                       DRAW_FOCAL_CIRCLE_RADIUS, _isSetFuse ? FocalDrawing::COLOR_ORANGE : FocalDrawing::COLOR_RED,
                       2, cv::LINE_8, 0);
            cv::circle(matForDrawing,
                       cv::Point(DRAW_CIRCLE_X,
                                 DRAW_CIRCLE_Y - (int) (_focalPointsVector[i] * (double) DRAW_LINE_LENGTH)),
                       DRAW_FOCAL_CIRCLE_RADIUS, _isSetFuse ? FocalDrawing::COLOR_ORANGE : FocalDrawing::COLOR_RED,
                       2, cv::LINE_8, 0);
        }

        if (_focalPointsVector[4] < 1.0) {
            cv::line(matForDrawing,
                     cv::Point(DRAW_CIRCLE_X + (int) (_focalPointsVector[4] * (double) DRAW_LINE_LENGTH),
                               DRAW_CIRCLE_Y),
                     cv::Point(DRAW_CIRCLE_X + DRAW_LINE_LENGTH, DRAW_CIRCLE_Y),
                     _isSetFuse ? FocalDrawing::COLOR_ORANGE : FocalDrawing::COLOR_RED,3, cv::LINE_8);
            cv::line(matForDrawing,
                     cv::Point(DRAW_CIRCLE_X - (int) (_focalPointsVector[4] * (double) DRAW_LINE_LENGTH),
                               DRAW_CIRCLE_Y),
                     cv::Point(DRAW_CIRCLE_X - DRAW_LINE_LENGTH, DRAW_CIRCLE_Y),
                     _isSetFuse ? FocalDrawing::COLOR_ORANGE : FocalDrawing::COLOR_RED, 3, cv::LINE_8);
            cv::line(matForDrawing,
                     cv::Point(DRAW_CIRCLE_X,
                               DRAW_CIRCLE_Y + (int) (_focalPointsVector[4] * (double) DRAW_LINE_LENGTH)),
                     cv::Point(DRAW_CIRCLE_X, DRAW_CIRCLE_Y + DRAW_LINE_LENGTH),
                     _isSetFuse ? FocalDrawing::COLOR_ORANGE : FocalDrawing::COLOR_RED, 3, cv::LINE_8);
            cv::line(matForDrawing,
                     cv::Point(DRAW_CIRCLE_X,
                               DRAW_CIRCLE_Y - (int) (_focalPointsVector[4] * (double) DRAW_LINE_LENGTH)),
                     cv::Point(DRAW_CIRCLE_X, DRAW_CIRCLE_Y - DRAW_LINE_LENGTH),
                     _isSetFuse ? FocalDrawing::COLOR_ORANGE : FocalDrawing::COLOR_RED, 3, cv::LINE_8);
        }
    }

    // here is shows some debug messages:
    if (_isShowDebugMessages) {
        // print coordinates and distance
        std::string coordTextInfo = "CENTER: X=" + std::to_string(_rearSightProcessor->getROICenterPoint().first) +
                " Y=" + std::to_string(_rearSightProcessor->getROICenterPoint().second);
        cv::putText(matForDrawing, coordTextInfo, cv::Point(5, HEIGHT_STREAM_FRAME - 15),
                    cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0,
                    cv::Scalar(0, 0, 255), 1, cv::LINE_AA);
        std::string focalDeltaInfo = "Focal Delta: " + std::to_string(_focalDelta);
        cv::putText(matForDrawing, focalDeltaInfo, cv::Point(5, HEIGHT_STREAM_FRAME - 35),
                    cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0,
                    cv::Scalar(0, 0, 255), 1, cv::LINE_AA);
        std::string percentZoomDeltaInfo =
                "SCALE: " + std::to_string(_rearSightProcessor->getROIPercent() * 100.0) + "%, SCALE_CORR: " + std::to_string(_rearSightProcessor->getROIPercentCorrected() * 100.0) + "%";
        cv::putText(matForDrawing, percentZoomDeltaInfo, cv::Point(5, HEIGHT_STREAM_FRAME - 55),
                    cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0,
                    cv::Scalar(0, 0, 255), 1, cv::LINE_AA);

        // print some ballistic information
        std::string ballisticInfo = "Dist: " + std::to_string(_savedFireCorrection.distance) + "m"
                + ", dx: " + std::to_string(_savedFireCorrection.xCorrection) + "px"
                + ", dy: " + std::to_string(_savedFireCorrection.yCorrection) + "px";
        cv::putText(matForDrawing, ballisticInfo, cv::Point(5, HEIGHT_STREAM_FRAME - 75),
                    cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0,
                    cv::Scalar(0, 0, 255), 1, cv::LINE_AA);
    }
}

void CameraThread::moveUP() {
    _rearSightProcessor->on_move_up_processor();
    _rearSightProcessor->set_new_frame_param();
}

void CameraThread::moveDOWN() {
    _rearSightProcessor->on_move_down_processor();
    _rearSightProcessor->set_new_frame_param();
}

void CameraThread::moveLEFT() {
    _rearSightProcessor->on_move_left_processor();
    _rearSightProcessor->set_new_frame_param();
}

void CameraThread::moveRIGHT() {
    _rearSightProcessor->on_move_right_processor();
    _rearSightProcessor->set_new_frame_param();
}

void CameraThread::zoomPlus() {
    _rearSightProcessor->on_zoom_plus_processor();
    _rearSightProcessor->set_new_frame_param();
}

void CameraThread::zoomMinus() {
    _rearSightProcessor->on_zoom_minus_processor();
    _rearSightProcessor->set_new_frame_param();
}

double CameraThread::getCroppingPercent() {
    return _rearSightProcessor->getROIPercentCorrected();
}

int CameraThread::getX() {
    return _frameParameters->CROPPED_X;
}

int CameraThread::getY() {
    return _frameParameters->CROPPED_Y;
}

int CameraThread::getWidth() {
    return _frameParameters->CROPPED_WIDTH;
}

int CameraThread::getHeight() {
    return _frameParameters->CROPPED_HEIGHT;
}

void CameraThread::recalculateFocalPoints(double cropping) {
    _focalDelta = (_ballisticCalculator->getFocalPixelsCount() / cropping) / (double) DRAW_LINE_LENGTH;

    for (int i = 0; i < 5; i++)
        _focalPointsVector[i] = _focalDelta * (i + 1);
}

void CameraThread::startRegistrationFire() {
    _rearSightProcessor->disableFixedROICenter();
    _rearSightProcessor->set_new_frame_param();
}

void CameraThread::stopRegistrationFire() {
    _rearSightProcessor->enableFixedROICenter();
    _rearSightProcessor->set_new_frame_param();

    // and save to file
    std::ofstream myFile;
    myFile.open("src/roi_center.dat", std::ios::out | std::ofstream::trunc);
    myFile << _rearSightProcessor->getROICenterPoint().first << "\n"
           << _rearSightProcessor->getROICenterPoint().second << "\n"
           << (_rearSightProcessor->getROIPercent() - 0.001) << std::endl;
    myFile.close();
    _ballisticCalculator->saveToFile();
}

bool CameraThread::isRegistrationFire() {
    return !_rearSightProcessor->isFixedROICenter();
}

void CameraThread::swapStreams(){
    _isSwappedStreams = !_isSwappedStreams;
}

bool CameraThread::isSwappedStreams() {
    return _isSwappedStreams;
}

void CameraThread::planeDotsPlusDistance() {
    if (!_rearSightProcessor->isFixedROICenter())
        _ballisticCalculator->planeDotsPlusDistance();
}

void CameraThread::planeDotsMinusDistance() {
    if (!_rearSightProcessor->isFixedROICenter())
        _ballisticCalculator->planeDotsMinusDistance();
}

void CameraThread::removeFuse() {
    _isSetFuse = false;
}

void CameraThread::installFuse() {
    _isSetFuse = true;
}

void CameraThread::setNextFireCorrection() {
    _ballisticCalculator->setNextCorrection();
    _rearSightProcessor->setCorrections(_ballisticCalculator->getCurrentCorrection().xCorrection,
                                        _ballisticCalculator->getCurrentCorrection().yCorrection);
    _rearSightProcessor->set_new_frame_param();
    _savedFireCorrection = _ballisticCalculator->getCurrentCorrection();
}

void CameraThread::setPreviousFireCorrection() {
    _ballisticCalculator->setPreviousCorrection();
    _rearSightProcessor->setCorrections(_ballisticCalculator->getCurrentCorrection().xCorrection,
                                        _ballisticCalculator->getCurrentCorrection().yCorrection);
    _rearSightProcessor->set_new_frame_param();
    _savedFireCorrection = _ballisticCalculator->getCurrentCorrection();
}

std::string CameraThread::getFireCorrectionsLineJSON() {
    return _ballisticCalculator->getFireCorrectionsJSON();
}

int CameraThread::getFireCorrectionIndex() {
    return _ballisticCalculator->getFireCorrectionIndex();
}

void CameraThread::setFireCorrectionIndex(int index) {
    _ballisticCalculator->setFireCorrectionIndex(index);
    _rearSightProcessor->setCorrections(_ballisticCalculator->getCurrentCorrection().xCorrection,
                                        _ballisticCalculator->getCurrentCorrection().yCorrection);
    _rearSightProcessor->set_new_frame_param();
    _savedFireCorrection = _ballisticCalculator->getCurrentCorrection();
}

void CameraThread::setFireCorrections(double dx, double dy) {
    double scale = _rearSightProcessor->isFixedROICenter() ? _rearSightProcessor->getROIPercentCorrected()
            : _rearSightProcessor->getROIPercent();
    _ballisticCalculator->fixFireCorrection(dx * ((double) WIDTH) * scale, dy * ((double) HEIGHT) * scale);
    _rearSightProcessor->setCorrections(_ballisticCalculator->getCurrentCorrection().xCorrection,
                                        _ballisticCalculator->getCurrentCorrection().yCorrection);
    _rearSightProcessor->set_new_frame_param();
    _savedFireCorrection = _ballisticCalculator->getCurrentCorrection();
}

void CameraThread::setShowingDebugMessage() {
    _isShowDebugMessages = !_isShowDebugMessages;
}

void CameraThread::setDefaultFireCorrectionTable() {
    _ballisticCalculator->setDefaultFireCorrectionTable();

    _rearSightProcessor->setCorrections(_ballisticCalculator->getCurrentCorrection().xCorrection,
                                        _ballisticCalculator->getCurrentCorrection().yCorrection);
    _rearSightProcessor->set_new_frame_param();
    _savedFireCorrection = _ballisticCalculator->getCurrentCorrection();
}

void CameraThread::setDefaultFireCorrectionRecord(int index) {
    _ballisticCalculator->setDefaultFireCorrectionRecord(index);

    _rearSightProcessor->setCorrections(_ballisticCalculator->getCurrentCorrection().xCorrection,
                                        _ballisticCalculator->getCurrentCorrection().yCorrection);
    _rearSightProcessor->set_new_frame_param();
    _savedFireCorrection = _ballisticCalculator->getCurrentCorrection();
}
