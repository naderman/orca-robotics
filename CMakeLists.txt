#
# Test CMake version
#

# Older versions have a different signature for CMAKE_MINIMUM_REQUIRED, check it manually just to make sure
IF ( ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} LESS 2.4 )
  MESSAGE( FATAL_ERROR 
           "Orca2 requires at least CMake v2.4. You are running v${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}. Please upgrade." )
ENDIF ( ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} LESS 2.4 )

# If we get this far, use the modern signature.  This will also cause newer
# CMake versions to try to be backwards-compatible with the desired version
CMAKE_MINIMUM_REQUIRED( VERSION 2.4 FATAL_ERROR )

#
# Enter some information about the project
# This is the only configurable part of the build system
#
# project name
PROJECT( orca )
# project version string
SET( PROJECT_VERSION            2.0.2+       CACHE STRING "Version of Orca distribution" )
# the name of the environment variable which sets installation directory
SET( PROJECT_INSTALL_ENV_VAR    ORCA2_INSTALL )
# the name for project interface library
SET( PROJECT_INTERFACE_LIB      OrcaInterfaces )

#
# The rest is done by a script
#
INCLUDE( cmake/local/bootstrap.cmake )
