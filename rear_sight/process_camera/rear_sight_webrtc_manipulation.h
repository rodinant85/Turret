//
// Created by biba_bo on 2020-08-21.
//

#ifndef IRON_TURTLE_REAR_SIGHT_WEBRTC_MANIPULATION_H
#define IRON_TURTLE_REAR_SIGHT_WEBRTC_MANIPULATION_H

#include "rear_sight_processor/image_processing.h"
#include "rear_sight_processor/rear_sight_processor.h"
#include "process_camera/native_camera_eye/CameraThread.h"

#include <locale.h>
#include <glib.h>
#include <gst/gst.h>
#include <gst/sdp/sdp.h>

#ifdef G_OS_UNIX
#include <glib-unix.h>
#endif

#define GST_USE_UNSTABLE_API
#include <gst/webrtc/webrtc.h>

#include <json-glib/json-glib.h>
#include <string.h>

#include <opencv2/opencv.hpp>
#include <iostream>
#include <seasocks/WebSocket.h>

/*This ia a payload server for possibility to establish client-server connection
 * I tried to use a local payload server, but I had difficulties with configuring it.*/
#define RTP_PAYLOAD_TYPE                "96"
//#define STUN_SERVER                     "stun.l.google.com:19302"

#define STUN_SERVER                     "127.0.0.1:3478"
//#define STUN_SERVER                     "192.168.1.131:3478"

/// Struct which contains pipeline and other GstElements, for comfortable work with connection.
typedef struct _ReceiverEntry ReceiverEntry;

/// Creating a ReceiverEntry structure. Usually it uses when happens a new connection.
ReceiverEntry *create_receiver_entry (seasocks::WebSocket * connection);
/// Destructing a ReceiverEntry structure. Last one is used often, when we often change a connection.
void destroy_receiver_entry (gpointer receiver_entry_ptr);

/** This .h file created in the C++ class style, but, cause some functions are called in other places, I cannot use a class structure,
 * so this one just initialize variables for processing a frame, and an object recognition object (here is also the thread for this processing).*/
void init_rear_sight_processor();

/** This function is one of the three steps of the WebRTC connection processing.
 * This one are called when the offer created and the realtime stream can be started.*/
void on_offer_created_cb (GstPromise * promise, gpointer user_data);
/** This function is one of the three steps of the WebRTC connection processing.
 * This one are called when the GST pipeline are prepared.*/
void on_negotiation_needed_cb (GstElement * webrtcbin, gpointer user_data);
/** This function is one of the three steps of the WebRTC connection processing.
 * This one are called when new user want to get a real time media stream.*/
void on_ice_candidate_cb (GstElement * webrtcbin, guint mline_index,
                          gchar * candidate, gpointer user_data);

/** This function handle webrtc session, and dependently from the client message it calls functions:
 * 'on_offer_created_cb', 'on_negotiation_needed_cb', 'on_ice_candidate_cb', or throw an exception.*/
void webrtc_session_handle (const char * message);

/** This function create a new ReceiverEntry */
void webrtc_pipeline_restart(seasocks::WebSocket *connection);
/** This function doing disconnection of the connected client, and breaks an pipeline*/
void webrtc_pipeline_deactivate(seasocks::WebSocket *connection);

/** This function is a realization of GST loop (kind of the independent thread) for processing a current pipeline*/
int webrtc_gst_loop(seasocks::WebSocket *connection);

/** This function is converts a JsonObjet to the string*/
static gchar *get_string_from_json_object (JsonObject * object);

/// This function is manage a cropping process, and this one DECREASE a cropped image size
void on_zoom_plus_processor();
/// This function is manage a cropping process, and this one INCREASE a cropped image size
void on_zoom_minus_processor();
/// This function is returns a cropping percent
double get_cropping_percent();

/// This function is manage a cropping process, and this one MOVE a cropped window to the LEFT side
void on_move_left_processor();
/// This function is manage a cropping process, and this one MOVE a cropped window to the RIGHT side
void on_move_right_processor();
/// This function is manage a cropping process, and this one MOVE a cropped window to the TOP side
void on_move_up_processor();
/// This function is manage a cropping process, and this one MOVE a cropped window to the BOTTOM side
void on_move_down_processor();

/// this function starts registration fire
void onStartRegistrationFire();
/// this function stops registration fire and save it to file
void onStopRegistrationFire();

void startTracking(int x, int y);

/// this function returns status of registration of fire
bool isRegistrationFire();

/// this function swap streams
void swapStreams();
/// this function returns is streams swapped or no
bool isStreamsSwapped();

/// this function used for configuring plane dots (for increasing distance between these)
void planeDotsPlusDistance();
/// this function used for configuring plane dots (for decreasing distance between these)
void planeDotsMinusDistance();

/// this function used for disabling fuse
void removeFuse();
/// this function used for enabling fuse
void installFuse();

/// this function used for selecting next correction
void setNextFireCorrection();
/// this function used for selecting previous correction
void setPreviousFireCorrection();

/// this function used getting string with JSON formatted array of fire corrections
std::string getFireCorrectionsLineJSON();
/// this function used for getting index of selected fire correction
int getFireCorrectionIndex();
/// this function used for setting selecting index of fire correction
void setFireCorrectionIndex(int index);
/// this function used for applying fire correction on this distance
void setFireCorrection(double dx, double dy);

/// this function used for returning the fire correction table to the initial state
void setDefaultFireCorrectionTable();
/// this function used for returning default value for only one record in the fire correction table by the index
void setDefaultFireCorrectionRecord(int index);

/// this function used for enabling/disabling showing debug messages in the stream
void setShowingDebugMessagesInStream();

/** This is a realization of the ReceiverEntry and this one contains
 * seasocks::WebSocket connection - this is a current connection and it used for establishing WebRTC connection
 * GstElement *ocvvideosrc - this is a Gst element, used for modification a pipeline
 * GstElement *pipeline - this is a Gst element, used for controlling whole pipeline
 * GstElement *webrtcbin - this is a Gst element, used for establishing WebRTC connection*/
struct _ReceiverEntry
{
    seasocks::WebSocket *connection;

    GstElement *ocvvideosrc;
    GstElement *pipeline;
    GstElement *webrtcbin;
};

/// This is a pointer to ReceiverEntry - used for establishing WebRTC connection and modification a stream in the pipeline
static ReceiverEntry *my_receiver_entry;
/// This is a pointer to GMainLoop - this is a kind of thread for the GST pipeline
static GMainLoop *mainloop;
/// This is frame counter - this one is says us how many frames we should skip, before send a frame to the FormDetectionProcessor
static int count_frames;

/// this is an object with a speed value (here is not an actual speed, this is values for creating speed commands)
static std::string current_speed_0 = "First  Speed LW=0, RW=0";

/// this is an object with a speed value (here is not an actual speed, this is values for creating speed commands)
static std::string current_speed_1 = "Second Speed LW=0, RW=0";

/// this is an object with a POWER value (here is not an actual POWER, this is values for creating POWER commands)
static std::string currentPower = "POWER=0";

static CameraThread* eyeCameraThread;

#endif //IRON_TURTLE_REAR_SIGHT_WEBRTC_MANIPULATION_H
