INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/components/segwayrmp/rmpdriver )

# Glob all source files
FILE( GLOB SRCS *.cpp )

ADD_LIBRARY( OrcaSegwayRmpUsb SHARED ${SRCS} )
INSTALL_TARGETS( /lib OrcaSegwayRmpUsb )
TARGET_LINK_LIBRARIES( OrcaSegwayRmpUsb OrcaSegwayRmpUsbFtdi )

ADD_SUBDIRECTORY( usbftdi )

IF ( BUILD_TESTS )
    ADD_SUBDIRECTORY ( test )
ENDIF ( BUILD_TESTS )
