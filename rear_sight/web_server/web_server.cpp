//
// Created by biba_bo on 2020-08-18.
//


#include "web_server.h"

MyHandler::MyHandler(MyServer *server) : _server(server) {
    this->_delegate = DelegateWS::getInstance();
    this->eventClientConnected = std::make_shared<EventWS>(EVENT_CLIENT_CONNECTED);
    this->eventClientDisconnected = std::make_shared<EventWS>(EVENT_CLIENT_DISCONNECTED);

    _isFirstWebRTC_Connection = true;
}

void MyHandler::onConnect(WebSocket *connection) {
    if (_connections.size() == 0) {
        _connections.insert(connection);
        if (_isFirstWebRTC_Connection) {
            _rearSightThread = std::thread([this, connection]() {
                init_rear_sight_processor();
                webrtc_gst_loop(connection);
            });
            _isFirstWebRTC_Connection = false;
        } else {
            webrtc_pipeline_restart(connection);
        }
        _delegate->doEvent(eventClientConnected);
    } else {
        connection->send(MESSAGE_FOR_EXCESS_CLIENT);
        connection->close();
    }

    // enable the fuse for each connections.
    installFuse();
}

void MyHandler::onData(WebSocket *connection, const char *data) {
    if (strstr(data, "coords:")) {
        doEventHandling(data);
        return;
    }
    if (strstr(data, "bt:")) {
        std::string line = data;
        int pushedButtons = std::stoi(line.substr(3, line.size() - 1), nullptr, 2);
        doEventHandling(pushedButtons);
        return;
    }
    if (strstr(data, "comm")) {
        JsonParser *jsonParser = json_parser_new();
        JsonNode *jsonRoot;
        JsonObject *jsonRootObject;
        JsonArray *jsonCommArray;
        if (json_parser_load_from_data(jsonParser, data, -1, NULL)) {
            jsonRoot = json_parser_get_root(jsonParser);
            if (JSON_NODE_HOLDS_OBJECT(jsonRoot)) {
                jsonRootObject = json_node_get_object(jsonRoot);
                jsonCommArray = json_object_get_array_member(jsonRootObject, "comm");
                for (int inx = 0; inx < json_array_get_length(jsonCommArray); inx++) {
                    doEventHandling(json_array_get_string_element(jsonCommArray, inx));
                }
            }
        }
        g_object_unref(jsonParser);
        return;
    }

    webrtc_session_handle(data);
}

void MyHandler::onDisconnect(WebSocket *connection) {
#ifdef UBUNTU_PC
    if (_connections.contains(connection)) {
        _delegate->doEvent(eventClientDisconnected);
    }
#endif //UBUNTU_PC
#ifdef RASPBERRY_PI
    for (WebSocket *c: _connections) {
        if (c == connection) {
            _delegate->doEvent(eventClientDisconnected);
        }
    }
#endif //RASPBERRY_PI

    webrtc_pipeline_deactivate(connection);
    _connections.erase(connection);
}

void MyHandler::sendValuesJSON(std::string values) {
    for (auto c : _connections) {
        c->send(values);
    }
}

