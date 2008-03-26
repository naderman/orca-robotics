# Locate OpenCv-0.9.9 install directory

# This module defines
# OPENCV_HOME where to find include, lib, bin, etc.
# OPENCV_FOUND, If false, don't try to use Ice.

INCLUDE(${ORCA_CMAKE_DIR}/FindPkgConfig.cmake)

IF( CMAKE_PKGCONFIG_EXECUTABLE )

    # Find all the opencv stuff with pkg-config
    PKGCONFIG( "opencv >= 0.9.9" OPENCV_FOUND OPENCV_INCLUDE_DIRS OPENCV_DEFINES OPENCV_LINK_DIRS OPENCV_LIBS )

    IF( OPENCV_FOUND )
       MESSAGE( STATUS "   Includes in: ${OPENCV_INCLUDE_DIRS}")
       MESSAGE( STATUS "   Libraries in: ${OPENCV_LINK_DIRS}")
       MESSAGE( STATUS "   Libraries: ${OPENCV_LIBS}")
       MESSAGE( STATUS "   Defines: ${OPENCV_DEFINES}")
    ENDIF( OPENCV_FOUND )

ELSE  ( CMAKE_PKGCONFIG_EXECUTABLE )

    # Can't find pkg-config -- have to search manually

ENDIF( CMAKE_PKGCONFIG_EXECUTABLE )
