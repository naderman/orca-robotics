# for now we assume that Ice is installed.
INCLUDE_DIRECTORIES( BEFORE
    ${ICE_HOME}/include
    ${PROJECT_SOURCE_DIR}/src/libs
)
LINK_DIRECTORIES(
    ${ICE_HOME}/lib
    ${ICE_HOME}/lib64
)

IF ( NOT OS_WIN )
  ADD_DEFINITIONS( "-Wall" )
ELSE ( NOT OS_WIN )
  # windows: turn off deprication warnings  
  ADD_DEFINITIONS( "-Wall -D_CRT_SECURE_NO_DEPRECATE" )
ENDIF ( NOT OS_WIN )

FILE( GLOB SRCS *.cpp )

IF( NOT OS_WIN )
  GLOBAL_ADD_LIBRARY( OrcaIceUtil SHARED ${SRCS} )
  TARGET_LINK_LIBRARIES( OrcaIceUtil IceUtil )
ELSE ( NOT OS_WIN )
  # building shared libs is complicated in windows
  GLOBAL_ADD_LIBRARY( OrcaIceUtil STATIC ${SRCS} )
  # windows... have to link to different libs depending on build type
  TARGET_LINK_LIBRARIES( OrcaIceUtil optimized IceUtil debug IceUtild )
ENDIF ( NOT OS_WIN )

INSTALL_FILES( /include/orcaiceutil .*\\.h$ )

IF ( BUILD_TESTS )
    ADD_SUBDIRECTORY ( test )
ENDIF ( BUILD_TESTS )
