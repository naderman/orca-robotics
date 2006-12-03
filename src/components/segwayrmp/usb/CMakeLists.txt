INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

# Glob all source files
FILE( GLOB SRCS *.cpp )

# Define some stuff to fool Player code
ADD_DEFINITIONS( -DHAVE_STDINT_H )

INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/components/segwayrmp/usb )

ADD_LIBRARY( OrcaSegwayRmpUsb SHARED ${SRCS} )
INSTALL_TARGETS( /lib OrcaSegwayRmpUsb )
TARGET_LINK_LIBRARIES( OrcaSegwayRmpUsb OrcaSegwayRmpUsbFtdi )
ADD_SUBDIRECTORY( usbftdi )
ADD_SUBDIRECTORY( test )