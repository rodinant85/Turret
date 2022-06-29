#include <opencv2/tracking/tracking.hpp>
#include <memory>

class Tracker {
    public:
    Tracker();
    void init(const cv::Mat& frame, const cv::Rect2d& roi);

    std::pair<bool, std::pair<double, double>> update(cv::Mat& frame, cv::Rect2d& bb);

    private:
    static std::pair<double, double> get_delta(const cv::Rect2d& bb, double fw, double fh);

    private:
    cv::Ptr<cv::TrackerMedianFlow> _tracker;
};
