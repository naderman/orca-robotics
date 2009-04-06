#
# Official dependency number 0: CMake.
# Obviously, we must have it if we are here but test version.
#
CMAKE_MINIMUM_REQUIRED( VERSION 2.4 FATAL_ERROR )

#
# Enter some information about the project
# This is the only configurable part of the build system
#
# project name
PROJECT( orca )

# project version string
set( GBX_PROJECT_VERSION 2.13.0+ )

#
# The rest is done by a script
#
include( cmake/internal/bootstrap.cmake )
