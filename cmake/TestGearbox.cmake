# Test that Gearbox has everything we need (for Hydro) and those libs work.
#
# This module assumes that Gearbox is already found.
#
# Also assumes that Gearbox manifest is already loaded.
#
# Defines the following variables:
# GEARBOX_WORKS : 1 if test passed, 0 otherwise.

# include( ${CMAKE_ROOT}/Modules/CheckIncludeFileCXX.cmake )

if( NOT GEARBOX_FOUND )
    message( STATUS "Testing Gearbox - CANNOT test because gearbox was not found." )
    set( GEARBOX_WORKS 0 )
endif( NOT GEARBOX_FOUND )

if( GBXUTILACFR_INSTALLED AND GBXICEUTILACFR_INSTALLED )
    set( GEARBOX_WORKS 1 )
else( GBXUTILACFR_INSTALLED AND GBXICEUTILACFR_INSTALLED )
    set( GEARBOX_WORKS 0 )
endif( GBXUTILACFR_INSTALLED AND GBXICEUTILACFR_INSTALLED )
