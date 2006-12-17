#
# Do not include /config/component_rules.cmake here. It will lead to circular dependencies.
# alexm: is this true? component_rules does not link OrcaIce.
#

# MESSAGE( STATUS "DEBUG: ICE_HOME=${ICE_HOME}" )

INCLUDE_DIRECTORIES( BEFORE
    ${ICE_HOME}/include
    ${PROJECT_BINARY_DIR}/src/interfaces/cpp
    ${PROJECT_SOURCE_DIR}/src/libs
)

LINK_DIRECTORIES ( ${ICE_HOME}/lib
                   ${PROJECT_BINARY_DIR}/src/interfaces/cpp/orca )

# pass the Orca release version to the library so it can report it through its API
IF ( NOT OS_WIN )
  # note: for gcc need to produce this in the Makefile: -DORCA_VERSION=\"X.Y.Z\", without
  # escaping the quotes the compiler will strip them off.
  ADD_DEFINITIONS( "-Wall -DORCA_VERSION=\\\"${PROJECT_VERSION}\\\"" )
ELSE ( NOT OS_WIN )
  # windows: turn off deprication warnings  
  ADD_DEFINITIONS( "-Wall -DORCA_VERSION=\"${PROJECT_VERSION}\" -D_CRT_SECURE_NO_DEPRECATE" )
ENDIF ( NOT OS_WIN )

FILE( GLOB SRCS *.cpp )

IF( NOT OS_WIN )
  GLOBAL_ADD_LIBRARY( OrcaIce SHARED ${SRCS} )
  TARGET_LINK_LIBRARIES( OrcaIce
              Ice IceUtil IceStorm IceBox IceGrid OrcaInterfaces )
ELSE ( NOT OS_WIN )
  # building shared libs is complicated in windows
  GLOBAL_ADD_LIBRARY( OrcaIce STATIC ${SRCS} )
  # windows... have to link to different libs depending on build type
  TARGET_LINK_LIBRARIES( OrcaIce
              optimized Ice optimized IceUtil optimized IceStorm optimized IceBox optimized IceGrid
              debug Iced debug IceUtild debug IceStormd debug IceBoxd debug IceGridd
              optimized Glacier2 debug Glacier2d
              OrcaInterfaces )
ENDIF ( NOT OS_WIN )

# special dependency on the generated files
ADD_DEPENDENCIES( OrcaIce OrcaInterfaces )

# not all header files need to be installed
# could create a 'private' directory but this seems easier
#INSTALL_FILES( /include/orcaice .*\\.h$ )
SET ( HEADER_FILES FILES
application.h
# autoconsumerI.h
buffer.h
# bufferedautoconsumerI.h
bufferedconsumerI.h
bufferedtimedconsumerI.h
common.h
component.h
configutils.h
connectutils.h
context.h
exceptions.h
#homeI.h
heartbeater.h
iceapplication.h
notify.h
orcaice.h
printutils.h
#privateutils.h
proputils.h
# proxiedautoconsumerI.h
proxiedconsumerI.h
proxy.h
ptrbuffer.h
ptrnotify.h
ptrproxy.h
service.h
#statustracerI.h
stringutils.h
sysutils.h
thread.h
timer.h
tracer.h
)
INSTALL_FILES( /include/orcaice ${HEADER_FILES} )

IF ( BUILD_TESTS )
    ADD_SUBDIRECTORY ( test )
ENDIF ( BUILD_TESTS )
