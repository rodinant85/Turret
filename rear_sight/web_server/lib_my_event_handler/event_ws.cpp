//
// Created by biba_bo on 2020-08-18.
//

#include "event_ws.h"

EventWS::EventWS(int idEvent) {
    _idEvent = idEvent;
    _data = 0.0;
}

EventWS::EventWS(int idEvent, double data) {
    _idEvent = idEvent;
    _data = data;
    _dataSecond = 0.0;
}

EventWS::EventWS(int idEvent, double dataFirst, double dataSecond) {
    _idEvent = idEvent;
    _data = dataFirst;
    _dataSecond = dataSecond;
}

EventWS::EventWS(int idEvent, int x_coord, int y_coord) {
    _idEvent = idEvent;
    _x = x_coord;
    _y = y_coord;
}

EventWS::EventWS(int idEvent, double data, int width, int height) {
    _idEvent = idEvent;
    _data = data;
    _width = width;
    _height = height;
}

int EventWS::getEventID() {
    return _idEvent;
}

double EventWS::getData() {
    return _data;
}

double EventWS::getDataFirst() {
    return _data;
}

int EventWS::getWidth() {
    return _width;
}

int EventWS::getHeight() {
    return _height;
}

double EventWS::getDataSecond() {
    return _dataSecond;
}
int EventWS::getX() {
    return _x;
}

int EventWS::getY() {
    return _y;
}
