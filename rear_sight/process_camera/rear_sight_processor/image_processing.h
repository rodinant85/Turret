//
// Created by biba_bo on 2020-08-14.
//

#ifndef REAR_SIGHT_CLONE_PROJECT_IMAGE_PROCESSING_H
#define REAR_SIGHT_CLONE_PROJECT_IMAGE_PROCESSING_H

#ifdef FRAMERATE_5
#define STR_FRAMERATE           "5/1"
#endif //FRAMERATE_5

#ifdef FRAMERATE_15
#define STR_FRAMERATE           "15/1"
#endif //FRAMERATE_15

#ifdef FRAMERATE_30
#define STR_FRAMERATE           "30/1"
#endif //FRAMERATE_30

#ifdef FRAMERATE_60
#define STR_FRAMERATE           "60/1"
#endif //FRAMERATE_60


/*This defines are used for cropping an image from the current frame
  in future code modifications it can be changed to variables
  ! be careful with frame's WIDTH nad HEIGHT
 * CROPPED_WIDTH - width of cropping
 * CROPPED_HEIGHT - height of cropping
 * CROPPED_X, CROPPED_Y - a coordinates of cropping*/
#define WIDTH                     4056
#define HEIGHT                    2600
#define CROPPED_WIDTH_2           4056
#define CROPPED_HEIGHT_2          2600
#define CROPPED_X_BEGIN           0
#define CROPPED_Y_BEGIN           0

#define ZOOM_STEP                 1
#define MOVE_STEP                 1

#ifdef RES_1080P
/*This is a video settings for the camera, it's impossible to change dynamically
 * STR_... version is used in configuring pipeline.*/
//                              1080p
#define WIDTH_STREAM_FRAME      1920
#define HEIGHT_STREAM_FRAME     1080
#define STR_WIDTH_STREAM_FRAME  "1920"
#define STR_HEIGHT_STREAM_FRAME "1080"

/*This defines are used for resizing the current frame into a small image, where:
  in future code modifications it can be changed to variables
  ! be careful with frame's WIDTH nad HEIGHT
 * RESIZE_WIDTH - a width for a small image
 * RESIZE_HEIGHT - a height for a small image*/
//                             1080p
#define RESIZE_WIDTH           600
#define RESIZE_HEIGHT          338
#define RESIZE_X               1320
#define RESIZE_Y               0


/* This is managing constants for drawing helpful elements on a video frame*/
//                              1080p
#define DRAW_CIRCLE_X           960
#define DRAW_CIRCLE_Y           540
#define DRAW_CIRCLE_RADIUS      30
// #define DRAW_LINE_LENGTH     150
#define DRAW_LINE_1B_X          960
#define DRAW_LINE_1B_Y          525
#define DRAW_LINE_1E_X          960
#define DRAW_LINE_1E_Y          375
//
#define DRAW_LINE_2B_X          975
#define DRAW_LINE_2B_Y          540
#define DRAW_LINE_2E_X          1125
#define DRAW_LINE_2E_Y          540
//
#define DRAW_LINE_3B_X          960
#define DRAW_LINE_3B_Y          555
#define DRAW_LINE_3E_X          960
#define DRAW_LINE_3E_Y          705
//
#define DRAW_LINE_4B_X          945
#define DRAW_LINE_4B_Y          540
#define DRAW_LINE_4E_X          795
#define DRAW_LINE_4E_Y          540
#endif //RES_1080P

#ifdef RES_720P
/*This is a video settings for the camera, it's impossible to change dynamically
 * STR_... version is used in configuring pipeline.*/
//                              720p
#define WIDTH_STREAM_FRAME      1280
#define HEIGHT_STREAM_FRAME     720
#define STR_WIDTH_STREAM_FRAME  "1280"
#define STR_HEIGHT_STREAM_FRAME "720"

/*This defines are used for resizing the current frame into a small image, where:
  in future code modifications it can be changed to variables
  ! be careful with frame's WIDTH nad HEIGHT
 * RESIZE_WIDTH - a width for a small image
 * RESIZE_HEIGHT - a height for a small image*/
//                             720p
#define RESIZE_WIDTH           400
#define RESIZE_HEIGHT          225
//#define RESIZE_X               880
#define RESIZE_X               0
#define RESIZE_Y               0


/* This is managing constants for drawing helpful elements on a video frame*/
//                              720p
#define DRAW_CIRCLE_X           640
#define DRAW_CIRCLE_Y           360
#define DRAW_CIRCLE_RADIUS      20

#define DRAW_FOCAL_CIRCLE_RADIUS 2

#define DRAW_LINE_LENGTH        250

