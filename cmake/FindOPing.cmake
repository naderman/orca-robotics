# Confirm that oping library is installed

# This module defines
# LIBOPING_FOUND, If false, don't try to use liboping.

FIND_LIBRARY( LIBOPING_FOUND NAMES oping PATHS "/usr/lib" )

# Can't use this because of size_t...
#CHECK_INCLUDE_FILE( oping.h LIBOPING_FOUND )

IF ( LIBOPING_FOUND )
  MESSAGE( STATUS "Looking for oping - found")
ELSE ( LIBOPING_FOUND )
  MESSAGE( STATUS "Looking for oping - not found")
ENDIF ( LIBOPING_FOUND )
