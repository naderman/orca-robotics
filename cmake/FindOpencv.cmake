# Locate OpenCv-0.9.9 install directory

# This module defines
# OPENCV_HOME where to find include, lib, bin, etc.
# OPENCV_FOUND, If false, don't try to use Ice.

INCLUDE (${ORCA_CMAKE_DIR}/FindPkgConfig.cmake)

IF ( CMAKE_PKGCONFIG_EXECUTABLE )

#    MESSAGE(  STATUS "Using pkgconfig" )
    
    # Find all the opencv stuff with pkg-config
    PKGCONFIG( "opencv >= 0.9.9" OPENCV_FOUND OPENCV_INCLUDE_DIRS OPENCV_DEFINES OPENCV_LINK_DIRS OPENCV_LIBS )

    IF( OPENCV9_FOUND )
#        MESSAGE("   Includes in: ${OPENCV_INCLUDE_DIRS}")
#        MESSAGE("   Libraries in: ${OPENCV_LINK_DIRS}")
#        MESSAGE("   Libraries: ${OPENCV_LIBS}")
#        MESSAGE("   Defines: ${OPENCV_DEFINES}")
    ENDIF ( OPENCV9_FOUND )

ELSE  ( CMAKE_PKGCONFIG_EXECUTABLE )

    # Can't find pkg-config -- have to search manually

ENDIF ( CMAKE_PKGCONFIG_EXECUTABLE )
