//
// Created by biba_bo on 2020-12-15.
//

#ifndef DRIVEN_IRON_TURTLE_CONTROL_MODE_EVENTS_H
#define DRIVEN_IRON_TURTLE_CONTROL_MODE_EVENTS_H

#define COMMAND_REG_FIRE            "REG_FIRE"
#define COMMAND_STOP_REG_FIRE       "N_REG_FIRE"
#define COMMAND_SWAP_STEAMS         "SWP_STRM"

constexpr int EVENT_REG_FIRE        = 41;
constexpr int EVENT_STOP_REG_FIRE   = 42;
constexpr int EVENT_SWAP_STREAMS    = 43;

static std::shared_ptr<EventWS> eventSetModeRegFire = std::make_shared<EventWS>(EVENT_REG_FIRE);
static std::shared_ptr<EventWS> eventStopModeRegFire = std::make_shared<EventWS>(EVENT_STOP_REG_FIRE);
static std::shared_ptr<EventWS> eventSwapStreams = std::make_shared<EventWS>(EVENT_SWAP_STREAMS);

#define COMMAND_PDOT_PLUS           "PDOT_P"
#define COMMAND_PDOT_MINUS          "PDOT_M"
constexpr int EVENT_PDOT_PLUS     = 44;
constexpr int EVENT_PDOT_MINUS    = 45;
static std::shared_ptr<EventWS> eventPDotPlus = std::make_shared<EventWS>(EVENT_PDOT_PLUS);
static std::shared_ptr<EventWS> eventPDotMinus = std::make_shared<EventWS>(EVENT_PDOT_MINUS);

#define COMMAND_FUSE_INSTALL        "FUSE_I"
#define COMMAND_FUSE_REMOVE         "FUSE_R"
constexpr int EVENT_FUSE_INSTALL  = 46;
constexpr int EVENT_FUSE_REMOVE   = 47;
static std::shared_ptr<EventWS> eventFuseInstall = std::make_shared<EventWS>(EVENT_FUSE_INSTALL);
static std::shared_ptr<EventWS> eventFuseRemove = std::make_shared<EventWS>(EVENT_FUSE_REMOVE);

#define COMMAND_GET_FIRE_CORR_ARRAY  "GET_FCA"
#define COMMAND_GET_FIRE_CORR_INDEX  "GET_FCI"
#define COMMAND_SET_FIRE_CORR_INDEX  "SET_FCI"
#define COMMAND_FIX_FIRE_CORR        "FIX_FC"
constexpr int EVENT_GET_FIRE_CORR_ARRAY = 48;
constexpr int EVENT_GET_FIRE_CORR_INDEX = 49;
constexpr int EVENT_SET_FIRE_CORR_INDEX = 50;
constexpr int EVENT_FIX_FIRE_CORR       = 51;
static std::shared_ptr<EventWS> eventGetFireCorrArray = std::make_shared<EventWS>(EVENT_GET_FIRE_CORR_ARRAY);
static std::shared_ptr<EventWS> eventGetFireCorrIndex = std::make_shared<EventWS>(EVENT_GET_FIRE_CORR_INDEX);
static std::shared_ptr<EventWS> eventSetFireCorrIndex = std::make_shared<EventWS>(EVENT_SET_FIRE_CORR_INDEX);

#define COMMAND_SET_DEFAULT_FIRE_CORR_BY_INDEX    "CLR_TBL_REC"
#define COMMAND_SET_DEFAULT_FULL_FIRE_CORR_TBL    "CLR_FULL_TBL"
constexpr int EVENT_SET_DEFAULT_FIRE_CORR_BY_INDEX = 52;
constexpr int EVENT_SET_DEFAULT_FULL_FIRE_CORR_TBL = 53;

constexpr int TRACKER_SET_COORDS = 80;

static std::shared_ptr<EventWS> eventSetDefaultFullFireCorrTable =
        std::make_shared<EventWS>(EVENT_SET_DEFAULT_FULL_FIRE_CORR_TBL);
static std::shared_ptr<EventWS> eventTrackerInit = std::make_shared<EventWS>(TRACKER_SET_COORDS);

/// This constants contains a maximum code for possible control modes
constexpr int MAX_CODE_FOR_CONTROL_MODES   = 70;

#endif //DRIVEN_IRON_TURTLE_CONTROL_MODE_EVENTS_H
