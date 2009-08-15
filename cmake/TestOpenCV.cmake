# This module defines
# OPENCV_CAMERASUPPORT

# Test the ability to connect to a camera (opencv does not require libdc1394 to be built... 
# we do for the cameraopencv driver

# NOTE: this macro cannot be used from satellite projects (because it relies on the code which
# is not installed) (alexm)

TRY_COMPILE( OPENCV_CAMERASUPPORT 
             ${PROJECT_BINARY_DIR}/CMakeTmp 
             ${ORCA_CMAKE_DIR}/opencvcameratest/main.cpp
             CMAKE_FLAGS "-DLINK_LIBRARIES=${OPENCV_LIBS}"
             OUTPUT_VARIABLE OUTPUT
           )
