# Locate Player install directory

# This module defines
# PLAYER_HOME where to find include, lib, bin, etc.
# PLAYER_FOUND, If false, don't try to use Ice.

# using HYDRO_CMAKE_DIR makes this script not-portable
include( ${HYDRO_CMAKE_DIR}/FindPkgConfig.cmake )

if( CMAKE_PKGCONFIG_EXECUTABLE )

#    message( "Using pkgconfig" )
    
    # Find all the librtk stuff with pkg-config
    PKGCONFIG( "playerc++ >= 2.0" PLAYER_FOUND PLAYER_INCLUDE_DIRS PLAYER_DEFINES PLAYER_LINK_DIRS PLAYER_LIBS )

#     if( PLAYER_FOUND )
#        message( STATUS "   Includes in: ${PLAYER_INCLUDE_DIRS}")
#        message( STATUS "   Libraries in: ${PLAYER_LINK_DIRS}")
#        message( STATUS "   Libraries: ${PLAYER_LIBS}")
#        message( STATUS "   Defines: ${PLAYER_DEFINES}")
#     endif( PLAYER_FOUND )

else( CMAKE_PKGCONFIG_EXECUTABLE )

    # Can't find pkg-config -- have to search manually

endif( CMAKE_PKGCONFIG_EXECUTABLE )
