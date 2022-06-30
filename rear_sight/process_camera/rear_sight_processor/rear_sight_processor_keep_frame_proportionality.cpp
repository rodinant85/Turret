//
// Created by biba_bo on 2020-08-17.
//

#include "rear_sight_processor.h"
#include "image_processing.h"
#include "../../web_server/web_server_events/holder_commands.h"
#include <fstream>
#include <cmath>
#include <string.h>

void saveSizes(int width, int height, double zoom){
    std::string z = std::to_string(zoom);
    std::string w = std::to_string(width);
    std::string h = std::to_string(height);
    std::ofstream myfile;
    myfile.open ("src/sizes.conf");
    std::string message;
    message = "{\"sizes\":{\"full_width\":" +
              w + ",\"full_height\":" +
              h + ",\"zoom\":" + z + "}}";
    myfile << message;
    myfile.close();
}


RearSightProcessor::RearSightProcessor(std::shared_ptr<FrameParameters> frame_param) {
    this->frame_param = frame_param;
    m_CROPPED_WIDTH = frame_param->CROPPED_WIDTH;
    m_CROPPED_HEIGHT = frame_param->CROPPED_HEIGHT;
    m_CROPPED_X = frame_param->CROPPED_X;
    m_CROPPED_Y = frame_param->CROPPED_Y;
    m_CURRENT_ZOOM_SIZE = ( ((double)m_CROPPED_HEIGHT / (double)HEIGHT) + ((double)m_CROPPED_WIDTH / (double)WIDTH) ) / 2;

    _isFixedROICenter = false;
    _roiCenterPoint.first = m_CROPPED_X + (m_CROPPED_WIDTH / 2);
    _roiCenterPoint.second = m_CROPPED_Y + (m_CROPPED_HEIGHT / 2);

    _roiCenterPointCorrected.first = _roiCenterPoint.first;
    _roiCenterPointCorrected.second = _roiCenterPoint.second;
    _roiDistanceCorrection.first = 0;
    _roiDistanceCorrection.second = 0;
    _correctedZoomSize = m_CURRENT_ZOOM_SIZE;
}


int RearSightProcessor::on_zoom_plus_processor() {
    _delegate = DelegateWS::getInstance();
    if (m_CURRENT_ZOOM_SIZE == MIN_ZOOM_COEFFICIENT)
        return ZOOM_OP_UNSUCCESS;
    if (m_CURRENT_ZOOM_SIZE - ZOOMING_STEP_COEFFICIENT < MIN_ZOOM_COEFFICIENT)
        m_CURRENT_ZOOM_SIZE = MIN_ZOOM_COEFFICIENT;
    else
        m_CURRENT_ZOOM_SIZE -= ZOOMING_STEP_COEFFICIENT;
    int n_CROPPED_WIDTH = m_CURRENT_ZOOM_SIZE * WIDTH;
    int n_CROPPED_HEIGHT = m_CURRENT_ZOOM_SIZE * HEIGHT;
    m_CROPPED_X += (m_CROPPED_WIDTH -  n_CROPPED_WIDTH) / 2;
    m_CROPPED_Y += (m_CROPPED_HEIGHT - n_CROPPED_HEIGHT) / 2;
    m_CROPPED_WIDTH = n_CROPPED_WIDTH;
    m_CROPPED_HEIGHT = n_CROPPED_HEIGHT;

//    std::cout << "*******************************************************\n";
//    std::cout << "+ CROPP             x=" << m_CROPPED_X << " y=" << m_CROPPED_Y << " w=" << m_CROPPED_WIDTH << " h=" << m_CROPPED_HEIGHT << "\n";
//    std::cout << "+ Center            x=" << _roiCenterPoint.first << " y=" << _roiCenterPoint.second << "\n";
//    std::cout << "+ actual ROI center x=" << m_CROPPED_X + (m_CROPPED_WIDTH / 2) << " y=" << m_CROPPED_Y + (m_CROPPED_HEIGHT / 2) << "\n";
    // fixing zooming center
    if (m_CROPPED_X + (m_CROPPED_WIDTH / 2) < _roiCenterPoint.first)
        m_CROPPED_X += _roiCenterPoint.first - (m_CROPPED_X + (m_CROPPED_WIDTH / 2));
    if (m_CROPPED_X + (m_CROPPED_WIDTH / 2) > _roiCenterPoint.first)
        m_CROPPED_X -= (m_CROPPED_X + (m_CROPPED_WIDTH / 2)) - _roiCenterPoint.first;

    if (m_CROPPED_Y + (m_CROPPED_HEIGHT / 2) < _roiCenterPoint.second)
        m_CROPPED_Y += _roiCenterPoint.second - (m_CROPPED_Y + (m_CROPPED_HEIGHT / 2));
    if (m_CROPPED_Y + (m_CROPPED_HEIGHT / 2) > _roiCenterPoint.second)
        m_CROPPED_Y -= (m_CROPPED_Y + (m_CROPPED_HEIGHT / 2)) - _roiCenterPoint.second;
//    std::cout << "+ fixed ROI center  x=" << m_CROPPED_X + (m_CROPPED_WIDTH / 2) << " y=" << m_CROPPED_Y + (m_CROPPED_HEIGHT / 2) << "\n";

    if (_isFixedROICenter)
        recalculateCorrectedRoiCenter();
    saveSizes(WIDTH_STREAM_FRAME, HEIGHT_STREAM_FRAME, m_CURRENT_ZOOM_SIZE);
    _delegate->doEvent(std::make_shared<EventWS>(EVENT_CHANGE_ZOOM, m_CURRENT_ZOOM_SIZE, WIDTH_STREAM_FRAME, HEIGHT_STREAM_FRAME));

    return OPERATION_SUCCESSFUL;
}

