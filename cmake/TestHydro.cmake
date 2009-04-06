# Test that Hydro has everything we need and works.
#
# This module assumes that Hydro is already found.
#
# Also assumes that hydro manifest is already loaded.
#
# Defines the following variables:
# HYDRO_WORKS : 1 if test passed, 0 otherwise.

# include( ${CMAKE_ROOT}/Modules/CheckIncludeFileCXX.cmake )

if( NOT HYDRO_FOUND )
    message( STATUS "Testing Hydro - CANNOT test because hydro was not found." )
    set( HYDRO_WORKS 0 )
endif( NOT HYDRO_FOUND )

if( HYDROUTIL_INSTALLED AND HYDROICEUTIL_INSTALLED )
    set( HYDRO_WORKS 1 )
else( HYDROUTIL_INSTALLED AND HYDROICEUTIL_INSTALLED )
    set( HYDRO_WORKS 0 )
endif( HYDROUTIL_INSTALLED AND HYDROICEUTIL_INSTALLED )
