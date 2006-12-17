INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

FILE( GLOB SRCS *.cpp )

ADD_LIBRARY( OrcaNovatelSpanInsGps SHARED ${SRCS} )

INSTALL_TARGETS( /lib OrcaNovatelSpanInsGps )
