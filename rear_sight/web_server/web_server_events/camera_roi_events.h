//
// Created by biba_bo on 2020-12-15.
//

#ifndef DRIVEN_IRON_TURTLE_CAMERA_ROI_EVENTS_H
#define DRIVEN_IRON_TURTLE_CAMERA_ROI_EVENTS_H

/// this code for pushed button 'Q' - which means decrease ROI for extracting (zoom PLUS)
constexpr int PUSHED_Q = 0x400;
/// this code for pushed button 'W' - which means move a camera ROI up
constexpr int PUSHED_W = 0x200;
/// this code for pushed button 'E' - which means increase ROI for extracting (zoom MINUS)
constexpr int PUSHED_E = 0x100;
/// this code for pushed button 'A' - which means move a camera ROI lefter
constexpr int PUSHED_A = 0x80;
/// this code for pushed button 'S' - which means move a camera ROI down
constexpr int PUSHED_S = 0x40;
/// this code for pushed button 'D' - which means move a camera ROI righter
constexpr int PUSHED_D = 0x20;
/// this code for pushed button 'UP' - which means move the iron turtle in the forward direction

/// My own event code (this means a client is wanting to decrease ROI's size)
constexpr int EVENT_CAM_ZP = 21;
/// My own event code (this means a client is wanting to increase ROI's size)
constexpr int EVENT_CAM_ZM = 22;
/// My own event code (this means a client is wanting to move the ROI's rectangle UP)
constexpr int EVENT_CAM_UP = 23;
/// My own event code (this means a client is wanting to move the ROI's rectangle DOWN)
constexpr int EVENT_CAM_DOWN = 24;
/// My own event code (this means a client is wanting to move the ROI's rectangle RIGHT)
constexpr int EVENT_CAM_RIGHT = 25;
/// My own event code (this means a client is wanting to move the ROI's rectangle LEFT)
constexpr int EVENT_CAM_LEFT = 26;

/// This constants contains a maximum code for the changing camera ROI
constexpr int MAX_CODE_FOR_CAM_ROI = 40;


/// smart pointer to my own event "decreasing the ROI's rectangle size"
static std::shared_ptr<EventWS> eventCamZP = std::make_shared<EventWS>(EVENT_CAM_ZP);
/// smart pointer to my own event "increasing the ROI's rectangle size"
static std::shared_ptr<EventWS> eventCamZM = std::make_shared<EventWS>(EVENT_CAM_ZM);
/// smart pointer to my own event "moving the ROI's rectangle up"
static std::shared_ptr<EventWS> eventCamUp = std::make_shared<EventWS>(EVENT_CAM_UP);
/// smart pointer to my own event "moving the ROI's rectangle down"
static std::shared_ptr<EventWS> eventCamDown = std::make_shared<EventWS>(EVENT_CAM_DOWN);
/// smart pointer to my own event "moving the ROI's rectangle left"
static std::shared_ptr<EventWS> eventCamLeft = std::make_shared<EventWS>(EVENT_CAM_LEFT);
/// smart pointer to my own event "moving the ROI's rectangle right"
static std::shared_ptr<EventWS> eventCamRight = std::make_shared<EventWS>(EVENT_CAM_RIGHT);

#endif //DRIVEN_IRON_TURTLE_CAMERA_ROI_EVENTS_H
