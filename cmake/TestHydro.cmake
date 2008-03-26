# Test that Hydro has everything we need and works.
#
# This module assumes that Hydro is already found and HYDRO_HOME is defined
#
# Also assumes that hydro manifest is already loaded.
#
# Defines the following variables:
# HYDRO_WORKS : 1 if test passed, 0 otherwise.

# INCLUDE( ${CMAKE_ROOT}/Modules/CheckIncludeFileCXX.cmake )

IF( NOT DEFINED HYDRO_HOME )
    MESSAGE( STATUS "Testing Hydro - CANNOT test because HYDRO_HOME is not defined." )
    SET( HYDRO_WORKS 0 )
ENDIF( NOT DEFINED HYDRO_HOME )

IF( HYDROUTIL_INSTALLED AND HYDROICEUTIL_INSTALLED )
    SET( HYDRO_WORKS 1 )
ELSE ( HYDROUTIL_INSTALLED AND HYDROICEUTIL_INSTALLED )
    SET( HYDRO_WORKS 0 )
ENDIF( HYDROUTIL_INSTALLED AND HYDROICEUTIL_INSTALLED )