int RearSightProcessor::on_zoom_minus_processor() {
    _delegate = DelegateWS::getInstance();
//    std::cout << "//////////////////////////////////////////////////////////\n";
//    std::cout << "- CROPP             x=" << m_CROPPED_X << " y=" << m_CROPPED_Y << " w=" << m_CROPPED_WIDTH << " h=" << m_CROPPED_HEIGHT << "\n";
//    std::cout << "- Center            x=" << _roiCenterPoint.first << " y=" << _roiCenterPoint.second << "\n";
//    std::cout << "- actual ROI center x=" << m_CROPPED_X + (m_CROPPED_WIDTH / 2) << " y=" << m_CROPPED_Y + (m_CROPPED_HEIGHT / 2) << "\n";
    if (m_CURRENT_ZOOM_SIZE == MAX_ZOOM_COEFFICIENT)
        return ZOOM_OP_UNSUCCESS;
    if (m_CURRENT_ZOOM_SIZE + ZOOMING_STEP_COEFFICIENT > MAX_ZOOM_COEFFICIENT) {
        if (_isFixedROICenter) {
            int nRoiCenterX = WIDTH / 2;
            int nRoiCenterY = HEIGHT / 2;
            if (nRoiCenterX != _roiCenterPoint.first || nRoiCenterY != _roiCenterPoint.second)
                return ZOOM_OP_UNSUCCESS;
        }
        m_CURRENT_ZOOM_SIZE = MAX_ZOOM_COEFFICIENT;
        m_CROPPED_X = 0;
        m_CROPPED_Y = 0;
        m_CROPPED_WIDTH = WIDTH;
        m_CROPPED_HEIGHT = HEIGHT;
    } else {
        double n_CURRENT_ZOOM_SIZE = m_CURRENT_ZOOM_SIZE + ZOOMING_STEP_COEFFICIENT;
        int n_CROPPED_WIDTH = n_CURRENT_ZOOM_SIZE * WIDTH;
        int n_CROPPED_HEIGHT = n_CURRENT_ZOOM_SIZE * HEIGHT;
        int diff_WIDTH = n_CROPPED_WIDTH - m_CROPPED_WIDTH;
        int diff_HEIGHT = n_CROPPED_HEIGHT - m_CROPPED_HEIGHT;

        // SETTING NEW ROIs PARAMETERS FOR X_AXIS
        // means on left side
        if (m_CROPPED_X == 0) {
            if (_isFixedROICenter)
                return ZOOM_OP_UNSUCCESS;
            m_CROPPED_WIDTH = n_CROPPED_WIDTH;
            _roiCenterPoint.first = m_CROPPED_X + (m_CROPPED_WIDTH / 2);
        // means on right side
        } else if (m_CROPPED_X + m_CROPPED_WIDTH == WIDTH) {
            if (_isFixedROICenter)
                return ZOOM_OP_UNSUCCESS;
            m_CROPPED_X -= diff_WIDTH;
            m_CROPPED_WIDTH = n_CROPPED_WIDTH;
            _roiCenterPoint.first = m_CROPPED_X + (m_CROPPED_WIDTH / 2);
        // otherwise
        } else {
            // means over left side
            if (m_CROPPED_X - (diff_WIDTH / 2) < 0) {
                if(_isFixedROICenter)
                    return ZOOM_OP_UNSUCCESS;
                m_CROPPED_X = 0;
                m_CROPPED_WIDTH = n_CROPPED_WIDTH;
                _roiCenterPoint.first = m_CROPPED_X + (m_CROPPED_WIDTH / 2);
            // means over right side
            } else if (m_CROPPED_X + n_CROPPED_WIDTH > WIDTH) {
                if (_isFixedROICenter)
                    return ZOOM_OP_UNSUCCESS;
                m_CROPPED_X = WIDTH - n_CROPPED_WIDTH;
                m_CROPPED_WIDTH = n_CROPPED_WIDTH;
                _roiCenterPoint.first = m_CROPPED_X + (m_CROPPED_WIDTH / 2);
           // otherwise, means between left and right side
            } else {
                m_CROPPED_X -= (diff_WIDTH / 2);
                m_CROPPED_WIDTH = n_CROPPED_WIDTH;

                // check and fix ROIs center
                if (m_CROPPED_X + (m_CROPPED_WIDTH / 2) < _roiCenterPoint.first)
                    m_CROPPED_X += _roiCenterPoint.first - (m_CROPPED_X + (m_CROPPED_WIDTH / 2));
                if (m_CROPPED_X + (m_CROPPED_WIDTH / 2) > _roiCenterPoint.first)
                    m_CROPPED_X -= (m_CROPPED_X + (m_CROPPED_WIDTH / 2)) - _roiCenterPoint.first;
            }
        }

        // SETTING NEW ROIs PARAMETERS FOR X_AXIS
        // means on top side
        if (m_CROPPED_Y == 0) {
            if (_isFixedROICenter)
                return ZOOM_OP_UNSUCCESS;
            m_CROPPED_HEIGHT = n_CROPPED_HEIGHT;
            _roiCenterPoint.second = m_CROPPED_Y + (m_CROPPED_HEIGHT / 2);
        // means on bottom side
        } else if (m_CROPPED_Y + m_CROPPED_HEIGHT == HEIGHT) {
            if (_isFixedROICenter)
                return ZOOM_OP_UNSUCCESS;
            m_CROPPED_Y -= diff_HEIGHT;
            m_CROPPED_HEIGHT = n_CROPPED_HEIGHT;
            _roiCenterPoint.second = m_CROPPED_Y + (m_CROPPED_HEIGHT / 2);
        // other_wise
        } else {
            // means over top side
            if (m_CROPPED_Y - (diff_HEIGHT / 2) < 0) {
                if (_isFixedROICenter)
                    return ZOOM_OP_UNSUCCESS;
                m_CROPPED_Y = 0;
                m_CROPPED_HEIGHT = n_CROPPED_HEIGHT;
                _roiCenterPoint.second = m_CROPPED_Y + (m_CROPPED_HEIGHT / 2);
            // means over bottom side
            } else if (m_CROPPED_Y + n_CROPPED_HEIGHT > HEIGHT) {
                if (_isFixedROICenter)
                    return ZOOM_OP_UNSUCCESS;
                m_CROPPED_Y = HEIGHT - n_CROPPED_HEIGHT;
                m_CROPPED_HEIGHT = n_CROPPED_HEIGHT;
                _roiCenterPoint.second = m_CROPPED_Y + (m_CROPPED_HEIGHT / 2);
            // means between top and bottom side
            } else {
                m_CROPPED_Y -= (diff_HEIGHT / 2);
                m_CROPPED_HEIGHT = n_CROPPED_HEIGHT;

                // check and fix ROIs center
                if (m_CROPPED_Y + (m_CROPPED_HEIGHT / 2) < _roiCenterPoint.second)
                    m_CROPPED_Y += _roiCenterPoint.second - (m_CROPPED_Y + (m_CROPPED_HEIGHT / 2));
                if (m_CROPPED_Y + (m_CROPPED_HEIGHT / 2) > _roiCenterPoint.second)
                    m_CROPPED_Y -= (m_CROPPED_Y + (m_CROPPED_HEIGHT / 2)) - _roiCenterPoint.second;
            }
        }
        m_CURRENT_ZOOM_SIZE = n_CURRENT_ZOOM_SIZE;
    }
//    std::cout << "- fixed ROI center  x=" << m_CROPPED_X + (m_CROPPED_WIDTH / 2) << " y=" << m_CROPPED_Y + (m_CROPPED_HEIGHT / 2) << "\n";
    if (_isFixedROICenter)
        recalculateCorrectedRoiCenter();

    saveSizes(WIDTH_STREAM_FRAME, HEIGHT_STREAM_FRAME, m_CURRENT_ZOOM_SIZE);
    _delegate->doEvent(std::make_shared<EventWS>(EVENT_CHANGE_ZOOM, m_CURRENT_ZOOM_SIZE, WIDTH_STREAM_FRAME, HEIGHT_STREAM_FRAME));

    return OPERATION_SUCCESSFUL;
}

