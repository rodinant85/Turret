//
// Created by biba_bo on 2020-08-18.
//

#include "web_server_worker.h"
#include <cmath>
#include <string.h>


WebServerWorker::WebServerWorker() {
//    pathToWebFiles = "src/server_files";
    start_web_server_processing();
}

WebServerWorker::WebServerWorker(const std::string &pathToWebFiles) {
//    this->pathToWebFiles = pathToWebFiles + "src/server_files";
    start_web_server_processing();
}

void WebServerWorker::start_web_server_processing() {
    //    // creating an object of TurtleManager with connecting throw COM-port
//    turtle_manager = make_shared<SmoothTurtleManager>();
//    _masterTurtleManager = std::make_shared<MasterTurtleManager>();

/// WARNING!!!!! default values sends by WebServer to client web-page script, if you wish change it here, you also
///             should change it in WebServer in function onConnection()
/// END of WARNING!!!!
    // starting web-server
    this->startServer();
    // this thread sleeps for 1 seconds for avoiding bugs and errors
    sleep(1);
    // staring processing the server.
    this->processServer();
}

void WebServerWorker::startServer() {
    // creating a new thread for the web-server
    this->server_thread = std::thread([this]() {
        // creating a new object of PrintfLogger for possibility to log working process
        logger = std::make_shared<PrintfLogger>();
        // creating a new object of MyServer and adding here a logger
        ws_server = make_shared<MyServer>(logger);
        // creating a new object of MyHandler for handling events from web-client (onConnect, onDisconnect, onMessage)
        handler = std::make_shared<MyHandler>(ws_server.get());
        // adding a page handler for working with web-pages
        ws_server->addPageHandler(std::make_shared<MyAuthHandler>());
        // added a socket handler
        ws_server->addWebSocketHandler("/chart", handler);
        // starting webserver
//        ws_server->serve(pathToWebFiles.c_str(), 56778);
        ws_server->serve("src/server_files", 56778);
    });

    // starting a Holder Controller
    _holderController = std::make_shared<HolderController>();
}

void WebServerWorker::processServer() {
    // still nothing works here
}

void WebServerWorker::joinServerTread() {
    // like a feature for avoiding bugs
    if (server_thread.joinable())
        server_thread.join();
}

void WebServerWorker::handleEventWS(std::shared_ptr<EventWS> event) {
    // handling events by the event code
    if (event->getEventID() == TRACKER_SET_COORDS) {
        startTracking(event->getX(), event->getY());
    } else if (event->getEventID() < MAX_CODE_FOR_MOVE_EVENTS) {
        handleBodyMovingEvents(event->getEventID());
    } else if (event->getEventID() < MAX_CODE_FOR_CAM_ROI) {
        handleCamaraROIEvents(event->getEventID());
    } else if (event->getEventID() < MAX_CODE_FOR_CONTROL_MODES) {
        handleControlModeEvents(event);
    } else if (event->getEventID() < MAX_CODE_FOR_HOLDER_EVENTS) {
        handleHolderEvents(event);
    } else {
        std::string jsonString = "";

        switch (event->getEventID()) {
            case EVENT_CLIENT_CONNECTED:
                jsonString += "{";
                if (isRegistrationFire())
                    jsonString += "REG_FIRE";
                else
                    jsonString += "N_REG_FIRE";
                if (isStreamsSwapped())
                    jsonString += ", SWP_STRM";
                else
                    jsonString += ", N_SWP_STRM";
                jsonString += "}";
                handler->sendValuesJSON(jsonString.data());

                // send message with correction table information
                // set timeout before sending, that give time for creating CameraThread object instance
                g_usleep(2000);
                jsonString = getFireCorrectionsLineJSON();
                handler->sendValuesJSON(jsonString.data());
                std::cout << "\n\n\n" << jsonString << "\n\n\n";
                break;
            case EVENT_CLIENT_DISCONNECTED:
//            turtle_manager->say_server_leave();
//                _masterTurtleManager->command_server_leave();
                break;
        }
    }
}



void WebServerWorker::handleBodyMovingEvents(int eventCode) {
    switch (eventCode) {
        case EVENT_MOVE_FORWARD:
            setNextFireCorrection();
            break;
        case EVENT_MOVE_BACK:
            setPreviousFireCorrection();
            break;
        case EVENT_MOVE_LEFTER:
            planeDotsPlusDistance();
            break;
        case EVENT_MOVE_RIGHTER:
            planeDotsMinusDistance();
            break;
        case EVENT_STOP_MOVING:
            // on pushing '0'-button
            setShowingDebugMessagesInStream();
            break;
        case EVENT_MOVE_FORWARD_LEFT:
            break;
        case EVENT_MOVE_FORWARD_RIGHT:
            break;
        case EVENT_MOVE_BACK_LEFT:
            break;
        case EVENT_MOVE_BACK_RIGHT:
            break;
    }
}

void WebServerWorker::handleCamaraROIEvents(int eventCode) {
    switch (eventCode) {
        case EVENT_CAM_ZM:
            on_zoom_minus_processor();
            break;
        case EVENT_CAM_ZP:
            on_zoom_plus_processor();
            break;
        case EVENT_CAM_UP:
            on_move_up_processor();
            break;
        case EVENT_CAM_DOWN:
            on_move_down_processor();
            break;
        case EVENT_CAM_LEFT:
            on_move_left_processor();
            break;
        case EVENT_CAM_RIGHT:
            on_move_right_processor();
            break;
    }
}

