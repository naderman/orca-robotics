# Confirm that libpcan library is installed

# This module defines
# PEAKCAN_FOUND, If false, don't try to use libpcan.so

# Look for the library, set the Variable if it exists
CHECK_INCLUDE_FILE_CXX( libpcan.h PEAKCAN_FOUND )
IF( PEAKCAN_FOUND )
    MESSAGE( STATUS "Looking for libpcan - found")
ELSE ( PEAKCAN_FOUND )
    MESSAGE( STATUS "Looking for libpcan - not found")
ENDIF( PEAKCAN_FOUND )
