# Confirm that oping library is installed

# This module defines
# LIBOPING_FOUND, If false, don't try to use liboping.

FIND_LIBRARY( LIBOPING_FOUND NAMES oping PATHS "/usr/lib" )

# Can't use this because of size_t...
#check_include_file( oping.h LIBOPING_FOUND )

if( LIBOPING_FOUND )
    message( STATUS "Looking for oping - found")
else( LIBOPING_FOUND )
    message( STATUS "Looking for oping - not found")
endif( LIBOPING_FOUND )
