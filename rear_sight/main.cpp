#include <signal.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

/// Included a source code for working with Seasocks server.
#include "web_server_worker.h"

void my_handler(int s){
    printf("Caught signal %d\n",s);
    exit(1);
}

/** This is a start point of the program*/
int main(int argc, char *argv[]) {
    // save default frame size
    std::ofstream myfile;
    myfile.open ("src/sizes.conf");
    std::string message;
    message = "{\"sizes\":{\"full_width\":" + std::to_string(WIDTH_STREAM_FRAME) +
            ",\"full_height\":" + std::to_string(HEIGHT_STREAM_FRAME) + ",\"zoom\":0.0445}}";
    myfile << message;
    myfile.close();
    //----------------
    
    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = my_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);

    //this is an instance of the interrupt handling delegator.
    DelegateWS* delegate = DelegateWS::getInstance();
    //this is start point of working webserver and system at all, also here starts threads for webserver, handling thread,
    // and thread for the iron turtle driver.
    WebServerWorker *worker;
//    if (argc > 1) {
//        std::cout << argv[1] << "\n";
//        worker = new WebServerWorker(argv[0]);
//    } else {
//        worker = new WebServerWorker();
//    }
    worker = new WebServerWorker();
    // adding our WebServerWorker as delegator for the WebServer.
    delegate->addHandler(worker);
    // now this line are useful cause, this thread hasn't a loop, so it will fails without joining the server thread
    worker->joinServerTread();
    return 0;
}
