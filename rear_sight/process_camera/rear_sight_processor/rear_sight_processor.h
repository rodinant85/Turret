//
// Created by biba_bo on 2020-08-14.
//

#ifndef REAR_SIGHT_CLONE_PROJECT_REAR_SIGHT_PROCESSOR_H
#define REAR_SIGHT_CLONE_PROJECT_REAR_SIGHT_PROCESSOR_H

#define OPERATION_SUCCESSFUL        0
#define MOVE_OP_UNSUCCESS           -1
#define ZOOM_OP_UNSUCCESS           -2

#define MIN_ZOOM_COEFFICIENT        0.1
#define MAX_ZOOM_COEFFICIENT        1.0
#define ZOOMING_STEP_COEFFICIENT    0.005

#include <memory>
#include <iostream>

#include "image_processing.h"
#include "../../web_server/lib_my_event_handler/delegate_ws.h"

class RearSightProcessor {
private:
    std::shared_ptr<FrameParameters> frame_param;
    int m_CROPPED_WIDTH;
    int m_CROPPED_HEIGHT;
    int m_CROPPED_X;
    int m_CROPPED_Y;
    double m_CURRENT_ZOOM_SIZE;

    bool _isFixedROICenter;
    std::pair<int, int> _roiCenterPoint; // first = x, second = y

    std::pair<int, int> _roiCenterPointCorrected; // first = x, second = y
    std::pair<int, int> _roiDistanceCorrection; // first = dx, second = dy
    double _correctedZoomSize;

    DelegateWS *_delegate;

    void recalculateCorrectedRoiCenter();

public:
    RearSightProcessor(std::shared_ptr<FrameParameters> frame_param);

    int on_zoom_plus_processor();
    int on_zoom_minus_processor();

    int on_move_left_processor();
    int on_move_right_processor();
    int on_move_up_processor();
    int on_move_down_processor();

    int set_new_frame_param();

    void enableFixedROICenter();
    void disableFixedROICenter();
    bool isFixedROICenter();
    void setROICenter(int x, int y, double percent);
    std::pair<int, int> getROICenterPoint();

    double getROIPercent();

    double getROIPercentCorrected();
    void setCorrections(int dx, int dy);
};


#endif //REAR_SIGHT_CLONE_PROJECT_REAR_SIGHT_PROCESSOR_H