void MyHandler::doEventHandling(const char *data) {
    if(strstr(data, "coords:")) {
        std::cout << "data: " << data << "\n";
        std::string c(data);
        c = c.substr(8, c.size()-7);
        int delim_index = c.find(" ");

        int xc = std::stoi(c.substr(0, delim_index));
        int yc = std::stoi(c.substr(delim_index+1, c.size()-delim_index));
        std::cout << "xc: "<< xc << " yc: " << yc << std::endl;
        _delegate->doEvent(std::make_shared<EventWS>(TRACKER_SET_COORDS, xc, yc));
        return;
    }
    if (strlen(data) > 15
        && data[0] == COMMAND_MOVE_HOLDER_IN_POS[0] && data[1] == COMMAND_MOVE_HOLDER_IN_POS[1]
        && data[3] == COMMAND_MOVE_HOLDER_IN_POS[3] && data[4] == COMMAND_MOVE_HOLDER_IN_POS[4]) {
        std::string line = data;
        std::vector<std::string> splitLine = seasocks::split(line, ':');
        double dx = atof(splitLine[2].c_str());
        double dy = atof(splitLine[4].c_str());
        _delegate->doEvent(std::make_shared<EventWS>(EVENT_MOVE_HOLDER_IN_POS, dx, dy));
        return;
    }

    if (strcmp(data, COMMAND_REG_FIRE) == 0) {
        _delegate->doEvent(eventSetModeRegFire);
        return;
    }

    if (strcmp(data, COMMAND_STOP_REG_FIRE) == 0) {
        _delegate->doEvent(eventStopModeRegFire);
        return;
    }

    if (strcmp(data, COMMAND_SWAP_STEAMS) == 0) {
        _delegate->doEvent(eventSwapStreams);
        return;
    }

    if (strcmp(data, COMMAND_PDOT_PLUS) == 0) {
        _delegate->doEvent(eventPDotPlus);
        return;
    }

    if (strcmp(data, COMMAND_PDOT_MINUS) == 0) {
        _delegate->doEvent(eventPDotMinus);
        return;
    }

    if (strcmp(data, COMMAND_FUSE_INSTALL) == 0) {
        _delegate->doEvent(eventFuseInstall);
        return;
    }

    if (strcmp(data, COMMAND_FUSE_REMOVE) == 0) {
        _delegate->doEvent(eventFuseRemove);
        return;
    }

    if (strcmp(data, COMMAND_GET_FIRE_CORR_ARRAY) == 0) {
        _delegate->doEvent(eventGetFireCorrArray);
        return;
    }

    if (strcmp(data, COMMAND_GET_FIRE_CORR_INDEX) == 0) {
        _delegate->doEvent(eventGetFireCorrIndex);
        return;
    }

    if (strlen(data) > 8
        && data[0] == COMMAND_SET_FIRE_CORR_INDEX[0] && data[3] == COMMAND_SET_FIRE_CORR_INDEX[3]
        && data[6] == COMMAND_SET_FIRE_CORR_INDEX[6] && data[7] == ':') {
        std::string line = data;
        int inx = std::stoi(line.substr(8, line.size() - 1), nullptr, 10);
        _delegate->doEvent(std::make_shared<EventWS>(EVENT_SET_FIRE_CORR_INDEX, inx));
        return;
    }

    if (strlen(data) > 13
        && data[0] == COMMAND_FIX_FIRE_CORR[0] && data[1] == COMMAND_FIX_FIRE_CORR[1]
        && data[4] == COMMAND_FIX_FIRE_CORR[4] && data[5] == COMMAND_FIX_FIRE_CORR[5]) {
        std::string line = data;
        std::vector<std::string> splitLine = seasocks::split(line, ':');
        double dx = atof(splitLine[2].c_str());
        double dy = atof(splitLine[4].c_str());
        _delegate->doEvent(std::make_shared<EventWS>(EVENT_FIX_FIRE_CORR, dx, dy));
        return;
    }

    if (strcmp(data, COMMAND_SET_DEFAULT_FULL_FIRE_CORR_TBL) == 0) {
        _delegate->doEvent(eventSetDefaultFullFireCorrTable);
        return;
    }

    if (strlen(data) > 12
        && data[0] == COMMAND_SET_DEFAULT_FIRE_CORR_BY_INDEX[0]
        && data[3] == COMMAND_SET_DEFAULT_FIRE_CORR_BY_INDEX[3]
        && data[4] == COMMAND_SET_DEFAULT_FIRE_CORR_BY_INDEX[4]
        && data[8] == COMMAND_SET_DEFAULT_FIRE_CORR_BY_INDEX[8]
        && data[11] == ':') {
        std::string line = data;
        std::vector<std::string> splitLine = seasocks::split(line, ':');
        int index = atoi(splitLine[1].c_str());
        _delegate->doEvent(std::make_shared<EventWS>(EVENT_SET_DEFAULT_FIRE_CORR_BY_INDEX, index));
        return;
    }

    if (strcmp(data, COMMAND_HOLDER_FUSE_UP) == 0) {
        std::cout << "Got FUSE UP\n";
        _delegate->doEvent(eventHolderFuseUp);
        return;
    }

    if (strcmp(data, COMMAND_HOLDER_FUSE_DOWN) == 0) {
        std::cout << "Got FUSE DOWN\n";
        _delegate->doEvent(eventHolderFuseDown);
        return;
    }

    if (strlen(data) > 7
        && data[0] == COMMAND_HOLDER_DO_FIRE[0]
        && data[1] == COMMAND_HOLDER_DO_FIRE[1]
        && data[3] == COMMAND_HOLDER_DO_FIRE[3]
        && data[4] == COMMAND_HOLDER_DO_FIRE[4]
        && data[5] == COMMAND_HOLDER_DO_FIRE[5]
        && data[7] == ':') {
        std::cout << "Got DO FIRE\n";
        std::string line = data;
        std::vector<std::string> splitLine = seasocks::split(line, ':');
        int countFires = atoi(splitLine[1].c_str());
        _delegate->doEvent(std::make_shared<EventWS>(EVENT_HOLDER_DO_FIRE, countFires));
        return;
    }
}

void MyHandler::doEventHandling(int pushedButtons) {
    if (pushedButtons & PUSHED_0) {
        _delegate->doEvent(eventStopMoving);
        return;
    }
    if (pushedButtons & PUSHED_UP) {
        if (pushedButtons & PUSHED_LEFT) {
            _delegate->doEvent(eventMoveForwardLeft);
        } else if (pushedButtons & PUSHED_RIGHT) {
            _delegate->doEvent(eventMoveForwardRight);
        } else {
            _delegate->doEvent(eventMoveForward);
        }
        return;
    }
    if (pushedButtons & PUSHED_DOWN) {
        if (pushedButtons & PUSHED_LEFT) {
            _delegate->doEvent(eventMoveBackLeft);
        } else if (pushedButtons & PUSHED_RIGHT) {
            _delegate->doEvent(eventMoveBackRight);
        } else {
            _delegate->doEvent(eventMoveBack);
        }
        return;
    }
    if (pushedButtons & PUSHED_LEFT) {
        _delegate->doEvent(eventMoveLefter);
        return;
    }
    if (pushedButtons & PUSHED_RIGHT) {
        _delegate->doEvent(eventMoveRighter);
        return;
    }

    if (pushedButtons & PUSHED_Q) {
        _delegate->doEvent(eventCamZM);
    } else if (pushedButtons & PUSHED_E) {
        _delegate->doEvent(eventCamZP);
    }

    if (pushedButtons & PUSHED_W) {
        _delegate->doEvent(eventCamUp);
    } else if (pushedButtons & PUSHED_S) {
        _delegate->doEvent(eventCamDown);
    }

    if (pushedButtons & PUSHED_D) {
        _delegate->doEvent(eventCamRight);
    } else if (pushedButtons & PUSHED_A) {
        _delegate->doEvent(eventCamLeft);
    }
}
