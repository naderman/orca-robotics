INCLUDE( ${PROJECT_SOURCE_DIR}/config/include_orcaice.cmake )

# Glob all source files
FILE( GLOB SRCS *.cpp )

# Define some stuff to fool Player code
ADD_DEFINITIONS( -DHAVE_STDINT_H )

ADD_LIBRARY( OrcaRobot2dSegwayRmpUsb SHARED ${SRCS} )

INSTALL_TARGETS( /lib OrcaRobot2dSegwayRmpUsb )
