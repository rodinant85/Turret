//
// Created by biba_bo on 2020-08-18.
//

#ifndef IRON_TURTLE_WEB_SERVER_H
#define IRON_TURTLE_WEB_SERVER_H

#include <seasocks/PrintfLogger.h>      // This class used for logging any events with the webserver
#include <seasocks/Server.h>            // This class is just server model
#include <seasocks/StringUtil.h>        // This class contains string utils for an message editing
#include <seasocks/WebSocket.h>         // This class contains class WebSocket which I extends for my own functionality
#include <seasocks/util/Json.h>         // This class contains utils for working with JSON messages (e.g. the JSON parser)
#include <seasocks/PageHandler.h>       // This class contains a page handler for connection processing
#include <json-glib/json-glib.h>        // This class contains a json parser, which actually uses for the client message parsing.

#include "lib_my_event_handler/event_ws.h"                      // This is a class with a custom event class.
#include "lib_my_event_handler/delegate_ws.h"                   // This is a class with a custom event delegator (works as singleton)
#include "../process_camera/rear_sight_webrtc_manipulation.h"   // This is a .h file with manipulations with current frame and its modification, here uses for activating and other events processing
#include "web_server_events/web_server_events.h"                // This h.-file contains events codes for events, which can happens in the server

#include <iostream>    // For the working with external files (as .html, .js, .css, and so on).
#include <cstdio>      //
#include <chrono>
#include <cstdlib>
#include <getopt.h>
#include <memory>      // For using smart pointers
#include <cstring>     // For c-like string comparing
#include <set>         // As a set of the current connections
#include <sstream>
#include <string>
#include <thread>      // An object of the created threads
#include <mutex>       // For thread locking (possibly will be useful)


/// My own event code (this means we have a new active client's connection)
constexpr int EVENT_CLIENT_CONNECTED = 1011;
/// My own event code (this means we loose an active client's connection)
constexpr int EVENT_CLIENT_DISCONNECTED = 1012;

/// This command gets from a client, if the client wants
#define COMMAND_CLIENT_IS_STILL_HERE    "OK"

/// This command gets from a client, if the client wants
#define MESSAGE_FOR_EXCESS_CLIENT   "YOU_ARE_EXCESS"

using namespace std::chrono;
using namespace seasocks;
using namespace std;

class MyServer : public Server {
public:
    using Server::Server;

private:
    virtual void checkThread() const override {

    };
};


class MyHandler : public WebSocket::Handler {
public:
    explicit MyHandler(MyServer *server);

    /// overloading a handling function onConnect
    virtual void onConnect(WebSocket *connection);

    /// overloading a handling function onMessage or onData
    virtual void onData(WebSocket *connection, const char *data);

    /// overloading a handlilg function onDisconnect
    virtual void onDisconnect(WebSocket *connection);

    /// a function for sending messages for clients (in this version of the server for one client)
    void sendValuesJSON(std::string values);


private:
    ///set of WebSocket connections
    set<WebSocket *> _connections;
    ///pointer on object of this server
    Server *_server;
    ///instance of event delegator
    DelegateWS *_delegate;
    ///thread for new connection
    std::thread _rearSightThread;

    /** this variable answers on the question "Had it WebRTC connection/gst loop?"
      also by this variable we decide to create a new thread for gst loop or not*/
    bool _isFirstWebRTC_Connection;

    /// smart pointer to my own event "have a new active client's connection"
    std::shared_ptr<EventWS> eventClientConnected;
    /// smart pointer to my own event "lose an active client's connection"
    std::shared_ptr<EventWS> eventClientDisconnected;

    /// a private function for handling a vector commands from a client
    void doEventHandling(const char *command);

    /// a private function for handling a vector commands from a client
    void doEventHandling(int pushedButtons);
};

struct MyAuthHandler : PageHandler {
    std::shared_ptr<Response> handle(const Request &request) override {
        // Here one would handle one's authentication system, for example;
        // * check to see if the user has a trusted cookie: if so, accept it.
        // * if not, redirect to a login handler page, and await a redirection
        //   back here with relevant URL parameters indicating success. Then,
        //   set the cookie.
        // For this example, we set the user's authentication information purely
        // from their connection.
        request.credentials()->username = formatAddress(request.getRemoteAddress());
        return Response::unhandled(); // cause next handler to run
    }
};


#endif //IRON_TURTLE_WEB_SERVER_H
