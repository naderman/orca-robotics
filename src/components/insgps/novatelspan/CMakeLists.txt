INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/include_orcaice.cmake )

INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/utils )

FILE( GLOB SRCS *.cpp )

LINK_DIRECTORIES( ${PROJECT_BINARY_DIR}/src/util/serial )
LINK_DIRECTORIES( ${PROJECT_BINARY_DIR}/src/sandbox/insgps/novatel )

INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/sandbox/insgps )

ADD_LIBRARY( OrcaNovatelSpanInsGps SHARED ${SRCS} )
TARGET_LINK_LIBRARIES( OrcaNovatelSpanInsGps novatel )

INSTALL_TARGETS( /lib OrcaNovatelSpanInsGps )
