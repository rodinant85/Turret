//
// Created by biba_bo on 2021-06-22.
//

#ifndef EYE_CAMERATHREAD_H
#define EYE_CAMERATHREAD_H

#define CHECK(ret) assert((ret) == 0)
constexpr int MAX_MATS_QUEUE_SIZE = 2;

#include <iostream>
#include <thread>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

#include "Eye.h"
#include "../rear_sight_processor/rear_sight_processor.h"
#include "../ballistic_calculator/ballistic_calculator.h"
#include "../../web_server/lib_my_event_handler/delegate_ws.h"
#include "Tracker.h"

namespace FocalDrawing {
    const cv::Scalar COLOR_RED(0, 0, 255);
    const cv::Scalar COLOR_ORANGE(100, 150, 250);
}

void processInfo(int idStream, libcamera::FrameBuffer *buffer);
void processRequest(libcamera::Request *request);

class CameraThread {
private:
    static CameraThread * _cameraThreadInstance;    //private static field for the singular link on the CameraThread object

    std::shared_ptr<FrameParameters> _frameParameters;
    std::shared_ptr<RearSightProcessor> _rearSightProcessor;
    bool _isActiveCameraThread;
    std::thread _cameraStreamingThread;
    double _focalDelta;
    FireCorrection _savedFireCorrection;

    bool _isSwappedStreams;
    bool _isSetFuse;
    bool _isShowDebugMessages;

    bool TRACKING_FLAG;

    std::shared_ptr<Tracker> _tracker;
    cv::Rect2d _bb;

    std::list<cv::Mat> _matsQueue;
    std::vector<double> _focalPointsVector = {0.1, 0.2, 0.3, 0.4, 0.5};
    std::shared_ptr<BallisticCalculator> _ballisticCalculator;
    DelegateWS *_delegate;

    CameraThread();                             // private constructor
    CameraThread(const CameraThread&);
    CameraThread&operator= (CameraThread&);     // announced using of '=' operator
    ~CameraThread();

    void processCameraThread();
    void recalculateFocalPoints(double cropping);

public:
    void startTracking(int x, int y);
    static CameraThread* getCameraThreadInstance();

    void startCameraStream();
    void stopCameraStream();

    void joinCameraThread();

    bool isHasFramesInQueue();
    void addMat(cv::Mat newFrame);
    cv::Mat getLastMat();

    void drawOnMat(cv::Mat &matForDrawing);

    void moveUP();
    void moveDOWN();
    void moveLEFT();
    void moveRIGHT();

    void zoomPlus();
    void zoomMinus();
    double getCroppingPercent();

    int getX();
    int getY();
    int getWidth();
    int getHeight();

   void startRegistrationFire();
   void stopRegistrationFire();
   bool isRegistrationFire();

   void swapStreams();
   bool isSwappedStreams();

   void planeDotsPlusDistance();
   void planeDotsMinusDistance();

   void removeFuse();
   void installFuse();

   void setNextFireCorrection();
   void setPreviousFireCorrection();
   std::string getFireCorrectionsLineJSON();
   int getFireCorrectionIndex();
   void setFireCorrectionIndex(int index);
   void setFireCorrections(double dx, double dy);

   void setShowingDebugMessage();

   void setDefaultFireCorrectionTable();
   void setDefaultFireCorrectionRecord(int index);
};


#endif //EYE_CAMERATHREAD_H
