#
# Official dependency number 0: CMake.
# Obviously, we must have it if we are here but test its version.
# Note: 2.6.2 fails to properly import GearBox targets.
#
cmake_minimum_required( VERSION 2.6.3 FATAL_ERROR )

#
# Enter some information about the project
# This is the only configurable part of the build system
#
# project name
project( orca )

# project version
set( GBX_PROJECT_VERSION_MAJOR "9" )
set( GBX_PROJECT_VERSION_MINOR "11" )
set( GBX_PROJECT_VERSION_PATCH "+s" )

set( GBX_PROJECT_VERSION
    ${GBX_PROJECT_VERSION_MAJOR}.${GBX_PROJECT_VERSION_MINOR}.${GBX_PROJECT_VERSION_PATCH} )

#
# The rest is done by a script
#
include( cmake/internal/Setup.cmake )
