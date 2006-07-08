#
# Test CMake version
#
CMAKE_MINIMUM_REQUIRED( VERSION 2.4 FATAL_ERROR )
# Older versions have a different signature, check it manually just to make sure
IF ( ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} LESS 2.4 )
  MESSAGE( FATAL_ERROR 
           "Orca2 requires at least CMake v2.4. You are running v${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}. Please upgrade." )
ENDIF ( ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} LESS 2.4 )

#
# Enter some information about the project
# This is the only configurable part of the build system
#
# project name
PROJECT( orca )
# project version string
SET( PROJECT_VERSION            2.0.0rc5+       CACHE STRING "Version of Orca distribution" )
# the name for project interface library
SET( PROJECT_INTERFACE_LIB      OrcaInterfaces )
# the name of the environment variable which sets installation directory
SET( PROJECT_INSTALL_ENV_VAR    ORCA2_INSTALL )

#
# The rest is done by a script
#
INCLUDE( config/global_setup.cmake )
