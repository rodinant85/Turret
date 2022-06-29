#include "Tracker.h"

Tracker::Tracker() {
    _tracker = cv::TrackerMedianFlow::create();
}

void Tracker::init(const cv::Mat& frame, const cv::Rect2d& roi) {
    _tracker->init(frame, roi);
    std::cout << "Init" << std::endl;
}

std::pair<bool, std::pair<double, double>> Tracker::update(cv::Mat& frame, cv::Rect2d& bb) {
    bool success = _tracker->update(frame, bb);
    std::cout << "Success: " << success << std::endl;
    if(!success) return std::make_pair(false, std::make_pair(0.0, 0.0));
//    return std::make_pair(true, std::make_pair(0.0, 0.0));
    return std::make_pair(true, get_delta(bb, frame.size().width, frame.size().height));
}

std::pair<double, double> Tracker::get_delta(const cv::Rect2d& bb, double fw, double fh) {
    double x = bb.x, y = bb.y, w = bb.width, h = bb.height;
    if(w > fw || h > fh) throw std::invalid_argument("Incorrect bounding box");
    double dx = 0, dy = 0;

    double fr_center_x = fw / 2;
    double fr_center_y = fh / 2;
    double bb_center_x = x + (w / 2);
    double bb_center_y = y + (h / 2);
    double max_fr = std::max(fw, fh);

    dx = (bb_center_x - fr_center_x)/max_fr;
    dy = (bb_center_y - fr_center_y)/max_fr;
    return std::make_pair(dx, dy);
}
