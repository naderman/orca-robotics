#
# Official dependency number 0: CMake.
# Obviously, we must have it if we are here but test version.
#
cmake_minimum_required( VERSION 2.6 FATAL_ERROR )

#
# Enter some information about the project
# This is the only configurable part of the build system
#
# project name
project( orca )

# project version
set( GBX_PROJECT_VERSION_MAJOR "2" )
set( GBX_PROJECT_VERSION_MINOR "13" )
set( GBX_PROJECT_VERSION_PATCH "+" )

set( GBX_PROJECT_VERSION 
    ${GBX_PROJECT_VERSION_MAJOR}.${GBX_PROJECT_VERSION_MINOR}.${GBX_PROJECT_VERSION_PATCH} )

#
# The rest is done by a script
#
include( cmake/internal/Setup.cmake )