void WebServerWorker::handleControlModeEvents(std::shared_ptr<EventWS> event) {
    switch (event->getEventID()) {
        case TRACKER_SET_COORDS:
            startTracking(event->getX(), event->getY());
            break;
        case EVENT_REG_FIRE:
            onStartRegistrationFire();
            break;
        case EVENT_STOP_REG_FIRE:
            onStopRegistrationFire();
            break;
        case EVENT_SWAP_STREAMS:
            swapStreams();
            break;
        case EVENT_PDOT_PLUS:
            planeDotsPlusDistance();
            break;
        case EVENT_PDOT_MINUS:
            planeDotsMinusDistance();
            break;
        case EVENT_FUSE_INSTALL:
            installFuse();
            break;
        case EVENT_FUSE_REMOVE:
            removeFuse();
            break;
        case EVENT_GET_FIRE_CORR_ARRAY:
            handler->sendValuesJSON(getFireCorrectionsLineJSON().data());
            break;
        case EVENT_GET_FIRE_CORR_INDEX: {
            std::string message = "{\"FCI\":" + to_string(getFireCorrectionIndex()) + "}";
            handler->sendValuesJSON(message.data());
        }
            break;
        case EVENT_SET_FIRE_CORR_INDEX:
            setFireCorrectionIndex(event->getData());
            break;
        case EVENT_FIX_FIRE_CORR:
            setFireCorrection(event->getDataFirst(), event->getDataSecond());
            break;
        case EVENT_SET_DEFAULT_FIRE_CORR_BY_INDEX:
            setDefaultFireCorrectionRecord(event->getData());
            break;
        case EVENT_SET_DEFAULT_FULL_FIRE_CORR_TBL:
            setDefaultFireCorrectionTable();
            break;
    }
}

void WebServerWorker::handleHolderEvents(std::shared_ptr<EventWS> eventWs) {
    double zooming = get_cropping_percent();
    double xSpeedPer = 0.0;
    double ySpeedPer = 0.0;
    uint8_t direction = 0x00;
    std::string message = "{\"comm\":[\"";

    switch (eventWs->getEventID()) {
        case EVENT_MOVE_HOLDER_IN_POS:
            xSpeedPer = eventWs->getDataFirst();
            ySpeedPer = eventWs->getDataSecond() * 3.0 / 4.0;
            xSpeedPer *= zooming;
            ySpeedPer *= zooming;
            xSpeedPer *= (double) HolderMovementConst::X_MAX_SPEED;
            ySpeedPer *= (double) HolderMovementConst::Y_MAX_SPEED;
            if (ySpeedPer > 0) {
                if (xSpeedPer > 0)
                    direction = HolderMovementDirection::UP_RIGHT;
                else
                    direction = HolderMovementDirection::UP_LEFT;
            } else {
                if (xSpeedPer > 0)
                    direction = HolderMovementDirection::DOWN_RIGHT;
                else
                    direction = HolderMovementDirection::DOWN_LEFT;
            }
            _holderController->setMoveSpeed((uint8_t) fabs(xSpeedPer),
                                            (uint8_t) fabs(ySpeedPer),
                                            direction);
            break;
        case EVENT_HOLDER_FUSE_UP:
            std::cout << "# HOLDER # -> FUSE IS UP\n";

            // send to holder message to FUSE UP
            // but it actually doesn't exist, so we send to web-client message that FUSE UP
            message += COMMAND_HOLDER_FUSE_UP;
            message += "\"]}";
            handler->sendValuesJSON(message);
            break;
        case EVENT_HOLDER_FUSE_DOWN:
            std::cout << "# HOLDER # -> FUSE IS DOWN\n";

            // send to holder message to FUSE DOWN
            // but it actually doesn't exist, so we send to web-client message that FUSE DOWN
            message += COMMAND_HOLDER_FUSE_DOWN;
            message += "\"]}";
            handler->sendValuesJSON(message);
            break;
        case EVENT_HOLDER_DO_FIRE:
            std::cout << "# HOLDER # -> FIRE FOR " << eventWs->getData() << "\n";

            // send to holder message to FIRE ON
            // but it actually doesn't exist, so we send to web-client message that FIRE DONE
            message += COMMAND_HOLDER_DONE_FIRE;
            message += "\"]}";
            handler->sendValuesJSON(message);

            _holderController->fireOn((uint8_t) eventWs->getData());

            break;
        case EVENT_CAMERA_DELTA: {
            std::cout << "# CAMERA CORRECT " << "X:" << eventWs->getDataFirst() << "Y:" << eventWs->getDataSecond()
                      << "D\n";
            std::string s_dx = std::to_string(eventWs->getDataFirst());
            std::string s_dy = std::to_string(eventWs->getDataSecond());
            message = "{\"tracker_target\":{\"t_dx\":" + s_dx + ",\"t_dy\":" + s_dy + "}}";
            handler->sendValuesJSON(message);

            _holderController->fireOn((uint8_t) eventWs->getData());
        }
            break;

        case EVENT_CHANGE_ZOOM: {

            std::string z = std::to_string(round(eventWs->getData() * 1000) / 1000);
            char round_z[5];
            strncpy(round_z, &z[0], 5);
            std::string w = std::to_string(eventWs->getWidth());
            std::string h = std::to_string(eventWs->getHeight());
            std::cout << " ZOOM: " << round_z << "W: " << w << "H: " << h << "\n";
            message = "{\"sizes\":{\"full_width\":" + w + ",\"full_height\":" + h + ",\"zoom\":" + round_z + "}}";
            handler->sendValuesJSON(message);
    }
            break;
    }
}
