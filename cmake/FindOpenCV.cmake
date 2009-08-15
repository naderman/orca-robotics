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