int RearSightProcessor::on_move_left_processor() {
    if (_isFixedROICenter || (m_CROPPED_X - MOVE_STEP < 0))
        return MOVE_OP_UNSUCCESS;
    m_CROPPED_X = m_CROPPED_X - MOVE_STEP;
    _roiCenterPoint.first = _roiCenterPoint.first - MOVE_STEP;
    return OPERATION_SUCCESSFUL;
}
int RearSightProcessor::on_move_right_processor() {
    if (_isFixedROICenter || (m_CROPPED_X + m_CROPPED_WIDTH + MOVE_STEP > WIDTH))
        return MOVE_OP_UNSUCCESS;
    m_CROPPED_X = m_CROPPED_X + MOVE_STEP;
    _roiCenterPoint.first = _roiCenterPoint.first + MOVE_STEP;
    return OPERATION_SUCCESSFUL;
}

int RearSightProcessor::on_move_up_processor() {
    if (_isFixedROICenter || (m_CROPPED_Y - MOVE_STEP < 0))
        return MOVE_OP_UNSUCCESS;
    m_CROPPED_Y = m_CROPPED_Y - MOVE_STEP;
    _roiCenterPoint.second = _roiCenterPoint.second - MOVE_STEP;
    return OPERATION_SUCCESSFUL;
}

