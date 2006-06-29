#
# Test CMake version
#
CMAKE_MINIMUM_REQUIRED(VERSION 2.2.2)

###################################################
#                                                 #
#       Start project customization section       #
#                                                 #
###################################################

PROJECT(orca)

SET( VERSION 2.0.0rc5+ CACHE STRING "Version of Orca distribution" )
  MESSAGE( STATUS "Orca version ${VERSION}" )

SET( INSTALL_DIR $ENV{ORCA2_INSTALL} )
# If environment variable ORCA2_INSTALL is not set, use defaults
IF( NOT INSTALL_DIR )
  IF   ( NOT WIN32 )
    # Linux and friends: if environment variable ORCA2_INSTALL is not set, install into /opt/orca-<VERSION>
    SET( INSTALL_DIR /opt/orca-${VERSION} )
  ELSE ( NOT WIN32 )
    # Windows: install into C:\orca-<VERSION>
    SET( INSTALL_DIR C:/orca-${VERSION} )
  ENDIF ( NOT WIN32 )
ENDIF( NOT INSTALL_DIR )
SET( CMAKE_INSTALL_PREFIX ${INSTALL_DIR} CACHE PATH "Installation directory" FORCE )
MESSAGE( STATUS "Setting installation directory to ${CMAKE_INSTALL_PREFIX}" )

SET( PROJ_SOURCE_DIR ${orca_SOURCE_DIR} )
SET( PROJ_BINARY_DIR ${orca_BINARY_DIR} )

SET( PROJ_INTERFACE_LIB OrcaInterfaces )

###################################################
#                                                 #
#       End project customization section         #
#                                                 #
###################################################

#
# Set the build type (affects debugging symbols and optimization)
#
IF ( NOT CMAKE_BUILD_TYPE )
  IF ( NOT WIN32 )
    SET( CMAKE_BUILD_TYPE Release )
  ELSE ( NOT WIN32 )
    # windows... a temp hack: VCC does not seem to respect the cmake
    # setting and always defaults to debug, we have to match it here.
#    SET( CMAKE_BUILD_TYPE Debug )
  ENDIF ( NOT WIN32 )
  MESSAGE( STATUS "Setting build type to '${CMAKE_BUILD_TYPE}'" )
ELSE ( NOT CMAKE_BUILD_TYPE )
  MESSAGE( STATUS "Build type set to '${CMAKE_BUILD_TYPE}' by user." )
ENDIF ( NOT CMAKE_BUILD_TYPE )

###################################################
#                                                 #
#       Do some global set-up & checking          #
#                                                 #
###################################################

INCLUDE(config/global_setup.cmake)

###########################################################
#                                                         #
# Look for low-level C headers, write defines to config.h #
#                                                         #
###########################################################

# Only do this once.
IF ( NOT WROTE_CONFIG_H )
  MESSAGE( STATUS "Writing config.h" )
  INCLUDE(config/write_config_h.cmake)
  SET( WROTE_CONFIG_H TRUE CACHE INTERNAL 
       "Remember that we've already written config.h" )
ELSE ( NOT WROTE_CONFIG_H )
  MESSAGE( STATUS "Not writing config.h -- wrote it previously" )
ENDIF ( NOT WROTE_CONFIG_H )

###########################################################
#                                                         #
# Look for dependencies required by individual components #
#                                                         #
###########################################################

INCLUDE(config/check_depend.cmake)
SUBDIRS( src )


###########################################################
#                                                         #
# Enable testing by including the Dart module             #
#                                                         #
###########################################################

INCLUDE(Dart)
