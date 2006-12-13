#
# Give feedback on custom entries
#
MESSAGE( STATUS "Setting project name to ${PROJECT_NAME}" )
MESSAGE( STATUS "Setting project version to ${PROJECT_VERSION}" )
MESSAGE( STATUS "Setting project interface lib name to ${PROJECT_INTERFACE_LIB}" )

###########################################################
#
# Project directories
#
###########################################################
SET( PROJECT_INSTALL_DIR $ENV{${PROJECT_INSTALL_ENV_VAR}} )
# If environment variable ORCA2_INSTALL is NOT set, use defaults
IF( NOT PROJECT_INSTALL_DIR )
  IF   ( NOT WIN32 )
    # Linux and friends: if environment variable ORCA2_INSTALL is not set, e.g. /opt/orca-1.2.3
    SET( PROJECT_INSTALL_DIR /opt/${PROJECT_NAME}-${PROJECT_VERSION} )
  ELSE ( NOT WIN32 )
    # Windows: e.g. C:\orca-1.2.3
    SET( PROJECT_INSTALL_DIR C:/${PROJECT_NAME}-${PROJECT_VERSION} )
  ENDIF ( NOT WIN32 )
ENDIF( NOT PROJECT_INSTALL_DIR )

# It's impossible in cmake to tell whether a variable was set by the system or by 
# the user. The hack we use is to check whether the installation directory is set 
# to cmake initial default (OS-dependent).
# btw: is there LINUX variable? or do we have to do this nonsense?
IF ( NOT WIN32 AND ${CMAKE_INSTALL_PREFIX} STREQUAL "/usr/local" )
    SET ( IS_INSTALL_PREFIX_UNTOUCHED 1 )
ENDIF ( NOT WIN32 AND ${CMAKE_INSTALL_PREFIX} STREQUAL "/usr/local" )
# windows side is untested
IF ( WIN32 AND ${CMAKE_INSTALL_PREFIX} STREQUAL "C:/Program Files" )
    SET ( IS_INSTALL_PREFIX_UNTOUCHED 1 )
ENDIF ( WIN32 AND ${CMAKE_INSTALL_PREFIX} STREQUAL "C:/Program Files" )

# Now, if it's still set to default, change it to our own prefered location. 
# Otherwise, it's probably set by us or by the user and we don't
# touch it again. The consequence of this is that the user CANNOT specify CMake 
# default installation directories.
IF ( ${IS_INSTALL_PREFIX_UNTOUCHED} )
    # Force our own default
    SET( CMAKE_INSTALL_PREFIX ${PROJECT_INSTALL_DIR} CACHE PATH "Installation directory" FORCE )
ENDIF ( ${IS_INSTALL_PREFIX_UNTOUCHED} )
MESSAGE( STATUS "Setting installation directory to ${CMAKE_INSTALL_PREFIX}" )

SET( PROJECT_SOURCE_DIR ${${PROJECT_NAME}_SOURCE_DIR} )
SET( PROJECT_BINARY_DIR ${${PROJECT_NAME}_BINARY_DIR} )

###########################################################
#
# Set the build type (affects debugging symbols and optimization)
#
###########################################################
IF ( NOT CMAKE_BUILD_TYPE )
  IF ( NOT WIN32 )
    SET( CMAKE_BUILD_TYPE Release )
  ELSE ( NOT WIN32 )
    # windows... a temp hack: VCC does not seem to respect the cmake
    # setting and always defaults to debug, we have to match it here.
    SET( CMAKE_BUILD_TYPE Debug )
  ENDIF ( NOT WIN32 )
  MESSAGE( STATUS "Setting build type to '${CMAKE_BUILD_TYPE}'" )
ELSE ( NOT CMAKE_BUILD_TYPE )
  MESSAGE( STATUS "Build type set to '${CMAKE_BUILD_TYPE}' by user." )
ENDIF ( NOT CMAKE_BUILD_TYPE )

#
# CMake seems not to set this property correctly for some reason
#
IF ( WIN32 )
    SET( EXE_EXTENSION ".exe" )
ENDIF ( WIN32 )

###########################################################
#
# Include local macro definitions
#
###########################################################
INCLUDE( ${ORCA_CMAKE_DIR}/orca_macros.cmake )

###########################################################
#
# Include os-specific definitions
#
###########################################################
INCLUDE( ${ORCA_CMAKE_DIR}/os.cmake )

###########################################################
#
# If we're using gcc, make sure the version is OK.
#
###########################################################
IF ( ${CMAKE_C_COMPILER} MATCHES gcc )

    EXEC_PROGRAM ( ${CMAKE_C_COMPILER} ARGS --version OUTPUT_VARIABLE GCC_VERSION )
    MESSAGE ( STATUS "gcc version: ${GCC_VERSION}")

    # Why doesn't this work?
    #STRING( REGEX MATCHALL "gcc\.*" VERSION_STRING ${CMAKE_C_COMPILER} )

    IF ( GCC_VERSION MATCHES ".*4\\.[0-9]\\.[0-9]" )
        SET ( GCC_VERSION_OK 1 )
    ENDIF ( GCC_VERSION MATCHES ".*4\\.[0-9]\\.[0-9]")

    ASSERT ( GCC_VERSION_OK
             "Checking gcc version - failed. Orca2 requires gcc v. 4.x"
             "Checking gcc version - ok"
             1 )
