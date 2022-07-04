//
// Created by biba_bo on 2020-08-18.
//

#ifndef IRON_TURTLE_WEB_SERVER_WORKER_H
#define IRON_TURTLE_WEB_SERVER_WORKER_H

#include <random>
#include <string>
#include <thread>
#include <chrono>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <zconf.h>

#include "web_server/web_server.h"
#include "web_server/lib_my_event_handler/handler_ws.h"

#include "process_camera/rear_sight_webrtc_manipulation.h"
#include "holder_processing/holder_controller.h"

/** This is the class WebServerWorker, which extends the abstract class HandlerWS (which are a part of my own handling system)
 * it used for handling interrupts from the WebServer, and works width hardware drivers*/
class WebServerWorker : public HandlerWS {
public:
    /** Main constructor here starts a new thread for the webserver*/
    WebServerWorker();

    /**
     * @brief main constructor with path to web files
     * @param pathToWebFiles - this is a path to folder with web server files
     */
    WebServerWorker(const std::string &pathToWebFiles);

    /** Destructor*/
    ~WebServerWorker();

    /** The function for joining a webserver's thread */
    void joinServerTread();

    /** The overriding a function from the class HandlerWS */
    void handleEventWS(std::shared_ptr<EventWS> event) override;

private:
    /**
     * @brief this function starts a web server
     */
    void start_web_server_processing();

    /**
     * The private method for starting web-server thread */
    void startServer();

    /** Method for processing a "hard" request from webserver in the another thread */
    void processServer();

    /**
     * @brief this function do processing an iron turtle body moving
     * @param eventCode - just an event code
     */
    void handleBodyMovingEvents(int eventCode);

    /**
     * @brief this function do processing an camera ROI's change
     */
    void handleCamaraROIEvents(int eventCode);

    /**
     * @brief this function do processing an changing an
     */
    void handleControlModeEvents(std::shared_ptr<EventWS> event);

    /**
     * @brief this function handle events related with holder
     */
     void handleHolderEvents(std::shared_ptr<EventWS> eventWs);

    /** The field with the web-server thread */
    std::thread server_thread;
    /** The field with the loggering system for web-server */
    shared_ptr<PrintfLogger> logger;
    /** The field with MySever - this is my web-server, which process connection with web clients */
    shared_ptr<MyServer> ws_server;
    /** The field with MyHandler - this is my handler for events from web-clients, but it handle only events:
     * onConnection, onDisconnecting, onMessage, and only in the web-server thread*/
    shared_ptr<MyHandler> handler;
//    /**
//     * @brief this is a path to the web-files folder
//     */
//    std::string pathToWebFiles;

    /** This field contains an object of HandlerController class,
     * that used for handling Holder events*/
    std::shared_ptr<HolderController> _holderController;
};


#endif //IRON_TURTLE_WEB_SERVER_WORKER_H