#define DRAW_LINE_1B_X          640
#define DRAW_LINE_1B_Y          (360 - DRAW_LINE_LENGTH)
#define DRAW_LINE_1E_X          640
#define DRAW_LINE_1E_Y          360
//
#define DRAW_LINE_2B_X          640
#define DRAW_LINE_2B_Y          360
#define DRAW_LINE_2E_X          (640 + DRAW_LINE_LENGTH)
#define DRAW_LINE_2E_Y          360
//
#define DRAW_LINE_3B_X          640
#define DRAW_LINE_3B_Y          360
#define DRAW_LINE_3E_X          640
#define DRAW_LINE_3E_Y          (360 + DRAW_LINE_LENGTH)
//
#define DRAW_LINE_4B_X          640
#define DRAW_LINE_4B_Y          360
#define DRAW_LINE_4E_X          (640 - DRAW_LINE_LENGTH)
#define DRAW_LINE_4E_Y          360
#endif

#ifdef RES_480P
/*This is a video settings for the camera, it's impossible to change dynamically
 * STR_... version is used in configuring pipeline.*/
//                               480p
#define WIDTH_STREAM_FRAME       640
#define HEIGHT_STREAM_FRAME      480
#define STR_WIDTH_STREAM_FRAME   "640"
#define STR_HEIGHT_STREAM_FRAME  "480"

/*This defines are used for resizing the current frame into a small image, where:
  in future code modifications it can be changed to variables
  ! be careful with frame's WIDTH nad HEIGHT
 * RESIZE_WIDTH - a width for a small image
 * RESIZE_HEIGHT - a height for a small image*/
//                             480p
#define RESIZE_WIDTH           200
#define RESIZE_HEIGHT          160
#define RESIZE_X               440
#define RESIZE_Y               0


/* This is managing constants for drawing helpful elements on a video frame*/
//                              480p
#define DRAW_CIRCLE_X           320
#define DRAW_CIRCLE_Y           240
#define DRAW_CIRCLE_RADIUS      10
// #define DRAW_LINE_LENGTH     200
#define DRAW_LINE_1B_X          320
#define DRAW_LINE_1B_Y          90
#define DRAW_LINE_1E_X          320
#define DRAW_LINE_1E_Y          230
//
#define DRAW_LINE_2B_X          330
#define DRAW_LINE_2B_Y          240
#define DRAW_LINE_2E_X          480
#define DRAW_LINE_2E_Y          240
//
#define DRAW_LINE_3B_X          320
#define DRAW_LINE_3B_Y          250
#define DRAW_LINE_3E_X          320
#define DRAW_LINE_3E_Y          400
//
#define DRAW_LINE_4B_X          310
#define DRAW_LINE_4B_Y          240
#define DRAW_LINE_4E_X          160
#define DRAW_LINE_4E_Y          240
#endif

#ifdef RES_360P
/*This is a video settings for the camera, it's impossible to change dynamically
 * STR_... version is used in configuring pipeline.*/
//                              360p
#define WIDTH_STREAM_FRAME      640
#define HEIGHT_STREAM_FRAME     360
#define STR_WIDTH_STREAM_FRAME  "640"
#define STR_HEIGHT_STREAM_FRAME "360"

/*This defines are used for resizing the current frame into a small image, where:
  in future code modifications it can be changed to variables
  ! be careful with frame's WIDTH nad HEIGHT
 * RESIZE_WIDTH - a width for a small image
 * RESIZE_HEIGHT - a height for a small image*/
//                             360p
#define RESIZE_WIDTH           200
#define RESIZE_HEIGHT          120
#define RESIZE_X               440
#define RESIZE_Y               0


/* This is managing constants for drawing helpful elements on a video frame*/
//                              360p
#define DRAW_CIRCLE_X           320
#define DRAW_CIRCLE_Y           180
#define DRAW_CIRCLE_RADIUS      10
// #define DRAW_LINE_LENGTH     150
#define DRAW_LINE_1B_X          320
#define DRAW_LINE_1B_Y          170
#define DRAW_LINE_1E_X          320
#define DRAW_LINE_1E_Y          20
//
#define DRAW_LINE_2B_X          320
#define DRAW_LINE_2B_Y          190
#define DRAW_LINE_2E_X          320
#define DRAW_LINE_2E_Y          340
//
#define DRAW_LINE_3B_X          310
#define DRAW_LINE_3B_Y          180
#define DRAW_LINE_3E_X          160
#define DRAW_LINE_3E_Y          180
//
#define DRAW_LINE_4B_X          330
#define DRAW_LINE_4B_Y          180
#define DRAW_LINE_4E_X          480
#define DRAW_LINE_4E_Y          180
#endif

class FrameParameters{
public:
    int CROPPED_WIDTH = CROPPED_WIDTH_2;
    int CROPPED_HEIGHT = CROPPED_HEIGHT_2;
    int CROPPED_X = CROPPED_X_BEGIN;
    int CROPPED_Y = CROPPED_Y_BEGIN;
};

#endif //REAR_SIGHT_CLONE_PROJECT_IMAGE_PROCESSING_H