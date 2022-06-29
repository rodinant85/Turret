cmake_minimum_required(VERSION 3.13)
project(machine_gun_rear_sight)

set(CMAKE_CXX_STANDARD 20)


unset(CAMERA_INCLUDE_DIR CACHE)
find_path(
        CAMERA_INCLUDE_DIR
        NAMES "libcamera/libcamera.h"
        PATH_SUFFIXES libcamera
)
message(${CAMERA_INCLUDE_DIR})


find_library(CAMERA_LIBRARY
        NAMES camera
        PATH_SUFFIXES libcamera
        )
message(${CAMERA_LIBRARY})
message(${CAMERA_INCLUDE_DIR})

include_directories(
        .
        ${CAMERA_INCLUDE_DIR}
)

add_executable(rear_sight
###      source file with the project begin function
        main.cpp

###      web server events
        web_server/web_server_events/body_moving_events.h
        web_server/web_server_events/camera_roi_events.h
        web_server/web_server_events/control_mode_events.h
        web_server/web_server_events/web_server_events.h

###      web server
        web_server/web_server.h
        web_server/web_server.cpp

###      class for the handling process in other thread for the WebServer events
        web_server_worker.h
        web_server_worker.cpp

###      my own system for calculating bullet ballistic
        process_camera/ballistic_calculator/ballistic_calculator.h
        process_camera/ballistic_calculator/ballistic_calculator.cpp

###      my own handling sub system
        web_server/lib_my_event_handler/delegate_ws.h
        web_server/lib_my_event_handler/delegate_ws.cpp
        web_server/lib_my_event_handler/event_ws.h
        web_server/lib_my_event_handler/event_ws.cpp
        web_server/lib_my_event_handler/handler_ws.h

###      this code of the eye lib which used libcamera for getting and native processing video stream
        process_camera/native_camera_eye/Eye.h
        process_camera/native_camera_eye/Eye.cpp
        process_camera/native_camera_eye/EventLoop.h
        process_camera/native_camera_eye/EventLoop.cpp
        process_camera/native_camera_eye/CameraThread.h
        process_camera/native_camera_eye/CameraThread.cpp
        process_camera/native_camera_eye/Tracker.h
        process_camera/native_camera_eye/Tracker.cpp

###      this is the source code for the image processing in the gst pipeline
        process_camera/rear_sight_processor/image_processing.h
        process_camera/rear_sight_processor/rear_sight_processor.h
        process_camera/rear_sight_processor/rear_sight_processor_keep_frame_proportionality.cpp
        process_camera/rear_sight_webrtc_manipulation.cpp
        process_camera/rear_sight_webrtc_manipulation.h
        web_server/web_server_events/holder_commands.h

###      this is the source code for working with a holder controller via a SPI interface
        holder_processing/spi_library/spi.h
        holder_processing/spi_library/spi.cpp
        holder_processing/holder_controller.h
        holder_processing/holder_controller.cpp

###      this is own logging system
        logging/LOG.h
        )

###       this is a target compile definitions for the different platforms (both linux - the first one is my work PC under
###       Ubuntu 20.04 and the second one is Raspberry Pi3 under Raspbian)
#target_compile_definitions(rear_sight PRIVATE UBUNTU_PC)
target_compile_definitions(rear_sight PRIVATE RASPBERRY_PI)
set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -latomic")

###       this is a target compile definitions for the DEBUG development steps
target_compile_definitions(rear_sight PRIVATE MY_DEBUG)

###       this compile definition sets POWER_TUNE for control iron-turtle wheel, otherwise it will control both speed and power
target_compile_definitions(rear_sight PRIVATE POWER_TUNE)

###       this compile definition sets for video-stream resolution
#target_compile_definitions(rear_sight PRIVATE RES_360P)
#target_compile_definitions(rear_sight PRIVATE RES_480P)
target_compile_definitions(rear_sight PRIVATE RES_720P)
#target_compile_definitions(rear_sight PRIVATE RES_1080P)

###       this compile definition sets fro video-stream default framerate
#target_compile_definitions(rear_sight PRIVATE FRAMERATE_5)
target_compile_definitions(rear_sight PRIVATE FRAMERATE_15)
#target_compile_definitions(rear_sight PRIVATE FRAMERATE_30)
#target_compile_definitions(rear_sight PRIVATE FRAMERATE_60)

###       Adding to the project the 'seasocks' library for the WebServer which works as WebSocket server
target_link_libraries(rear_sight seasocks "${ZLIB_LIBRARIES}")
###       Adding to the project
target_link_libraries(rear_sight pthread z)
###       Adding to the project other libs
target_link_libraries(rear_sight
        ${CAMERA_LIBRARY}
        tiff event event event_pthreads)

###       Adding to the project the 'opencv' library it used for the frame processing (and, +- as an object detector) and as an object tracker
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(rear_sight ${OpenCV_LIBS})

find_package(PkgConfig)
if (PKG_CONFIG_FOUND)
    pkg_check_modules(GLIB "glib-2.0")
    if (GLIB_FOUND)
        target_link_libraries(rear_sight ${GLIB_LIBRARIES})
        add_definitions(${GLIB_CFLAGS} ${GLIB_CFLAGS_OTHER})
    endif()
endif()

find_package(PkgConfig)
if (PKG_CONFIG_FOUND)
    pkg_check_modules(GST "gstreamer-1.0")
    if (GST_FOUND)
        target_link_libraries(rear_sight ${GST_LIBRARIES})
        add_definitions(${GST_CFLAGS} ${GST_CFLAGS_OTHER})
    endif()
endif()

find_package(PkgConfig)
if (PKG_CONFIG_FOUND)
    pkg_check_modules(SPD "gstreamer-sdp-1.0")
    if (SPD_FOUND)
        target_link_libraries(rear_sight ${SPD_LIBRARIES})
        add_definitions(${SPD_CFLAGS} ${SPD_CFLAGS_OTHER})
    endif()
endif()

find_package(PkgConfig)
if (PKG_CONFIG_FOUND)
    pkg_check_modules(WEBRTC "gstreamer-webrtc-1.0")
    if (WEBRTC_FOUND)
        target_link_libraries(rear_sight ${WEBRTC_LIBRARIES})
        add_definitions(${WEBRTC_CFLAGS} ${WEBRTC_CFLAGS_OTHER})
    endif()
endif()

find_package(PkgConfig)
if (PKG_CONFIG_FOUND)
    pkg_check_modules(JSON "json-glib-1.0")
    if (JSON_FOUND)
        target_link_libraries(rear_sight ${JSON_LIBRARIES})
        add_definitions(${JSON_CFLAGS} ${JSON_CFLAGS_OTHER})
    endif()
endif()

find_package(PkgConfig)
if (PKG_CONFIG_FOUND)
    pkg_check_modules(SOUP "libsoup-2.4")
    if (SOUP_FOUND)
        target_link_libraries(rear_sight ${SOUP_LIBRARIES})
        add_definitions(${SOUP_CFLAGS} ${SOUP_CFLAGS_OTHER})
    endif()
endif()

###       Copying the source files for the runnable file
add_custom_command(TARGET rear_sight POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/src $<TARGET_FILE_DIR:rear_sight>/src)
