//
// Created by biba_bo on 10/12/21.
//

#ifndef MACHINE_GUN_REAR_SIGHT_HOLDER_COMMANDS_H
#define MACHINE_GUN_REAR_SIGHT_HOLDER_COMMANDS_H

#define COMMAND_MOVE_HOLDER_IN_POS "MV_HLD"
constexpr int EVENT_MOVE_HOLDER_IN_POS = 71;

#define COMMAND_HOLDER_FUSE_UP     "FUSE_UP"
#define COMMAND_HOLDER_FUSE_DOWN   "FUSE_DOWN"
constexpr int EVENT_HOLDER_FUSE_UP     = 72;
constexpr int EVENT_HOLDER_FUSE_DOWN   = 73;
static std::shared_ptr<EventWS> eventHolderFuseUp = std::make_shared<EventWS>(EVENT_HOLDER_FUSE_UP);
static std::shared_ptr<EventWS> eventHolderFuseDown = std::make_shared<EventWS>(EVENT_HOLDER_FUSE_DOWN);

#define COMMAND_HOLDER_DO_FIRE     "DO_FIRE"
#define COMMAND_HOLDER_DONE_FIRE   "DONE_FIRE"
constexpr int EVENT_HOLDER_DO_FIRE     = 74;
constexpr int EVENT_HOLDER_DONE_FIRE   = 75;

#define COMMAND_CAMERA_DELTA        "CAMERA_DELTA"
constexpr int EVENT_CAMERA_DELTA     = 76;
static std::shared_ptr<EventWS> eventCameraDelta = std::make_shared<EventWS>(EVENT_CAMERA_DELTA);

#define COMMAND_CHANGE_ZOOM        "CHANGE_ZOOM"
constexpr int EVENT_CHANGE_ZOOM     = 77;
static std::shared_ptr<EventWS> eventChangeZoom = std::make_shared<EventWS>(EVENT_CHANGE_ZOOM);

/// This constants contains a maximum code for possible control modes
constexpr int MAX_CODE_FOR_HOLDER_EVENTS   = 100;

#endif //MACHINE_GUN_REAR_SIGHT_HOLDER_COMMANDS_H