ENDIF ( ${CMAKE_C_COMPILER} MATCHES gcc )

###########################################################
#
# Find Ice installation
#
###########################################################
IF ( DEFINED ICE_HOME )
    # Ice home is specified with a command line option or it's already in cache
    MESSAGE( STATUS "Ice location was specified or using cached value: ${ICE_HOME}")
ELSE ( DEFINED ICE_HOME )
    # Find Ice installation
    INCLUDE ( ${ORCA_CMAKE_DIR}/FindIce.cmake )
    ASSERT( ICE_FOUND 
            "Looking for Ice - not found. Please install Ice, ** delete CMakeCache.txt **, then re-run CMake." 
            "Looking for Ice - found in ${ICE_HOME}" 
            1 )
ENDIF ( DEFINED ICE_HOME )

# Test Ice installation
INCLUDE ( ${ORCA_CMAKE_DIR}/TestIce.cmake )
ASSERT ( ICE_WORKS
         "Testing Ice - failed. Please check or reinstall it, ** delete CMakeCache.txt **, then re-run CMake."
         "Testing Ice - ok."
         1 )

###########################################################
#
# Defaults for big source code switches 
# (these are defaults. after the user modifies these in GUI they stay in cache)
#
###########################################################
OPTION( BUILD_JAVA    "Turn me on to enable compilation of all Java interfaces and components" OFF  )
OPTION( BUILD_TESTS   "Turn me off to disable compilation of all tests"               ON  )
OPTION( BUILD_SANDBOX "Turn me on to enable compilation of everything in the sandbox" OFF )

###########################################################
#                                                         #
# Look for low-level C headers, write defines to config.h #
#                                                         #
###########################################################
INCLUDE( ${ORCA_CMAKE_DIR}/write_config_h.cmake )

###########################################################
#                                                         #
# Look for dependencies required by individual components #
#                                                         #
###########################################################
INCLUDE( ${ORCA_CMAKE_DIR}/check_depend.cmake )

###########################################################
#
# Project-specific global setup
#
###########################################################
INCLUDE( ${PROJECT_SOURCE_DIR}/cmake/local/project_setup.cmake )

# Store the location of the command in cache
# (after the project-specific Find functions are defined ...)
FIND_DEFTOOLS( DEFTOOLS_HOME )
SET( ORCA_DEF2CFG_COMMAND ${DEFTOOLS_HOME}/def2cfg${EXE_EXTENSION} 
        CACHE PATH "Path to def2cfg executable." FORCE )
SET( ORCA_DEF2XML_COMMAND ${DEFTOOLS_HOME}/def2xml${EXE_EXTENSION} 
        CACHE PATH "Path to def2xml executable." FORCE )
SET( ORCA_DEF2XMLTEMPLATE_COMMAND ${DEFTOOLS_HOME}/def2xmltemplate${EXE_EXTENSION} 
        CACHE PATH "Path to def2xmltemplate executable." FORCE )
MESSAGE( STATUS "Using ${ORCA_DEF2CFG_COMMAND}" )

###########################################################
#
# Installation preferences
#
###########################################################

# CMake default is FALSE
# SET( CMAKE_SKIP_BUILD_RPATH TRUE )
# CMake default is FALSE
# SET( CMAKE_BUILD_WITH_INSTALL_RPATH TRUE )
SET( CMAKE_INSTALL_RPATH 
    ${CMAKE_INSTALL_PREFIX}/lib
    ${ICE_HOME}/lib )

# IF ( NOT ${PROJECT_NAME} MATCHES "orca" )
IF ( NOT ORCA_MOTHERSHIP )
    SET( CMAKE_INSTALL_RPATH  
        ${CMAKE_INSTALL_RPATH}
        ${ORCA2_HOME}/lib )
ENDIF ( NOT ORCA_MOTHERSHIP )
# ENDIF ( NOT ${PROJECT_NAME} MATCHES "orca" )

IF ( ORCA_MOTHERSHIP )
    # For orca project only, install CMake scripts
    ADD_SUBDIRECTORY ( cmake )
ENDIF ( ORCA_MOTHERSHIP )


###########################################################
#                                                         #
# Enable testing by including the Dart module             #
# (must be done *before* entering source directories      #
#                                                         #
###########################################################
INCLUDE (${CMAKE_ROOT}/Modules/Dart.cmake)
ENABLE_TESTING()

###########################################################
#                                                         #
# Enter the source tree                                   #
#                                                         #
###########################################################
ADD_SUBDIRECTORY ( src )

###########################################################
#                                                         #
# Print results of CMake activity                         #
#                                                         #
###########################################################
INCLUDE ( ${ORCA_CMAKE_DIR}/build_config_report.cmake )
