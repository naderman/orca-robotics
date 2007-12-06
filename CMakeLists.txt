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
SET( PROJECT_VERSION            2.8.0       CACHE STRING "Version of Orca distribution" )
# the name of the environment variable which sets installation directory
SET( PROJECT_INSTALL_ENV_VAR    ORCA_INSTALL )
# the name for project interface library
SET( PROJECT_INTERFACE_LIB      OrcaInterfaces )

#
# The rest is done by a script
#
INCLUDE( cmake/local/bootstrap.cmake )
