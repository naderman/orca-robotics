#
# Do not include /config/component_rules.cmake here. It will lead to circular dependencies.
# alexm: is this true? component_rules does not link OrcaIce.
#

INCLUDE_DIRECTORIES( BEFORE
    ${ICE_HOME}/include
    ${PROJECT_BINARY_DIR}/src/interfaces/cpp
    ${PROJECT_SOURCE_DIR}/src/libs
    ${CMAKE_CURRENT_SOURCE_DIR}/detail
)

LINK_DIRECTORIES ( ${ICE_HOME}/lib )

IF ( NOT OS_WIN )
  ADD_DEFINITIONS( "-Wall" )
ELSE ( NOT OS_WIN )
  # windows: turn off deprication warnings  
  ADD_DEFINITIONS( "-Wall -D_CRT_SECURE_NO_DEPRECATE" )
ENDIF ( NOT OS_WIN )

#
# private classes which are not part of the public API
#
ADD_SUBDIRECTORY( detail )

FILE( GLOB SRCS *.cpp )

IF( NOT OS_WIN )
  GLOBAL_ADD_LIBRARY( OrcaIce SHARED ${SRCS} )
  TARGET_LINK_LIBRARIES( OrcaIce
              OrcaIceDetail
              Ice
              IceUtil
              IceStorm
              IceBox
              IceGrid
              OrcaInterfaces
              OrcaObjects
              Glacier2 )

ELSE ( NOT OS_WIN )
  # building shared libs is complicated in windows
  GLOBAL_ADD_LIBRARY( OrcaIce STATIC ${SRCS} )
  # windows... have to link to different libs depending on build type
  TARGET_LINK_LIBRARIES( OrcaIce
              optimized Ice optimized IceUtil optimized IceStorm optimized IceBox optimized IceGrid
              debug Iced debug IceUtild debug IceStormd debug IceBoxd debug IceGridd
              optimized Glacier2 debug Glacier2d
              OrcaIceDetail OrcaInterfaces )
ENDIF ( NOT OS_WIN )

# special dependency on the generated files
ADD_DEPENDENCIES( OrcaIce OrcaInterfaces )

INSTALL_FILES( /include/orcaice .*\\.h$ )

IF ( BUILD_TESTS )
    ADD_SUBDIRECTORY ( test )
ENDIF ( BUILD_TESTS )
