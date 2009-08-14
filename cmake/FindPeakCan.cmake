# Confirm that libpcan library is installed
#
# This module defines
# PEAKCAN_FOUND, If false, don't try to use libpcan.so

include( ${CMAKE_ROOT}/Modules/CheckIncludeFileCXX.cmake )

# Look for the library, set the Variable if it exists
check_include_file_cxx( libpcan.h PEAKCAN_FOUND )
if( PEAKCAN_FOUND )
    message( STATUS "Looking for libpcan - found")
else( PEAKCAN_FOUND )
    message( STATUS "Looking for libpcan - not found")
endif( PEAKCAN_FOUND )