int RearSightProcessor::on_move_down_processor() {
    if (_isFixedROICenter || (m_CROPPED_Y + m_CROPPED_HEIGHT + MOVE_STEP > HEIGHT))
        return MOVE_OP_UNSUCCESS;
    m_CROPPED_Y = m_CROPPED_Y + MOVE_STEP;
    _roiCenterPoint.second = _roiCenterPoint.second + MOVE_STEP;
    return OPERATION_SUCCESSFUL;
}

int RearSightProcessor::set_new_frame_param() {
    if (_isFixedROICenter) {
        int croppedWidth = (int) ((double) WIDTH * _correctedZoomSize);
        int croppedHeight = (int) ((double) HEIGHT * _correctedZoomSize);

        frame_param->CROPPED_WIDTH = croppedWidth;
        frame_param->CROPPED_HEIGHT = croppedHeight;
        frame_param->CROPPED_X = _roiCenterPointCorrected.first - croppedWidth / 2;
        frame_param->CROPPED_Y = _roiCenterPointCorrected.second - croppedHeight / 2;
    } else {
        frame_param->CROPPED_WIDTH = m_CROPPED_WIDTH;
        frame_param->CROPPED_HEIGHT = m_CROPPED_HEIGHT;
        frame_param->CROPPED_X = m_CROPPED_X;
        frame_param->CROPPED_Y = m_CROPPED_Y;
    }
    return OPERATION_SUCCESSFUL;
}

void RearSightProcessor::enableFixedROICenter() {
    _isFixedROICenter = true;

    recalculateCorrectedRoiCenter();
}

