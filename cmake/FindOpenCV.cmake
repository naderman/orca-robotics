# Locate OpenCv-0.9.9 install directory

# This module defines
# OPENCV_HOME where to find include, lib, bin, etc.
# OPENCV_FOUND, If false, don't try to use Ice.

INCLUDE (${ORCA_CMAKE_DIR}/FindPkgConfig.cmake)

if( CMAKE_PKGCONFIG_EXECUTABLE )

    # Find all the opencv stuff with pkg-config
    PKGCONFIG( "opencv >= 0.9.9" OPENCV_FOUND OPENCV_INCLUDE_DIRS OPENCV_DEFINES OPENCV_LINK_DIRS OPENCV_LIBS )

    if( OPENCV_FOUND )
       message( STATUS "   Includes in: ${OPENCV_INCLUDE_DIRS}")
       message( STATUS "   Libraries in: ${OPENCV_LINK_DIRS}")
       message( STATUS "   Libraries: ${OPENCV_LIBS}")
       message( STATUS "   Defines: ${OPENCV_DEFINES}")
    endif( OPENCV_FOUND )

else( CMAKE_PKGCONFIG_EXECUTABLE )

    # Can't find pkg-config -- have to search manually

endif( CMAKE_PKGCONFIG_EXECUTABLE )

# Test the ability to connect to a camera (opencv does not require libdc1394 to be built... we do for the cameraopencv driver

TRY_COMPILE( OPENCV_CAMERASUPPORT 
             ${PROJECT_BINARY_DIR}/CMakeTmp 
             ${ORCA_CMAKE_DIR}/opencvcameratest/main.cpp
             CMAKE_FLAGS "-DLINK_LIBRARIES=${OPENCV_LIBS}"
             OUTPUT_VARIABLE OUTPUT
           )

