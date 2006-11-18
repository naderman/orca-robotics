INCLUDE( ${ORCA_CMAKE_DIR}/include_orcaice.cmake )

# Glob all source files
FILE( GLOB SRCS *.cpp )

# Define some stuff to fool Player code
ADD_DEFINITIONS( -DHAVE_STDINT_H )

ADD_LIBRARY( OrcaSegwayRmpUsb SHARED ${SRCS} )

INSTALL_TARGETS( /lib OrcaSegwayRmpUsb )
