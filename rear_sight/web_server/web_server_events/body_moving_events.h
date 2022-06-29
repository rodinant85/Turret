//
// Created by biba_bo on 2020-12-15.
//

#ifndef DRIVEN_IRON_TURTLE_BODY_MOVING_EVENTS_H
#define DRIVEN_IRON_TURTLE_BODY_MOVING_EVENTS_H


constexpr int PUSHED_UP     = 0x10;
/// this code for pushed button 'DOWN' - which means move the iron turtle in the forward direction
constexpr int PUSHED_DOWN   = 0x8;
/// this code for pushed button 'LEFT' - which means move the iron turtle in the forward direction
constexpr int PUSHED_LEFT   = 0x4;
/// this code for pushed button 'RIGHT' - which means move the iron turtle in the forward direction
constexpr int PUSHED_RIGHT  = 0x2;
/// this code for pushed button '0' - which means move the iron turtle in the forward direction
constexpr int PUSHED_0      = 0x1;

/// My own event code (this means a client is wanting to move the iron turtle in the FORWARD direction faster).
constexpr int EVENT_MOVE_FORWARD         = 1;
/// My own event code (this means a client is wanting to move the iron turtle in the BACKWARD direction faster).
constexpr int EVENT_MOVE_BACK            = 2;
/// My own event code (this means a client is wanting to move the iron turtle in the RIGHT direction).
constexpr int EVENT_MOVE_RIGHTER         = 3;
/// My own event code (this means a client is wanting to move the iron turtle in the LEFT direction).
constexpr int EVENT_MOVE_LEFTER          = 4;

/// My own event code (this means a client is wanting to move the iron turtle in the FORWARD AND LEFT).
constexpr int EVENT_MOVE_FORWARD_LEFT    = 5;
/// My own event code (this means a client is wanting to move the iron turtle in the FORWARD AND RIGHT).
constexpr int EVENT_MOVE_FORWARD_RIGHT   = 6;
/// My own event code (this means a client is wanting to move the iron turtle in the BACK AND LEFT).
constexpr int EVENT_MOVE_BACK_LEFT       = 7;
/// My own event code (this means a client is wanting to move the iron turtle in the BACK AND RIGHT).
constexpr int EVENT_MOVE_BACK_RIGHT      = 8;

/// My own event code (this means a client is wanting to STOP the iron turtle moving).
constexpr int EVENT_STOP_MOVING          = 9;

/// This constants contains a maximum code for the moving process
constexpr int MAX_CODE_FOR_MOVE_EVENTS   = 20;




/// smart pointer to my own event "moving the iron turtle forward"
static std::shared_ptr<EventWS> eventMoveForward = std::make_shared<EventWS>(EVENT_MOVE_FORWARD);
/// smart pointer to my own event "moving the iron turtle backward"
static std::shared_ptr<EventWS> eventMoveBack = std::make_shared<EventWS>(EVENT_MOVE_BACK);
/// smart pointer to my own event "moving the iron turtle righter"
static std::shared_ptr<EventWS> eventMoveRighter = std::make_shared<EventWS>(EVENT_MOVE_RIGHTER);
/// smart pointer to my own event "moving the iron turtle lefter"
static std::shared_ptr<EventWS> eventMoveLefter = std::make_shared<EventWS>(EVENT_MOVE_LEFTER);
/// smart pointer to my own event "stopping the iron turtle moving"
static std::shared_ptr<EventWS> eventStopMoving = std::make_shared<EventWS>(EVENT_STOP_MOVING);

/// smart pointer to my own event "moving the iron turtle forward snd left"
static std::shared_ptr<EventWS> eventMoveForwardLeft = std::make_shared<EventWS>(EVENT_MOVE_FORWARD_LEFT);
/// smart pointer to my own event "moving the iron turtle forward and right"
static std::shared_ptr<EventWS> eventMoveForwardRight = std::make_shared<EventWS>(EVENT_MOVE_FORWARD_RIGHT);
/// smart pointer to my own event "moving the iron turtle back and left"
static std::shared_ptr<EventWS> eventMoveBackLeft = std::make_shared<EventWS>(EVENT_MOVE_BACK_LEFT);
/// smart pointer to my own event "moving the iron turtle back and right"
static std::shared_ptr<EventWS> eventMoveBackRight = std::make_shared<EventWS>(EVENT_MOVE_BACK_RIGHT);

#endif //DRIVEN_IRON_TURTLE_BODY_MOVING_EVENTS_H