void RearSightProcessor::disableFixedROICenter() {
    _isFixedROICenter = false;
}

bool RearSightProcessor::isFixedROICenter() {
    return _isFixedROICenter;
}

void RearSightProcessor::setROICenter(int x, int y, double percent) {
    m_CROPPED_WIDTH = (int) (percent * (double) WIDTH);
    m_CROPPED_HEIGHT = (int) (percent * (double) HEIGHT);
    m_CROPPED_X = x - (m_CROPPED_WIDTH / 2);
    m_CROPPED_Y = y - (m_CROPPED_HEIGHT / 2);

    _roiCenterPoint.first = x;
    _roiCenterPoint.second = y;

    m_CURRENT_ZOOM_SIZE = percent;
    _isFixedROICenter = true;
}

std::pair<int, int> RearSightProcessor::getROICenterPoint() {
    return _roiCenterPoint;
}

double RearSightProcessor::getROIPercent() {
    return m_CURRENT_ZOOM_SIZE;
}

double RearSightProcessor::getROIPercentCorrected() {
    return _correctedZoomSize;
}

void RearSightProcessor::setCorrections(int dx, int dy) {
    _roiDistanceCorrection.first = dx;
    _roiDistanceCorrection.second = dy;

    if (_isFixedROICenter)
        recalculateCorrectedRoiCenter();
}

void RearSightProcessor::recalculateCorrectedRoiCenter() {
    std::pair<int, int> roiCenterPointCorrected;
    _delegate = DelegateWS::getInstance();
    roiCenterPointCorrected.first = _roiCenterPoint.first - _roiDistanceCorrection.first;
    roiCenterPointCorrected.second = _roiCenterPoint.second - _roiDistanceCorrection.second;

    int widthROI = (int) ((double) WIDTH * m_CURRENT_ZOOM_SIZE);
    int heightROI = (int) ((double) HEIGHT * m_CURRENT_ZOOM_SIZE);

    // check is ok with current size
    if ((roiCenterPointCorrected.first + widthROI / 2 <= WIDTH)
        && (roiCenterPointCorrected.first - widthROI / 2 >= 0)
        && (roiCenterPointCorrected.second + heightROI / 2 <= HEIGHT)
        && (roiCenterPointCorrected.second - heightROI / 2 >= 0)) {
        _roiCenterPointCorrected.first = roiCenterPointCorrected.first;
        _roiCenterPointCorrected.second = roiCenterPointCorrected.second;
        _correctedZoomSize = m_CURRENT_ZOOM_SIZE;
        return;
    }

    // try to resize
    int availableX = widthROI / 2;
    if (roiCenterPointCorrected.first + widthROI / 2 > WIDTH)
        availableX = WIDTH - roiCenterPointCorrected.first;
    else if (roiCenterPointCorrected.first - widthROI / 2 < 0)
        availableX = roiCenterPointCorrected.first;

    int availableY = heightROI / 2;
    if (roiCenterPointCorrected.second + heightROI / 2 > HEIGHT)
        availableY = HEIGHT - roiCenterPointCorrected.second;
    else if (roiCenterPointCorrected.second - heightROI / 2 < 0)
        availableY = roiCenterPointCorrected.second;

    availableX *= 2;
    availableY *= 2;
    double currentPercent = ((double) availableX / (double) WIDTH < (double) availableY / (double) HEIGHT) ?
                            (double) availableX / (double) WIDTH : (double) availableY / (double) HEIGHT;
    std::cout << "\n\n\n Current percent " << currentPercent << "\n\n";

    saveSizes(WIDTH_STREAM_FRAME, HEIGHT_STREAM_FRAME, currentPercent);
    _delegate->doEvent(std::make_shared<EventWS>(EVENT_CHANGE_ZOOM, currentPercent, WIDTH_STREAM_FRAME, HEIGHT_STREAM_FRAME));

    if (currentPercent > MIN_ZOOM_COEFFICIENT) {
        _roiCenterPointCorrected.first = roiCenterPointCorrected.first;
        _roiCenterPointCorrected.second = roiCenterPointCorrected.second;
        _correctedZoomSize = currentPercent;
    }
}
