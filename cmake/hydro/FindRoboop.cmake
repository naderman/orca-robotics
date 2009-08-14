# Locate Roboop install directory

# This module defines
# ROBOOP_HOME where to find include, lib, bin, etc.
# ROBOOP_FOUND, If false, don't try to use Ice.

INCLUDE (${HYDRO_CMAKE_DIR}/FindPkgConfig.cmake)

if( CMAKE_PKGCONFIG_EXECUTABLE )

    # Find all the ROBOOP stuff with pkg-config
    PKGCONFIG( "roboop >= 1.31.1" ROBOOP_FOUND ROBOOP_INCLUDE_DIRS ROBOOP_DEFINES ROBOOP_LINK_DIRS ROBOOP_LIBS )

    if( ROBOOP_FOUND )
       message( STATUS "   Includes in: ${ROBOOP_INCLUDE_DIRS}")
       message( STATUS "   Libraries in: ${ROBOOP_LINK_DIRS}")
       message( STATUS "   Libraries: ${ROBOOP_LIBS}")
       message( STATUS "   Defines: ${ROBOOP_DEFINES}")
    endif( ROBOOP_FOUND )

else( CMAKE_PKGCONFIG_EXECUTABLE )

    # Can't find pkg-config -- have to search manually

endif( CMAKE_PKGCONFIG_EXECUTABLE )
